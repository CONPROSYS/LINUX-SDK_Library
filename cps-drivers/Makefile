subdirs = libCpsAio libCpsDio libCpsSsi libCpsCnt
MAKE=make --no-print-directory 

TARGET_ROOTFS   := ${CPS_SDK_INSTALL_FULLDIR}/${CPS_SDK_ROOTFS}

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

sdk_install:
	cp -p include/*  $(TARGET_ROOTFS)/usr/local/include

clean:
	@for subdir in $(subdirs) ; do \
	(cd $$subdir && $(MAKE) clean ) ;\
	done
