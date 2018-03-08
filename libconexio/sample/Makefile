#CC = gcc #selfbuild
CC=arm-linux-gnueabi-gcc #crossbuild
VERSION=1.1.1

LIBDIR=/usr/arm-linux-gnueabi/lib/

all:conexio_CMM920_send.o conexio_CMM920_recv.o

conexio_CMM920_send.o:	
	${CC} -L${LIBDIR} -o CMM920SendTest_${VERSION} conexio_CMM920_send.c -lconexio_CMM920 -lSerialFunc

conexio_CMM920_recv.o:	
	${CC} -L${LIBDIR} -o CMM920RecvTest_${VERSION} conexio_CMM920_recv.c -lconexio_CMM920 -lSerialFunc

clean:
	rm -f *.o CMM920*Test_*
