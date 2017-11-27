
#define _LIBCPSEEPROM_C_
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libCpsEeprom.h"

/* EEPROM offset address define */
#define EEPROMADDR_HEADER_INFO					0
#define EEPROMADDR_BOARD_NAME					4
#define EEPROMADDR_CPU_MEMORY_INFO				12
#define EEPROMADDR_DISPLAY_OUTPUT				13
#define EEPROMADDR_ETHERNET_MAC					14
#define EEPROMADDR_SPREAD_SPECTRUM_CLOCKING		26
#define EEPROMADDR_SERIAL_PORT_FOR_DEBUG		27
#define EEPROMADDR_SERIAL_NUMBER				28
#define EEPROMADDR_AI_CARIBRATION				42
#define EEPROMADDR_AO_CARIBRATION				74
#define EEPROMADDR_BOARD_REVISION				106
#define EEPROMADDR_FPGA_REVISION				108
#define EEPROMADDR_NO_CARE						112

static char cEepromFilePath[256] = "/sys/bus/i2c/devices/1-0050/eeprom";	/* EEPROM file path */

/*
 * @brief read EEPROM data
 */
static int ReadEEPROMdata(
char *buf)	/* (OUT) Ši”[ƒoƒbƒtƒ@256byte Stored buffer 256byte */
{
	int fd = -1;
	int readedsize=0;
	int retcd = -1;

	fd = open(cEepromFilePath, O_RDONLY);
	if (fd < 0) {
		return retcd;
	}

	readedsize = read(fd, buf, 256);
	if (readedsize == 256) {
		retcd = 0;
	}

	close(fd);

	return retcd;
}

/*
 * @brief Set EEPROM file path
 * 
 */
int SetEepromFilePath(
const char *cFilePath) /* (IN) Set File pathname */
{
	strcpy(cEepromFilePath, cFilePath);
	return 0;
}

/*
 * @brief Get EEPROM Header Info 
 * 
 */
int GetEepromHeaderInfo(	/* return code 0:Success -1:Error */
char *outbuf)	/* (OUT) Ši”[ƒoƒbƒtƒ@ Stored buffer (4byte)*/
{
	char eeprombuf[256];
	int ret;

	ret = ReadEEPROMdata(eeprombuf);
	if (ret == 0) {
		memcpy(outbuf, &eeprombuf[EEPROMADDR_HEADER_INFO], 4);
	}

	return ret;
}

/*
 * @brief Get Board Name
 * 
 */
int GetEepromBoardName(	/* return code 0:Success -1:Error */
char *cBoardName)	/* (OUT) BOARD Name (9byte or more)*/
{
	char eeprombuf[256];
	int ret;
	int i;

	ret = ReadEEPROMdata(eeprombuf);
	if (ret == 0) {
		memcpy(cBoardName, &eeprombuf[EEPROMADDR_BOARD_NAME], 8);
		for (i=0; i<8; i++) {
			if (cBoardName[i] < 0x20 || cBoardName[i] > 0x7f) {
				cBoardName[i] = 0;
			}
		}
		cBoardName[i] = 0;
	}
	
	return ret;
}

/*
 * @brief Get CPU Clock Info
 * 
 */
int GetEepromCPUInfo(	/* return code 0:Success -1:Error */
int *iCpuInfo)	/* (OUT) CPU Clock Info 0x8:1GHz, 0x4:800MHz, 0x1:600MHz */
{
	char eeprombuf[256];
	int ret;

	ret = ReadEEPROMdata(eeprombuf);
	if (ret == 0) {
		*iCpuInfo = (eeprombuf[EEPROMADDR_CPU_MEMORY_INFO] >> 4) & 0xf;
	}

	return ret;
}

/*
 * @brief Get Memory Info
 * 
 */
int GetEepromMemoryInfo(	/* return code 0:Success -1:Error */
int *iCpuInfo)	/* (OUT) CPU Clock Info 0x2:512Mbyte, 0x0:256Mbyte, 0x1:128Mbyte */
{
	char eeprombuf[256];
	int ret;

	ret = ReadEEPROMdata(eeprombuf);
	if (ret == 0) {
		*iCpuInfo = eeprombuf[EEPROMADDR_CPU_MEMORY_INFO] & 0xf;
	}

	return ret;
}

/*
 * @brief Get Ethernet MAC Address
 * 
 */
