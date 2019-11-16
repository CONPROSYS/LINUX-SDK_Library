subdirall = cps-drivers SerialFunc libconexio libCpsEeprom
ifeq ($(CPS_SDK_PRODUCT_TYPE),CPS-MCS341-DSX)
  subdirs = cps-drivers 
endif
ifeq ($(CPS_SDK_PRODUCT_TYPE),CPS-MCS341G-DSX)
  subdirs = cps-drivers 
endif
ifeq ($(CPS_SDK_PRODUCT_TYPE),CPS-MC341Q-ADSCX)
  subdirs = SerialFunc libconexio
endif
ifeq ($(CPS_SDK_PRODUCT_TYPE),CPS-MCS341Q-DSX)
  subdirs = cps-drivers SerialFunc libconexio
endif
subdirs += libCpsEeprom
MAKE=make --no-print-directory 

all:	
	@for subdir in $(subdirs) ; do \
	(cd $$subdir && $(MAKE)) || exit 1;\
	done

install:
	@for subdir in $(subdirs) ; do \
	(cd $$subdir && $(MAKE) install ) || exit 1 ;\
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
	(cd $$subdir && $(MAKE) clean ) || exit 1 ;\
	done
	rm -f release/*
