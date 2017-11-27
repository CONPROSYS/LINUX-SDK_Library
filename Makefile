subdirall = cps-drivers SerialFunc libconexio libCpsEeprom
ifeq ($(CPS_SDK_PRODUCT_TYPE),CPS-MCS341-DSX)
  subdirs = cps-drivers 
endif
ifeq ($(CPS_SDK_PRODUCT_TYPE),CPS-MC341Q-ADSCX)
  subdirs = SerialFunc libconexio
endif
subdirs += libCpsEeprom
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

sdk_install:
	@for subdir in $(subdirs) ; do \
	(cd $$subdir && $(MAKE) sdk_install ) ;\
	done

clean:
	@for subdir in $(subdirall) ; do \
	(cd $$subdir && $(MAKE) clean ) ;\
	done
	rm -f release/*
