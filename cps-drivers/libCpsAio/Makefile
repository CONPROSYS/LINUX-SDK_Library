#CC = gcc
CC = arm-linux-gnueabi-gcc
TARGET=libCpsAio.so
OBJ=libcpsaio.o
SRC=libcpsaio.c
CFLAGS= -g -Wall
INCLUDE= -I$(CPS_SDK_ROOTDIR)/driver/cps-drivers/include
VERSION =1.0.0
TARGET_ROOTFS   := ${CPS_SDK_INSTALL_FULLDIR}/rootfs

all:$(OBJ) ${TARGET}

$(OBJ):	
	${CC} ${INCLUDE} ${LD_FLAGS} $(SRC) -c -fPIC -o $(OBJ)

$(TARGET):
	${CC} ${INCLUDE} ${LD_FLAGS}  -shared -O2 -Wl,-soname,$(TARGET) -o $(TARGET) $(OBJ) -lm

install:
	cp -p $(TARGET) $(TARGET_ROOTFS)/usr/local/lib/$(TARGET).$(VERSION)
	@if [ ! -f $(TARGET_ROOTFS)/usr/local/lib/$(TARGET) ]; then \
	( cd $(TARGET_ROOTFS)/usr/local/lib/ && ln -s $(TARGET).$(VERSION) $(TARGET) ) \
	fi

release_copy:
	cp -p ./$(TARGET) ../../release/$(TARGET).$(VERSION)

clean:
	rm -f $(OBJ) $(TARGET)
