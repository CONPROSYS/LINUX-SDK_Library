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
    char *serialdev = "/dev/ttyO3";
    int value = 0;
    char data;

    fd = Serial_PortOpen_Full(serialdev, 115200, 8, 1, 0, 700, 1, 0);

    if (!fd) {
        fprintf(stderr, "Error while initializing %s.\n", serialdev);
        return 1;
    }

    if(*argv[1] == '0')
    {
    	Serial_Set_Rts( fd, 0 );
    	printf("SetRTS 0\n");
    }
    else if(*argv[1] == '1')
    {
    	Serial_Set_Rts( fd, 1 );
    	printf("SetRTS 1\n");
    }
    else
    {
		printf("Please any key enter(input 'e' exit)\n");

    	do {
    		Serial_Set_Rts( fd, value );
    		printf("SetRTS %d\n",value);
    		data = getchar();
    		value ^= 1;
    	} while (data != 'e');
    }

    Serial_PortClose( fd );
    return 0;
}
