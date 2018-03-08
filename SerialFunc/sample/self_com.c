
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

#include "serialfunc.h"

int main(int argc, char **argv){

	if(argc < 2){
		printf("self_com [device] [options]\n");
		printf("Ver 1.0.1\n");
		printf("options:\n");
		printf(" -b[Baudrate]\n");
		printf(" -l[Length]\n");
		printf(" -s[StopBit]\n");
		printf(" -p[Parity]\n");
		printf(" -c[Count]\n");
		printf(" -d[Data]\n");
		printf(" -w[Wait(ms)]\n");
		printf(" -Format=[Format(ASC or HEX)]\n");
		printf(" -f[FlowControl]  :full duplex\n");
		printf("     -fn : FlowControl None\n");
		printf("     -fr : FlowControl RTS/CTS\n");
		printf(" -h  :half duplex(default)\n");

		return -1;
	}
	
	int iBaudrate = 9600;
	int iLength = 7;
	int iStop = 2;
	int iParity = 1;
	
	int iCnt = 5;
	int iFlow = 0;
	int iWait = 700;
	char cMsg[512]={0};
	sprintf(cMsg, "0123456789abcdefghijklmnopqrstuvwxyz");
	
	int i=2;
	int j=0;

	int ret = 0;

	int dup = 0;

	char cFormat[4]={0};
	int iFormat = 0;

	//����
	while(i < argc){
		printf("[%d] %s\n", i, argv[i]);
		
		//Baudrate
		if(strncmp(argv[i], "-b", strlen("-b")) == 0){
			if(sscanf(argv[i], "-b%d", &iBaudrate) != 1){
				ret = -1;
			}
		}

		//Length
		if(strncmp(argv[i], "-l", strlen("-l")) == 0){
			if(sscanf(argv[i], "-l%d", &iLength) != 1){
				ret = -1;
			}
		}
		
		//Stop
		if(strncmp(argv[i], "-s", strlen("-s")) == 0){
			if(sscanf(argv[i], "-s%d", &iStop) != 1){
				ret = -1;
			}
		}
		
		//Parity
		if(strncmp(argv[i], "-p", strlen("-p")) == 0){
			if(sscanf(argv[i], "-p%d", &iParity) != 1){
				ret = -1;
			}
		}
		
		//Cnt
		if(strncmp(argv[i], "-c", strlen("-c")) == 0){
			if(sscanf(argv[i], "-c%d", &iCnt) != 1){
				ret = -1;
			}
		}
		
		//Wait
		if(strncmp(argv[i], "-w", strlen("-w")) == 0){
			if(sscanf(argv[i], "-w%d", &iWait) != 1){
				ret = -1;
			}
		}
		
		//Data
		if(strncmp(argv[i], "-d", strlen("-d")) == 0){
			if(strlen(argv[i]) > 2){
				strcpy(cMsg, &argv[i][2]);
			}
		}

		//duplex
		if(strncmp(argv[i], "-f", strlen("-f")) == 0){
			dup = 1;
			//Hardware Control
			if(strncmp(argv[i], "-fn", strlen("-fn")) == 0){
				iFlow = 0;
			}
			else if(strncmp(argv[i], "-fr", strlen("-fr")) == 0){
				iFlow = 1;
			}
		}
		if(strncmp(argv[i], "-h", strlen("-h")) == 0){
			dup = 0;
		}

		if(strncmp(argv[i], "-Format=", strlen("-Format=")) == 0){
			if(sscanf(argv[i], "-Format=%s", cFormat) != 1){
				ret = -1;
			}else{
				if(strncmp(cFormat, "ASC", strlen("ASC")) == 0){
					iFormat = 0;
				}else if(strncmp(cFormat, "HEX", strlen("HEX")) == 0){
					iFormat = 1;
				}
			}
		}

		i++;
	}
	
	if(ret){
		printf("param error\n");
		return ret;
	}
	
	printf("%s, %d, %d, %d %d %d\n", argv[1], iBaudrate, iLength, iStop, iParity, iFlow);
	printf("Wait = %dms Cnt = %d\n", iWait, iCnt);

	if( iFormat == 0) {
		printf("Data = %s\n", cMsg);
	}else if(iFormat == 1){
		printf("Data = ");
		for(j = 0;j < strlen(cMsg);j ++){
			printf("%-2x ", cMsg[j]);
		}
		printf("\n");
	}	
	int iPort = 0;
	
	if(dup == 0){
	
		iPort = Serial_PortOpen(argv[1], iBaudrate, iLength, iStop, iParity, iWait, 1);
		if(iPort < 0){
			printf("open error\n");
			return -1;
		}
	
	}else{
	
		iPort = Serial_PortOpen_Full(argv[1], iBaudrate, iLength, iStop, iParity, iWait, 1, iFlow);
		if(iPort < 0){
			printf("open error\n");
			return -1;
		}
	
	
	}


	
	char cRecv[512]={0};
	
	for(i = 0; i< iCnt; i++){
		//��ǂ�
		(void)Serial_GetString(iPort, (unsigned char*)&cRecv[0], sizeof(cRecv));

		//Serial_PutString(iPort, cMsg, sizeof(cMsg));
		Serial_PutString(iPort, (unsigned char*)&cMsg[0], strlen(cMsg) * sizeof(char));
		usleep(iWait * 1000);
		Serial_GetString(iPort, (unsigned char*)&cRecv[0], sizeof(cRecv));
		if( iFormat == 0 ){
			printf("Put = %s, Recv = %s\n", cMsg, cRecv);
		}else if( iFormat == 1 ){
			printf("Put = ");
			for(j = 0;j < strlen(cMsg);j ++){
				printf("%-2x ", cMsg[j]);
			}
			printf("\n\nRecv = ");
			for(j = 0;j < strlen(cRecv);j ++){
				printf("%-2x ", cRecv[j]);
			}
			printf("\n\n");
		}
		//��v���Ȃ�
		if(strcmp(cMsg, cRecv) != 0){
			ret = -1;
		}
	}
	
	if(ret){
		printf("NG\n");
	}else{
		printf("OK\n");
	}
	
	Serial_PortClose(iPort);
	
	return ret;
}
