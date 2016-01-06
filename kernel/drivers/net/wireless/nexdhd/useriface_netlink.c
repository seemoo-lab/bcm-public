#include <net/genetlink.h>
#include <linux/module.h>
#include <linux/kernel.h>

/* attributes (variables): the index in this enum is used as a reference for the type,
 *             userspace application has to indicate the corresponding type
 *             the policy is used for security considerations 
 */
enum {
	DOC_EXMPL_A_UNSPEC,
	DOC_EXMPL_A_MSG,
        __DOC_EXMPL_A_MAX,
};
#define DOC_EXMPL_A_MAX (__DOC_EXMPL_A_MAX - 1)

/* attribute policy: defines which attribute has which type (e.g int, char * etc)
 * possible values defined in net/netlink.h 
 */
static struct nla_policy doc_exmpl_genl_policy[DOC_EXMPL_A_MAX + 1] = {
	[DOC_EXMPL_A_MSG] = { .type = NLA_NUL_STRING },
};

#define VERSION_NR 1
/* family definition */
static struct genl_family doc_exmpl_gnl_family = {
	.id = GENL_ID_GENERATE,         //genetlink should generate an id
	.hdrsize = 0,
	.name = "CONTROL_EXMPL",        //the name of this family, used by userspace application
	.version = VERSION_NR,                   //version number  
	.maxattr = DOC_EXMPL_A_MAX,
};

/* commands: enumeration of all commands (functions), 
 * used by userspace application to identify command to be ececuted
 */
enum {
	DOC_EXMPL_C_UNSPEC,
	DOC_EXMPL_C_ECHO,
	__DOC_EXMPL_C_MAX,
};
#define DOC_EXMPL_C_MAX (__DOC_EXMPL_C_MAX - 1)


/* an echo command, receives a message, prints it and sends another message back */
int
doc_exmpl_echo(struct sk_buff *skb_2, struct genl_info *info)
{
        struct nlattr *na;
        struct sk_buff *skb;
        int rc;
	void *msg_head;
	char * mydata;
	
        if (info == NULL)
                goto out;
  
        /*for each attribute there is an index in info->attrs which points to a nlattr structure
         *in this structure the data is given
         */
        na = info->attrs[DOC_EXMPL_A_MSG];
       	if (na) {
		mydata = (char *)nla_data(na);
		if (mydata == NULL)
			printk("error while receiving data\n");
		else
			printk("received: %s\n", mydata);
		}
	else
		printk("no info->attrs %i\n", DOC_EXMPL_A_MSG);

        /* send a message back*/
        /* allocate some memory, since the size is not yet known use NLMSG_GOODSIZE*/	
        skb = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (skb == NULL)
		goto out;

	/* create the message headers */
        /* arguments of genlmsg_put: 
           struct sk_buff *, 
           int (sending) pid, 
           int sequence number, 
           struct genl_family *, 
           int flags, 
           u8 command index (why do we need this?)
        */
       	msg_head = genlmsg_put(skb, 0, info->snd_seq+1, &doc_exmpl_gnl_family, 0, DOC_EXMPL_C_ECHO);
	if (msg_head == NULL) {
		rc = -ENOMEM;
		goto out;
	}
	/* add a DOC_EXMPL_A_MSG attribute (actual value to be sent) */
	rc = nla_put_string(skb, DOC_EXMPL_A_MSG, "hello world from kernel space");
	if (rc != 0)
		goto out;
	
        /* finalize the message */
	genlmsg_end(skb, msg_head);

        /* send the message back */
	//rc = genlmsg_unicast(skb,info->snd_pid );
	rc = genlmsg_unicast(genl_info_net(info), skb,info->snd_pid );
	if (rc != 0)
		goto out;
	return 0;

 out:
        printk("an error occured in doc_exmpl_echo:\n");
  
      return 0;
}
/* commands: mapping between the command enumeration and the actual function*/
struct
genl_ops doc_exmpl_gnl_ops_echo = {
	.cmd = DOC_EXMPL_C_ECHO,
	.flags = 0,
	.policy = doc_exmpl_genl_policy,
	.doit = doc_exmpl_echo,
	.dumpit = NULL,
};

int
useriface_netlink_init(void)
{
	int rc;
        printk("INIT GENERIC NETLINK EXEMPLE MODULE\n");
        
        /*register new family*/
	rc = genl_register_family(&doc_exmpl_gnl_family);
	if (rc != 0)
		goto failure;
        /*register functions (commands) of the new family*/
	rc = genl_register_ops(&doc_exmpl_gnl_family, &doc_exmpl_gnl_ops_echo);
	if (rc != 0){
                printk("register ops: %i\n",rc);
                genl_unregister_family(&doc_exmpl_gnl_family);
		goto failure;
        }

	return 0;
	
  failure:
        printk("an error occured while inserting the generic netlink example module\n");
	return -1;
	
	
}

void
useriface_netlink_exit(void)
{
        int ret;
        printk("EXIT GENERIC NETLINK EXEMPLE MODULE\n");
        /*unregister the functions*/
	ret = genl_unregister_ops(&doc_exmpl_gnl_family, &doc_exmpl_gnl_ops_echo);
	if(ret != 0){
                printk("unregister ops: %i\n",ret);
                return;
        }
        /*unregister the family*/
	ret = genl_unregister_family(&doc_exmpl_gnl_family);
	if(ret !=0){
                printk("unregister family %i\n",ret);
        }
}

