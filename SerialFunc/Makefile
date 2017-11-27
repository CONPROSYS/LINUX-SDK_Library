CC=${CROSS_COMPILE}gcc
CFLAGS= -g -Wall

TARGETLIBNAME = libSerialFunc
TARGET=$(TARGETLIBNAME).so
VERSION=1.0.1
TARGET_ROOTFS := ${CPS_SDK_INSTALL_FULLDIR}/${CPS_SDK_ROOTFS}

ifeq ($(CC),gcc)
	INCLUDEPATH = -I
	INSTALL_DIR=/usr/local
else
	INCLUDEPATH = -I${CPS_SDK_ROOTDIR}/lib/include
	INSTALL_DIR= ${TARGET_ROOTFS}/usr/local
endif

all:libSerialFunc.o ${TARGET}

libSerialFunc.o:	
	${CC} libserialfunc.c -c -fPIC -o libSerialFunc.o ${INCLUDEPATH}

${TARGET}:
	${CC} -shared -O2 -Wl,-soname,${TARGET} -o ${TARGET} libSerialFunc.o

sdk_install: 
	cp -p ../include/serialfunc.h ${INSTALL_DIR}/include

install:
	cp ${TARGET} ${INSTALL_DIR}/lib/${TARGET}.${VERSION}
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

sdk_uninstall: uninstall
	rm ${INSTALL_DIR}/include/serialfunc.h

uninstall:
	rm ${INSTALL_DIR}/lib/${TARGET}.so.*
	ldconfig

clean:
	rm -f *.o *.so *.so.*
