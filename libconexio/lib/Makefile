CC=${CROSS_COMPILE}gcc
CFLAGS= -g -Wall
TARGET=libconexio_CMM920.so
VERSION=1.1.4
TARGET_ROOTFS := ${CPS_SDK_INSTALL_FULLDIR}/${CPS_SDK_ROOTFS}

ifeq ($(CC),gcc)
	INCLUDEPATH = -I.
	INSTALL_DIR=/usr/local/lib/
else
	INCLUDEPATH = -I${CPS_SDK_ROOTDIR}/lib/include
	INSTALL_DIR= ${TARGET_ROOTFS}/usr/local/lib/
endif

all:${TARGET}.${VERSION}
${TARGET}.${VERSION}:libconexio_CMM920.o libconexio_CMM920_wrap.o 

libconexio_CMM920.o: libconexio_CMM920_func.c
	${CC} libconexio_CMM920_func.c -c -fPIC -o libconexio_CMM920.o ${INCLUDEPATH}
libconexio_CMM920_wrap.o: libconexio_CMM920_wrap.c
	${CC} libconexio_CMM920_wrap.c -c -fPIC -o libconexio_CMM920_wrap.o ${INCLUDEPATH}
${TARGET}.${VERSION}:
	${CC} -shared -O2 -Wl,-soname,${TARGET} -o ${TARGET}.${VERSION} libconexio_CMM920.o libconexio_CMM920_wrap.o

install:
	cp ${TARGET}.${VERSION} ${INSTALL_DIR}
	@if [ "${CC}" = "gcc" ] ; then \
		ldconfig ; \
	else \
		CURRENTDIR=`pwd` ; \
		cd ${INSTALL_DIR} ; \
		if [ -f ${TARGET} ] ; then \
			rm ${TARGET} ; \
		fi; \
		ln -s ${TARGET}.${VERSION} ${TARGET} ; \
		cd ${CURRENTDIR} ; \
	fi

uninstall:
	rm ${INSTALL_DIR}/${TARGET}.*
	ldconfig

clean:
	rm -f *.o *.so *.so.*
