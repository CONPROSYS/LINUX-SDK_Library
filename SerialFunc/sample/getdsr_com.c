#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "serialfunc.h"


int main(int argc, char **argv)
{
	int fd;
	int value = 0;
	char data;

	if( argc < 2 ){
		printf("getdsr_com [device]\n");
		printf("Ver 1.0.0\n");

		return -1;
	}



	fd = Serial_PortOpen_Full(argv[1], 115200, 8, 1, 0, 700, 1, 0);

	if (!fd) {
	fprintf(stderr, "Error while initializing %s.\n", argv[1]);
	return 1;
	}

	printf("Please any key enter(input 'e' exit)\n");

	do {
		value = Serial_Get_Dsr( fd );
		printf("Get Dsr %d\n",value);
		data = getchar();
		value ^= 1;
	} while (data != 'e');

	Serial_PortClose( fd );
	return 0;
}
