-include version
ifndef VERSION
VERSION = 0.0.1
endif

CC = ${CROSS_COMPILE}gcc
CFLAGS = -g -Wall
TARGETLIBNAME = libCpsEeprom
TARGET = $(TARGETLIBNAME).so
TARGET_ROOTFS := ${CPS_SDK_INSTALL_FULLDIR}/${CPS_SDK_ROOTFS}

ifeq ($(CC),gcc)
	INCLUDEPATH = 
	INSTALL_DIR = /usr/local
else
	INCLUDEPATH = -I${CPS_SDK_ROOTDIR}/lib/include
	INSTALL_DIR = ${TARGET_ROOTFS}/usr/local
endif

all: ${TARGET}

${TARGET}: $(TARGETLIBNAME).o
	${CC} -shared -O2 -Wl,-soname,${TARGET} -o ${TARGET} $(TARGETLIBNAME).o

$(TARGETLIBNAME).o:	$(TARGETLIBNAME).c
	${CC} $(TARGETLIBNAME).c -c -fPIC -o $(TARGETLIBNAME).o ${INCLUDEPATH}

sdk_install: 
	cp -p $(TARGETLIBNAME).h ${INSTALL_DIR}/include

install:
	cp $(TARGET) ${INSTALL_DIR}/lib/$(TARGET).$(VERSION)
	@if [ "${CC}" = "gcc" ] ; then \
		ldconfig ; \
	else \
		CURRENTDIR=`pwd` ; \
		cd ${INSTALL_DIR}/lib ; \
		if [ -f ${TARGET} ] ; then \
			rm ${TARGET} ; \
		fi; \
		ln -s ${TARGET}.${VERSION} ${TARGET} ; \
		cd ${CURRENTDIR} ; \
	fi

sdk_uninstall: 
	rm ${INSTALL_DIR}/include/$(TARGETLIBNAME).h

uninstall:
	rm ${INSTALL_DIR}/lib/${TARGET}*
	ldconfig

clean:
	rm -f *.o *.so *.so.*

