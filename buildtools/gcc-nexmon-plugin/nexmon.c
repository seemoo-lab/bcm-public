#include <plugin.h>
#include <tree.h>
#include <print-tree.h>
#include <stdio.h>

static tree handle_nexmon_place_at_attribute(tree *node, tree name, tree args, int flags, bool *no_add_attr);

int plugin_is_GPL_compatible = 1;

static const char *objfile = "patch.o";
static const char *fwfile = "fw_bcmdhd.bin";
static const char *ldfile = "/dev/null";
static const char *makefile = "/dev/null";
static unsigned int ramstart = 0x180000;
static FILE *ld_fp, *make_fp;

static struct attribute_spec user_attr =
{
	.name = "at",
	.min_length = 1,
	.max_length = 2,
	.decl_required = true,
	.type_required = false,
	.function_type_required = false,
	.handler = handle_nexmon_place_at_attribute,
	.affects_type_identity = false,
};

static tree
handle_nexmon_place_at_attribute(tree *node, tree name, tree args, int flags, bool *no_add_attr)
{
	tree itr;

	const char *decl_name = IDENTIFIER_POINTER(DECL_NAME(*node));
	//const char *attr_name = IDENTIFIER_POINTER(name);
	//const char *param1_str = TREE_STRING_POINTER(TREE_VALUE(args));
	unsigned int addr = (unsigned int) strtol(TREE_STRING_POINTER(TREE_VALUE(args)), NULL, 0);
	bool is_dummy = false;
	if(TREE_CHAIN(args) != NULL_TREE) {
		is_dummy = !strcmp(TREE_STRING_POINTER(TREE_VALUE(TREE_CHAIN(args))), "dummy");
	}

	printf("decl_name: %s\n", decl_name);

	//printf("attr_name: %s\n", attr_name);

	//printf("align: %d\n", DECL_COMMON_CHECK (*node)->decl_common.align);
	if (DECL_COMMON_CHECK (*node)->decl_common.align == 32 && (addr & 1))
		DECL_COMMON_CHECK (*node)->decl_common.align = 8;

	if (DECL_COMMON_CHECK (*node)->decl_common.align == 32 && (addr & 2))
		DECL_COMMON_CHECK (*node)->decl_common.align = 16;
	//printf("align: %d\n", DECL_COMMON_CHECK (*node)->decl_common.align);

	for(itr = args; itr != NULL_TREE; itr = TREE_CHAIN(itr)) {
		printf("arg: %s %08x\n", TREE_STRING_POINTER(TREE_VALUE(itr)), (unsigned int) strtol(TREE_STRING_POINTER(TREE_VALUE(itr)), NULL, 0));
		//debug_tree(itr);
		//debug_tree(TREE_VALUE(itr));
	}

	if (is_dummy) {
		fprintf(ld_fp, ".text.dummy.%s 0x%08x : { %s (.*.%s) }\n", decl_name, addr, objfile, decl_name);
	} else {
		fprintf(ld_fp, ".text.%s 0x%08x : { KEEP(%s (.*.%s)) }\n", decl_name, addr, objfile, decl_name);
		fprintf(make_fp, "\t$(CC)objcopy -O binary -j .text.%s $< section.generated.bin && dd if=section.generated.bin of=$@ bs=1 conv=notrunc seek=$$((0x%08x - 0x%08x))\n", decl_name, addr, ramstart);
	}

	//debug_tree(*node);
	//debug_tree(name);
	return NULL_TREE;
}

static void
register_attributes(void *event_data, void *data)
{
	register_attribute(&user_attr);
}

static void
handle_plugin_finish(void *event_data, void *data)
{
	fprintf(make_fp, "\nFORCE:\n");

	fclose(ld_fp);
	fclose(make_fp);
}

int
plugin_init(struct plugin_name_args *info, struct plugin_gcc_version *ver)
{
	int i = 0;
	for (i = 0; i < info->argc; i++) {
		if (!strcmp(info->argv[i].key, "objfile")) {
			objfile = info->argv[i].value;
		} else if (!strcmp(info->argv[i].key, "ldfile")) {
			ldfile = info->argv[i].value;
		} else if (!strcmp(info->argv[i].key, "makefile")) {
			makefile = info->argv[i].value;
		} else if (!strcmp(info->argv[i].key, "fwfile")) {
			fwfile = info->argv[i].value;
		} else if (!strcmp(info->argv[i].key, "ramstart")) {
			ramstart = (unsigned int) strtol(info->argv[i].value, NULL, 0);
		}
	}

	unlink(ldfile);
	unlink(makefile);

	ld_fp = fopen(ldfile, "a");

	if (!ld_fp) {
		fprintf(stderr, "gcc_nexmon_plugin: Linker file not writeable! (error)\n");
		return -1;
	}

	make_fp = fopen(makefile, "a");

	if (!make_fp) {
		fprintf(stderr, "gcc_nexmon_plugin: Make file not writeable! (error)\n");
		return -1;
	}

	fprintf(make_fp, "%s: patch.elf FORCE\n", fwfile);

	register_callback("nexmon", PLUGIN_ATTRIBUTES, register_attributes, NULL);
	register_callback("nexmon", PLUGIN_FINISH, handle_plugin_finish, NULL);

	return 0;
}