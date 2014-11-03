
SUBDIRS = team_assign_1

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@
	cp team_assign_1/decide.o ./