int GetEepromEthernetMAC(	/* return code 0:Success -1:Error */
int iNum,		/* (IN) Ethernet Number (0: Ether#0, 1:Ether #1 */
char *cEthernetMAC)	/* (OUT) Ethernet MAC (6byte) */
{
	char eeprombuf[256];
	int ret;

	ret = ReadEEPROMdata(eeprombuf);
	if (ret == 0) {
		if (iNum == 0 || iNum == 1) {
			memcpy(cEthernetMAC, &eeprombuf[EEPROMADDR_ETHERNET_MAC+(6*iNum)], 6);
		}
		else {
			ret = -1;
		}
	}

	return ret;
}

/*
 * @brief Get Serial Number
 * 
 */
int GetEepromSerialNumber(	/* return code 0:Success -1:Error */
char *cSerialNumber)	/* (OUT) BOARD Name (9byte or more)*/
{
	char eeprombuf[256];
	int ret;
	int i;

	ret = ReadEEPROMdata(eeprombuf);
	if (ret == 0) {
		memcpy(cSerialNumber, &eeprombuf[EEPROMADDR_SERIAL_NUMBER], 13);
		for (i=0; i<13; i++) {
			if (cSerialNumber[i] < 0x20 || cSerialNumber[i] > 0x7f) {
				cSerialNumber[i] = 0;
			}
		}
		cSerialNumber[i] = 0;
	}
	
	return ret;
}

/*
 * @brief Get AI Caribration Data
 * 
 */
int GetEepromAICaribrationData(	/* return code 0:Success -1:Error */
short *sOffet,	/* (OUT) Offset Data (8 channnel) */
short *sGain)	/* (OUT) Gain Data  (8 channnel) */
{
	char eeprombuf[256];
	int ret;
	int i;
	int addr;

	ret = ReadEEPROMdata(eeprombuf);
	if (ret == 0) {
		for (i=0; i<8; i++) {
			addr = EEPROMADDR_AI_CARIBRATION + (i * 4);
			sOffet[i] = ((eeprombuf[addr] << 8) + eeprombuf[addr + 1]);
			sGain[i] = ((eeprombuf[addr + 2] << 8) + eeprombuf[addr + 3]);
		}
	}

	return ret;
}

/*
 * @brief Get AO Caribration Data
 * 
 */
int GetEepromAOCaribrationData(	/* return code 0:Success -1:Error */
short *sOffet,	/* (OUT) Offset Data (8 channnel) */
short *sGain)	/* (OUT) Gain Data  (8 channnel) */
{
	char eeprombuf[256];
	int ret;
	int i;
	int addr;

	ret = ReadEEPROMdata(eeprombuf);
	if (ret == 0) {
		for (i=0; i<8; i++) {
			addr = EEPROMADDR_AO_CARIBRATION + (i * 4);
			sOffet[i] = ((eeprombuf[addr] << 8) + eeprombuf[addr + 1]);
			sGain[i] = ((eeprombuf[addr + 2] << 8) + eeprombuf[addr + 3]);
		}
	}

	return ret;
}

/*
 * @brief Get Board Revision
 * 
 */
int GetEepromBoardRevision(	/* return code 0:Success -1:Error */
short *sRevision)	/* (OUT) Board Revision */
{
	char eeprombuf[256];
	int ret;

	ret = ReadEEPROMdata(eeprombuf);
	if (ret == 0) {
		*sRevision = ((eeprombuf[EEPROMADDR_BOARD_REVISION] << 8) + eeprombuf[EEPROMADDR_BOARD_REVISION + 1]);
	}

	return ret;
}

/*
 * @brief Get FPGA Revision
 * 
 */
int GetEepromFPGARevision(	/* return code 0:Success -1:Error */
long *lRevision)	/* (OUT) FPGA Revision */
{
	char eeprombuf[256];
	int ret;
	int	year, month, day;

	ret = ReadEEPROMdata(eeprombuf);
	if (ret == 0) {
		year = (((eeprombuf[EEPROMADDR_FPGA_REVISION] >> 4) & 0xf) * 1000) + (eeprombuf[EEPROMADDR_BOARD_REVISION] & 0xf * 100) + (((eeprombuf[EEPROMADDR_FPGA_REVISION+1] >> 4) & 0xf) * 10) + (eeprombuf[EEPROMADDR_BOARD_REVISION+1] & 0xf) ;
		month = (((eeprombuf[EEPROMADDR_FPGA_REVISION+2]  >> 4) & 0xf) * 10) + (eeprombuf[EEPROMADDR_BOARD_REVISION+2] & 0xf);
		day = (((eeprombuf[EEPROMADDR_FPGA_REVISION+3]  >> 4) & 0xf) * 10) + (eeprombuf[EEPROMADDR_BOARD_REVISION+3] & 0xf);
		*lRevision = (year * 10000) + (month * 100) + day;
	}

	return ret;
}
