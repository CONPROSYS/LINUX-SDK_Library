-include version
ifndef VERSION
VERSION = 1.2.3
endif

#set architecture
ifeq "${ARCH}" ""
 ifeq "${CPS_SDK_TARGET_NAME}" ""
  # without CONPROSYS Linux SDK
  ARCH=arm
  CROSS_COMPILE=arm-linux-gnueabi-
 else
  # CONPROSYS Linux SDK
  ARCH=$(shell arch)
 endif
endif

ifeq "${CROSS_COMPILE}" "arm-linux-gnueabihf-"
	HF_OPT=hf
endif

CC=${CROSS_COMPILE}gcc
LD=${CROSS_COMPILE}ld
TARGET=libCpsAio.so
OBJ=libcpsaio.o
SRC=libcpsaio.c
CFLAGS= -g -Wall -DCONPROSYS_MAKEFILE_VERSION=${VERSION}
INCLUDE= -I$(CPS_SDK_ROOTDIR)/driver/cps-drivers/include -I$(CPS_SDK_ROOTDIR)/lib/cps-drivers/include
TARGET_ROOTFS   := ${CPS_SDK_INSTALL_FULLDIR}/${CPS_SDK_ROOTFS}


all: $(TARGET)

$(TARGET): $(OBJ)
	${CC} ${INCLUDE} ${LD_FLAGS}  -shared -O2 -Wl,-soname,$(TARGET) -o $(TARGET) $(OBJ) -lm

$(OBJ):	$(SRC) ../include/libcpsaio.h
	${CC} ${INCLUDE} ${LD_FLAGS} $(SRC) -c -fPIC -o $(OBJ)

install:
	cp -p $(TARGET) $(TARGET_ROOTFS)/usr/local/lib/$(TARGET).$(VERSION)
	@if [ ! -f $(TARGET_ROOTFS)/usr/local/lib/$(TARGET) ]; then \
	( cd $(TARGET_ROOTFS)/usr/local/lib/ && ln -s $(TARGET).$(VERSION) $(TARGET) ) \
	fi

release_copy:
	cp -p ./$(TARGET) $(CPS_SDK_ROOTDIR)/release/$(ARCH)$(HF_OPT)/$(TARGET).$(VERSION)

clean:
	rm -f $(OBJ) $(TARGET)
