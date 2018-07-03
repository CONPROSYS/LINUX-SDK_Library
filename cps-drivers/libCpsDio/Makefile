CC=${CROSS_COMPILE}gcc
TARGET=libCpsDio.so
OBJ=libcpsdio.o
SRC=libcpsdio.c
CFLAGS= -g -Wall
INCLUDE= -I$(CPS_SDK_ROOTDIR)/driver/cps-drivers/include -I$(CPS_SDK_ROOTDIR)/lib/cps-drivers/include
VERSION =1.0.6
TARGET_ROOTFS   := ${CPS_SDK_INSTALL_FULLDIR}/${CPS_SDK_ROOTFS}

all:$(OBJ) ${TARGET}

$(OBJ):	$(SRC) ../include/libcpsdio.h
	${CC} ${INCLUDE} ${LD_FLAGS} $(SRC) -c -fPIC -o $(OBJ)

$(TARGET): $(OBJ)
	${CC} ${INCLUDE} ${LD_FLAGS}  -shared -O2 -Wl,-soname,$(TARGET) -o $(TARGET) $(OBJ)

install:
	cp -p $(TARGET) $(TARGET_ROOTFS)/usr/local/lib/$(TARGET).$(VERSION)
	@if [ ! -f $(TARGET_ROOTFS)/usr/local/lib/$(TARGET) ]; then \
	( cd $(TARGET_ROOTFS)/usr/local/lib/ && ln -s $(TARGET).$(VERSION) $(TARGET) ) \
	fi

release_copy:
	cp -p ./$(TARGET) $(CPS_SDK_ROOTDIR)/release/$(TARGET).$(VERSION)

clean:
	rm -f $(OBJ) $(TARGET)
