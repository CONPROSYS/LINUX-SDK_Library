#CC = gcc #selfbuild
#LD = ld
CC=arm-linux-gnueabi-gcc #crossbuild
LD = arm-linux-gnueabi-ld
CFLAGS= -g -Wall
VERSION=1.0.7

TARGET=libSerialFunc.so
OBJ=libserialfunc.o
SRC=libserialfunc.c

# CPS_SDK_MACRO_CHECK ( without CONPROSYS SDK )
ifneq "$(CPS_SDK_INSTALL_FULLDIR)" ""
TARGET_ROOTFS   := ${CPS_SDK_INSTALL_FULLDIR}/rootfs
endif

ifneq "$(CPS_SDK_ROOTDIR)" ""
CPS_LIB_REELASE_DIR:=../../release
else
CPS_LIB_REELASE_DIR:=./
endif


all:$(OBJ) ${TARGET}

$(OBJ):	
	${CC} ${INCLUDE} ${LD_FLAGS} $(SRC) -c -fPIC -o $(OBJ) -lm

$(TARGET):
	${CC} ${INCLUDE} ${LD_FLAGS}  -shared -O2 -Wl,-soname,$(TARGET) -o $(TARGET) $(OBJ) -lm

install:
	cp -p $(TARGET) $(TARGET_ROOTFS)/usr/local/lib/$(TARGET).$(VERSION)
	@if [ ! -f $(TARGET_ROOTFS)/usr/local/lib/$(TARGET) ]; then \
	( cd $(TARGET_ROOTFS)/usr/local/lib/ && ln -s $(TARGET).$(VERSION) $(TARGET) ) \
	fi

release_copy:
	cp -p ./$(TARGET) $(CPS_LIB_REELASE_DIR)/$(TARGET).$(VERSION)

clean:
	rm -f $(OBJ) $(TARGET)

