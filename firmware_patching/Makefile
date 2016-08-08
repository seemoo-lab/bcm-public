SUBDIRS := $(filter-out include/., $(wildcard */.))

all: $(SUBDIRS)

$(SUBDIRS):
	make -C $@

newproject:
	cp -R template_project $(NEWPROJECT) \
	   && sed -i "s#template_project#$(NEWPROJECT)#g" $(NEWPROJECT)/bcmdhd/Makefile

cloneproject:
	cp -R $(OLDPROJECT) $(NEWPROJECT) \
	   && sed -i "s#$(OLDPROJECT)#$(NEWPROJECT)#g" $(NEWPROJECT)/bcmdhd/Makefile

moveproject:
	mv $(OLDPROJECT) $(NEWPROJECT) \
	   && sed -i "s#$(OLDPROJECT)#$(NEWPROJECT)#g" $(NEWPROJECT)/bcmdhd/Makefile

.PHONY: all $(SUBDIRS)
