SUBDIRS := $(filter-out include/., $(wildcard */.))

all: $(SUBDIRS)

$(SUBDIRS):
	make -C $@

.PHONY: all $(SUBDIRS)
