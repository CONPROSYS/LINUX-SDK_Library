/*
 * This file is libCpsEeprom Header
 */
#ifndef _LIBCPSEEPROM_H_
#define _LIBCPSEEPROM_H_

#ifndef _LIBCPSEEPROM_C_
#define EXTERN extern
#else
#define EXTERN 
#endif

/*
 * @brief Set EEPROM file path
 * 
 */
EXTERN int SetEepromFilePath(const char *cFilePath);
/*
 * @brief Get EEPROM Header Info 
 * 
 */
EXTERN int GetEepromHeaderInfo(char *outbuf);
/*
 * @brief Get Board Name
 * 
 */
EXTERN int GetEepromBoardName(char *cBoardName);
/*
 * @brief Get CPU Clock Info
 * 
 */
EXTERN int GetEepromCPUInfo(	/* return code 0:Success -1:Error */
int *iCpuInfo);	/* (OUT) CPU Clock Info 0x8:1GHz, 0x4:800MHz, 0x1:600MHz */

/*
 * @brief Get Memory Info
 * 
 */
EXTERN int GetEepromMemoryInfo(	/* return code 0:Success -1:Error */
int *iCpuInfo);	/* (OUT) CPU Clock Info 0x2:512Mbyte, 0x0:256Mbyte, 0x1:128Mbyte */

/*
 * @brief Get Ethernet MAC Address
 * 
 */
EXTERN int GetEepromEthernetMAC(	/* return code 0:Success -1:Error */
int iNum,		/* (IN) Ethernet Number (0: Ether#0, 1:Ether #1 */
char *cEthernetMAC);	/* (OUT) Ethernet MAC (7byte or more) */

/*
 * @brief Get Serial Number
 * 
 */
EXTERN int GetEepromSerialNumber(	/* return code 0:Success -1:Error */
char *cSerialNumber);	/* (OUT) BOARD Name (14byte or more)*/

/*
 * @brief Get AI Caribration Data
 * 
 */
EXTERN int GetEepromAICaribrationData(	/* return code 0:Success -1:Error */
short *sOffet,	/* (OUT) Offset Data (8 channnel) */
short *sGain);	/* (OUT) Gain Data  (8 channnel) */

/*
 * @brief Get AO Caribration Data
 * 
 */
EXTERN int GetEepromAOCaribrationData(	/* return code 0:Success -1:Error */
short *sOffet,	/* (OUT) Offset Data (8 channnel) */
short *sGain);	/* (OUT) Gain Data  (8 channnel) */

/*
 * @brief Get Board Revision
 * 
 */
EXTERN int GetEepromBoardRevision(	/* return code 0:Success -1:Error */
short *sRevision);	/* (OUT) Board Revision */

/*
 * @brief Get FPGA Revision
 * 
 */
EXTERN int GetEepromFPGARevision(	/* return code 0:Success -1:Error */
long *lRevision);	/* (OUT) FPGA Revision */

#endif
