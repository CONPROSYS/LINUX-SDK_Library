CC=${CROSS_COMPILE}gcc
CFLAGS= -g -Wall
TARGETLIBNAME = libconexio_CMM920
TARGET=$(TARGETLIBNAME).so
VERSION=1.1.1
TARGET_ROOTFS := ${CPS_SDK_INSTALL_FULLDIR}/${CPS_SDK_ROOTFS}

ifeq ($(CC),gcc)
	INCLUDEPATH = -I.
	INSTALL_DIR=/usr/local
else
	INCLUDEPATH = -I${CPS_SDK_ROOTDIR}/lib/include
	INSTALL_DIR= ${TARGET_ROOTFS}/usr/local
endif

all:${TARGET}
${TARGET}:libconexio_CMM920.o libconexio_CMM920_wrap.o 

libconexio_CMM920.o: libconexio_CMM920_func.c
	${CC} libconexio_CMM920_func.c -c -fPIC -o libconexio_CMM920.o ${INCLUDEPATH}
libconexio_CMM920_wrap.o: libconexio_CMM920_wrap.c
	${CC} libconexio_CMM920_wrap.c -c -fPIC -o libconexio_CMM920_wrap.o ${INCLUDEPATH}
${TARGET}:
	${CC} -shared -O2 -Wl,-soname,${TARGET} -o ${TARGET} libconexio_CMM920.o libconexio_CMM920_wrap.o

sdk_install: 
	cp -p ../include/$(TARGETLIBNAME).h ${INSTALL_DIR}/include

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
	rm ${INSTALL_DIR}/include/$(TARGETLIBNAME).h

uninstall:
	rm ${INSTALL_DIR}/lib/${TARGET}.*
	ldconfig

clean:
	rm -f *.o *.so *.so.*
