subdirs = cps-drivers
MAKE=make --no-print-directory -e

all:	
	@for subdir in $(subdirs) ; do \
	(cd $$subdir && $(MAKE)) ;\
	done

install:
	@for subdir in $(subdirs) ; do \
	(cd $$subdir && $(MAKE) install ) ;\
	done

release_copy:
	@for subdir in $(subdirs) ; do \
	(cd $$subdir && $(MAKE) release_copy ) ;\
	done

clean:
	@for subdir in $(subdirs) ; do \
	(cd $$subdir && $(MAKE) clean ) ;\
	done
	rm -f release/*
