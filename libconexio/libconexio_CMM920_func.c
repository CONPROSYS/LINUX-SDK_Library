/***

	libconexio_CMM920_func.c - conexio_CMM920_functions library
	
	Copyright (C) 2015 Tomoyuki Niimi, Syunsuke Okamoto.<okamoto@contec.jp>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, see
   <http://www.gnu.org/licenses/>.  

	  update 2016.01.08 (1) Fixed memory leak error.
 	                    (2) Fixed Initialize value error.
                      (3) Fixed return length of RecvCommandAck function.
    update 2016.01.11 (1) Added Lsi sub functions.
                      (2) Added MHR functions.
                      (3) Fixed crc parameter to add RecvTelegramSingleHop function.
    update 2016.01.15 (1) Fixed [WRITE or READ flag] byte offset 3, with Lsi function. 
                      (2) Fixed sequence number added MHR.
                      (3) Fixed change value name , from panid to shortAddr.
                      (4) Fixed change value name,  from send_size to size.
***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include "libconexio_CMM920.h"
#include "serialfunc.h"

#if 1
#define DbgPrint(fmt...)	printf(fmt)
#else
#define DbgPrint(fmt...)	do { } while (0)
#endif

#if 0
#define DbgAllocFreeCheck(fmt...)	printf(fmt)
#else
#define DbgAllocFreeCheck(fmt...)	do { } while (0)
#endif

#if 0
#define DbgDataLength(fmt...)	printf(fmt)
#else
#define DbgDataLength(fmt...)	do { } while (0)
#endif

#if 0
#define DbgRecvTelegramParam(fmt...)	printf(fmt)
#else
#define DbgRecvTelegramParam(fmt...)	do { } while (0)
#endif

int iWait;


static int global_seq_num = 0;
static int iPort;
static short global_getLastError = 0;

/***
	@function _conexio_cmm920_check_read_or_write
***/
static int _conexio_cmm920_check_read_or_write( int iFlag )
{
	switch( iFlag ){
	case CONEXIO_CMM920_SET_READING_READ:
	case CONEXIO_CMM920_SET_READING_WRITE:
		return 0;
	default:
		return 1;
	}
}

BYTE _calc_Hex2Bcd( BYTE hex ){

	return ( ( hex / 16 ) * 10 + ( hex % 16 ) );

}

BYTE _calc_Bcd2Hex( BYTE bcd ){

	return ( ( bcd / 10 ) * 16 + ( bcd % 10 ) );

}

BYTE _conexio_cmm920_dBm2Hex( int dBm ){
	dBm = dBm * (-1);
	if( dBm >= 100 ){
		dBm -= 100;
	}
	return _calc_Bcd2Hex( (BYTE)dBm );
}

int _conexio_cmm920_Hex2dBm( BYTE hex ){
	int dBm;
	dBm = (int) _calc_Hex2Bcd( hex );
	
	if( dBm <= 10 ){
		dBm += 100;
	}
	dBm = dBm * (-1);
	return dBm;
}

int _conexio_cmm920_send_recv( BYTE Data[], int size ,int mode, int command ){
	int iRet, send_size = size;
	int offset = 0; // 2016.01.15 (1) add

	// 2016.01.15 (1) start
	if( mode == CONEXIO_CMM920_MODE_COMMON && 
		command == CONEXIO_CMM920_SET_LSI 
	){
		offset = 3;
	}
	// 2016.01.15 (1) end

	if( Data[offset] == CONEXIO_CMM920_SET_READING_READ ){
		send_size = 1;
	}

	iRet = SendCommand(	Data, send_size,	mode, command );

	if( iRet < 0 )	return (-16 * 1) + iRet;

	usleep(iWait);

	//iRet = RecvCommandAck(Data, &send_size,	mode, command );
	iRet = RecvCommandAck(Data, &size,	mode, command ); // 2016.01.15 (4)
	if( iRet < 0 )	return (-16 * 2) + iRet;
	
	return iRet;
}

// CONEXIO 920MHz Initialize
int conexio_cmm920_init(char *PortName){

	int iBaudrate = 115200;
	int iLength = 8;
	int iStop = 1;
	int iParity = 0;

	iPort = Serial_PortOpen_Full(
		PortName,
		iBaudrate,
		iLength,
		iStop,
		iParity,
		100 ,
		0
	);

	if(iPort < 0){
		return 1;
	}

	iWait = 50000;

	return 0;
}

//Exit処理
int conexio_cmm920_exit()
{
	if(iPort == 0) return 0;

	Serial_PortClose(iPort);

	return 0;
}

// GetLastError

int conexio_cmm920_get_last_error(){

	return global_getLastError;
}

// reset
int conexio_cmm920_reset()
{
	int iRet;
	DbgPrint("conexio_cmm920_reset A\n");

	BYTE Data[] = {0x01};
	int Size = 1;

	iRet = _conexio_cmm920_send_recv(
		Data,
		Size,
		CONEXIO_CMM920_MODE_COMMON,
		CONEXIO_CMM920_RESET
	);

	if( iRet ){
		DbgPrint("<conexio_cmm920_reset>:Sending Error\n");
	}

	return iRet;

}

// mode
int conexio_cmm920_mode(BYTE isWrite , int *code)
{
	int iRet;
	BYTE Data[2];
	int Size = 2;

	DbgPrint("conexio_cmm920_mode code:%x \n", *code );

	if( _conexio_cmm920_check_read_or_write( isWrite ) ){
		DbgPrint("<conexio_cmm920_mode>:Parameter Error\n");
		return -1;
	}

	Data[0] = isWrite;

	if( isWrite == CONEXIO_CMM920_SET_READING_WRITE ){
		switch( *code ){
		case CONEXIO_CMM920_SET_MODE_SETTING:
		case CONEXIO_CMM920_SET_MODE_RUN:
		case CONEXIO_CMM920_SET_MODE_TEST:
			break;
		case CONEXIO_CMM920_SET_MODE_STOP:
			tcflush( iPort, TCIOFLUSH );
			break;
		default:
			DbgPrint("<conexio_cmm920_mode>:Parameter Error\n");
			return -1;		
		}
		Data[1] = *code;
	}

	iRet = _conexio_cmm920_send_recv(
		Data,
		Size,
		CONEXIO_CMM920_MODE_COMMON,
		CONEXIO_CMM920_SET_MODE
	);

	if( iRet < 0 ){
		DbgPrint("<conexio_cmm920_mode>:Communication Error %x\n", iRet);
	}
	else if( iRet > 0 )
	{
		DbgPrint("<conexio_cmm920_mode>:Result Error : %x\n", iRet );
	}
	else
	{
		if( Data[0] == CONEXIO_CMM920_SET_READING_READ ){
			*code = Data[1];
		}
	}

	return iRet;

}

// Address
int conexio_cmm920_address(BYTE isWrite, unsigned short *panId, BYTE Addr[], unsigned short *shortAddr )
{
	int iRet;
	BYTE Data[13];
	int Size = 13;
	int i;

	DbgPrint("conexio_cmm920_address_setting PAN ID %x Short Addr : %x\n", *panId, *shortAddr);


	if( _conexio_cmm920_check_read_or_write( isWrite ) ){
		DbgPrint("<conexio_cmm920_address>:Parameter Error\n");
		return -1;
	}

	Data[0] = isWrite;

	if( isWrite == CONEXIO_CMM920_SET_READING_WRITE ){
		Data[1] = ( *panId & 0xFF00 )>> 8;
		Data[2] = ( *panId & 0xFF );
		for( i = 0 ; i < 8; i ++ ){
			Data[3 + i] = Addr[i];
		}
		// 2016.01.15 (3) start
		Data[11] = ( *shortAddr & 0xFF00 )>> 8;
		Data[12] = ( *shortAddr & 0xFF );
		// 2016.01.15 (3) end
	}

	iRet = _conexio_cmm920_send_recv(
		Data,
		Size,
		CONEXIO_CMM920_MODE_COMMON,
		CONEXIO_CMM920_SET_ADDRESS
	);

	if( iRet < 0 ){
		DbgPrint("<conexio_cmm920_address>:Communication Error %x\n", iRet);
	}
	else if( iRet > 0 )
	{
		DbgPrint("<conexio_cmm920_address>:Result Error : %x\n", iRet );
	}
	else
	{
		if( Data[0] == CONEXIO_CMM920_SET_READING_READ ){
			*panId = (Data[1] << 8 ) | Data[2];
			for( i = 0; i < 8; i ++){
				Addr[i] = Data[ 3 + i ];
			}
			*shortAddr = (Data[11] << 8 ) | Data[12];
		}
	}

	return iRet;

}

// Wireless ( bitrate ***kbps, channel **ch, Power **mW )
int conexio_cmm920_wireless(BYTE isWrite, BYTE *bitrate, BYTE *channel, BYTE *power, char *sendSenceLvl,
	 char *recvSenceLvl, unsigned short *sendSenceTim, BYTE *sendSenceNum, BYTE *ackRetryNum, unsigned short *ackWaitTim)
{
	int iRet;
	BYTE Data[12];
	int Size = 12;
	int i;

	DbgPrint("conexio_cmm920_wireless bitrate %x channel:%x Power : %x \n", *bitrate, *channel, *power);


	if( _conexio_cmm920_check_read_or_write( isWrite ) ){
		DbgPrint("<conexio_cmm920_address>:Parameter Error\n");
		return -1;
	}

	Data[0] = isWrite;

	if( isWrite == CONEXIO_CMM920_SET_READING_WRITE ){
		Data[1] = *bitrate;
		Data[2] = *channel;
		Data[3] = *power;
		if( *sendSenceLvl > (-82) || *sendSenceLvl < (-110) ){
			Data[4] = 0xFF;
		}else{
			Data[4] = _conexio_cmm920_dBm2Hex( *sendSenceLvl );
		}
		if( *recvSenceLvl > (-10) || *recvSenceLvl < (-110) ){
			Data[5] = 0xFF;
		}else{
			Data[5] = _conexio_cmm920_dBm2Hex( *recvSenceLvl );
		}
		Data[6] = (*sendSenceTim & 0xFF00 ) >> 8;
		Data[7] = (*sendSenceTim & 0xFF );
		Data[8] = *sendSenceNum;
		Data[9] = *ackRetryNum;
		Data[10] = (*ackWaitTim & 0xFF00 ) >> 8;
		Data[11] = (*ackWaitTim & 0xFF );
	}

	iRet = _conexio_cmm920_send_recv(
		Data,
		Size,
		CONEXIO_CMM920_MODE_COMMON,
		CONEXIO_CMM920_SET_WIRELESS
	);

	if( iRet < 0 ){
		DbgPrint("<conexio_cmm920_address>:Communication Error %x\n", iRet);
	}
	else if( iRet > 0 )
	{
		DbgPrint("<conexio_cmm920_address>:Result Error : %x\n", iRet );
	}
	else
	{
		if( Data[0] == CONEXIO_CMM920_SET_READING_READ ){
			*bitrate = Data[1];
			*channel = Data[2];
			*power = Data[3];
			if( Data[4] != 0xFF ){
				*sendSenceLvl = _conexio_cmm920_Hex2dBm(Data[4]);
			}else{
				*sendSenceLvl = Data[4];
			}
			if( Data[5] != 0xFF ){
				*recvSenceLvl = _conexio_cmm920_Hex2dBm(Data[5]);
			}else{
				*recvSenceLvl = Data[5];
			}
			*sendSenceTim = ( Data[6] << 8 ) | Data[7];
			*sendSenceNum = Data[8];
			*ackRetryNum = Data[9];
			*ackWaitTim = ( Data[10] << 8 ) | Data[11];
		}
	}

	return iRet;
}

// timer 
int conexio_cmm920_timer( BYTE isWrite, unsigned short *tim )
{

	int iRet;
	BYTE Data[3];
	int Size = 3;
	int i;

	DbgPrint("conexio_cmm920_timer Timer : %d\n", *tim);


	if( _conexio_cmm920_check_read_or_write( isWrite ) ){
		DbgPrint("<conexio_cmm920_timer>:Parameter Error\n");
		return -1;
	}

	Data[0] = isWrite;

	if( isWrite == CONEXIO_CMM920_SET_READING_WRITE ){
		
		Data[1] = (*tim & 0xFF00 ) >> 8;
		Data[2] = (*tim & 0xFF );
	}

	iRet = _conexio_cmm920_send_recv(
		Data,
		Size,
		CONEXIO_CMM920_MODE_COMMON,
		CONEXIO_CMM920_SET_TIMER
	);

	if( iRet < 0 ){
		DbgPrint("<conexio_cmm920_timer>:Communication Error %x\n", iRet);
	}
	else if( iRet > 0 )
	{
		DbgPrint("<conexio_cmm920_timer>:Result Error : %x\n", iRet );
	}
	else
	{
		if( Data[0] == CONEXIO_CMM920_SET_READING_READ ){
			*tim = ( Data[1] << 8 ) | Data[2];
		}
	}

	return iRet;
}

// Auto Ack Frame
int conexio_cmm920_auto_ack_frame( BYTE isWrite, unsigned short *phr, unsigned char *fc_upper )
{

	int iRet;
	BYTE Data[4];
	int Size = 4;
	int i;

	DbgPrint("conexio_cmm920_auto_ack_frame PHR : %x FC[15:8] : %x\n", *phr, *fc_upper);


	if( _conexio_cmm920_check_read_or_write( isWrite ) ){
		DbgPrint("<conexio_cmm920_auto_ack_frame>:Parameter Error\n");
		return -1;
	}

	Data[0] = isWrite;

	if( isWrite == CONEXIO_CMM920_SET_READING_WRITE ){
		Data[1] = (*phr & 0xFF00 ) >> 8;
		Data[2] = (*phr & 0xFF );
		Data[3] = *fc_upper;
	}

	iRet = _conexio_cmm920_send_recv(
		Data,
		Size,
		CONEXIO_CMM920_MODE_COMMON,
		CONEXIO_CMM920_SET_ACK
	);

	if( iRet < 0 ){
		DbgPrint("<conexio_cmm920_auto_ack_frame>:Ack Error\n");
	}
	else if( iRet > 0 )
	{
		DbgPrint("<conexio_cmm920_auto_ack_frame>:Result Error : %x\n", iRet );
	}
	else
	{
		if( Data[0] == CONEXIO_CMM920_SET_READING_READ ){
			*phr = ( Data[1] << 8 ) | Data[2];
			*fc_upper = Data[3];
		}
	}

	return iRet;

}

// Antenna mode (Internal or External )
int conexio_cmm920_antenna( BYTE isWrite, BYTE *antennaMode )
{
	int iRet;
	BYTE Data[2];
	int Size = 2;
	int i;

	DbgPrint("conexio_cmm920_antenna antenna mode : %x\n", *antennaMode );

	if( _conexio_cmm920_check_read_or_write( isWrite ) ){
		DbgPrint("<conexio_cmm920_antenna>:Parameter Error\n");
		return -1;
	}

	Data[0] = isWrite;

	if( isWrite == CONEXIO_CMM920_SET_READING_WRITE ){
		switch( *antennaMode ){

		case CONEXIO_CMM920_SET_ANTENNA_01:
			DbgPrint("<EXTERNAL>\n");
			break;
		case CONEXIO_CMM920_SET_ANTENNA_00:
			DbgPrint("<INTERNAL>\n");
			break;
		default:
			DbgPrint("<conexio_cmm920_antenna>:Parameter Error\n");
			return -1;
		}
		Data[1] = *antennaMode;
	}

	iRet = _conexio_cmm920_send_recv(
		Data,
		Size,
		CONEXIO_CMM920_MODE_COMMON,
		CONEXIO_CMM920_SET_ANTENNA
	);

	if( iRet < 0 ){
		DbgPrint("<conexio_cmm920_antenna>:Ack Error\n");
	}
	else if( iRet > 0 )
	{
		DbgPrint("<conexio_cmm920_antenna>:Result Error : %x\n", iRet );
	}
	else
	{
		if( Data[0] == CONEXIO_CMM920_SET_READING_READ ){
			*antennaMode = Data[1];
		}
	}

	return iRet;
}

// Version Read
int conexio_cmm920_version(int *ver, int *rev)
{
	int iRet;
	BYTE Data[3];
	int Size = 3;
	int i;

	DbgPrint("conexio_cmm920_version ver : %d rev: %d \n", *ver, *rev );


	Data[0] = CONEXIO_CMM920_SET_READING_READ;

	iRet = _conexio_cmm920_send_recv(
		Data,
		Size,
		CONEXIO_CMM920_MODE_COMMON,
		CONEXIO_CMM920_SET_VERSION
	);

	if( iRet < 0 ){
		DbgPrint("<conexio_cmm920_version >:Ack Error\n");
	}
	else if( iRet > 0 )
	{
		DbgPrint("<conexio_cmm920_antenna>:Result Error : %x\n", iRet );
	}
	else
	{
		if( ver != NULL )
			*ver = _calc_Hex2Bcd(Data[1]);
		if( rev != NULL )
			*rev = _calc_Hex2Bcd(Data[2]);
	}

	return iRet;
}

// LSI set function ( basic )
int conexio_cmm920_lsi(unsigned long lsi_addr, int isWrite, unsigned short *value)
{
	int iRet;
	BYTE Data[6];
	int Size = 6;
	int i;

	DbgPrint("conexio_cmm920_lsi addr : %lx value : %x \n", lsi_addr, *value );

	if( _conexio_cmm920_check_read_or_write( isWrite ) ){
		DbgPrint("<conexio_cmm920_lsi>:Parameter Error\n");
		return -1;
	}

	Data[0] = (lsi_addr & 0x00FF0000 ) >> 16;
	Data[1] = (lsi_addr & 0x0000FF00 ) >> 8;
	Data[2] = (lsi_addr & 0x000000FF );
	Data[3] = isWrite;

	if( isWrite == CONEXIO_CMM920_SET_READING_WRITE ){
		Data[4] = (*value & 0xFF00 ) >> 8;
		Data[5] = (*value & 0xFF );
	}

	iRet = _conexio_cmm920_send_recv(
		Data,
		Size,
		CONEXIO_CMM920_MODE_COMMON,
		CONEXIO_CMM920_SET_LSI
	);

	if( iRet < 0 ){
		DbgPrint("<conexio_cmm920_lsi>:Ack Error\n");
	}
	else if( iRet > 0 )
	{
		DbgPrint("<conexio_cmm920_lsi>:Result Error : %x\n", iRet );
	}
	else
	{
		if( Data[3] == CONEXIO_CMM920_SET_READING_READ ){
			*value = (Data[4] << 8) | Data[5];
		}
	}

	return iRet;

}

// Lsi function ( extensions )

// Data Preemble Bit Length
int conexio_cmm920_lsi_data_preamble_bit_len(int isWrite, BYTE length)
{
	unsigned long lsi_addr;
	unsigned short value;
	int iRet;

	if( length < 8 ){
		DbgPrint("<conexio_cmm920_lsi_data_preamble_bit_len>:Parameter Error : %x\n", iRet );
		return -1; 
	}
	lsi_addr = CONEXIO_CMM920_LSIADDRESS_PRELEN;
	value = (unsigned short) length;

	iRet = conexio_cmm920_lsi( lsi_addr, isWrite, &value );

	if( !iRet ){
		if( isWrite == CONEXIO_CMM920_SET_READING_READ ){
			length = (BYTE)value;
		}
	}else{
		DbgPrint("<conexio_cmm920_lsi_data_preamble_bit_len>:Error : %x\n", iRet );
	}
	
	return iRet;
}

// Data Whitening < Enable / Disable >
int conexio_cmm920_lsi_data_whitening( int isWrite, BYTE isEnable )
{
	unsigned long lsi_addr;
	unsigned short value;
	int iRet;

	if( isEnable > 1 ){
		DbgPrint("<conexio_cmm920_lsi_data_whitening>:Parameter Error : %x\n", iRet );
		return -1; 
	}

	lsi_addr = CONEXIO_CMM920_LSIADDRESS_WHITENING;
	value = (unsigned short) isEnable;

	iRet = conexio_cmm920_lsi( lsi_addr, isWrite, &value );

	if( !iRet ){
		if( isWrite == CONEXIO_CMM920_SET_READING_READ ){
			isEnable = (BYTE)value;
		}
	}else{
		DbgPrint("<conexio_cmm920_lsi_data_whitening>:Error : %x\n", iRet );
	}
	
	return iRet;
}

// Diversity <Enable / Disable>
int conexio_cmm920_lsi_diversity_enable( int isWrite, BYTE isEnable )
{
	unsigned long lsi_addr;
	unsigned short value;
	int iRet;

	if( isEnable > 1 ){
		DbgPrint("<conexio_cmm920_lsi_diversity_enable>:Parameter Error : %x\n", iRet );
		return -1; 
	}

	lsi_addr = CONEXIO_CMM920_LSIADDRESS_DIVER_ENABLE;
	value = (unsigned short) isEnable;

	iRet = conexio_cmm920_lsi( lsi_addr, isWrite, &value );

	if( !iRet ){
		if( isWrite == CONEXIO_CMM920_SET_READING_READ ){
			isEnable = (BYTE)value;
		}
	}else{
		DbgPrint("<conexio_cmm920_lsi_diversity_enable>:Error : %x\n", iRet );
	}
	
	return iRet;
}


// MHR <Enable / Disable>
int conexio_cmm920_lsi_mhr_mode( int isWrite, BYTE isEnable )
{
	unsigned long lsi_addr;
	unsigned short value;
	int iRet;

	if( isEnable > 1 ){
		DbgPrint("<conexio_cmm920_lsi_mhr_mode>:Parameter Error : %x\n", iRet );
		return -1;
	}

	lsi_addr = CONEXIO_CMM920_LSIADDRESS_MHR_MODE;
	value = (unsigned short) isEnable;

	iRet = conexio_cmm920_lsi( lsi_addr, isWrite, &value );

	if( !iRet ){
		if( isWrite == CONEXIO_CMM920_SET_READING_READ ){
			isEnable = (BYTE)value;
		}
	}else{
		DbgPrint("<conexio_cmm920_lsi_mhr_mode>:Error : %x\n", iRet );
	}

	return iRet;
}

// CRC CALC INVERSE <Enable / Disable>
int conexio_cmm920_lsi_crc_calc_inverse( int isWrite, BYTE isEnable )
{
	unsigned long lsi_addr;
	unsigned short value;
	int iRet;

	if( isEnable > 1 ){
		DbgPrint("<conexio_cmm920_lsi_crc_calc_inverse>:Parameter Error : %x\n", iRet );
		return -1;
	}

	lsi_addr = CONEXIO_CMM920_LSIADDRESS_CRC_CALC_INVERSE;
	value = (unsigned short) isEnable;

	iRet = conexio_cmm920_lsi( lsi_addr, isWrite, &value );

	if( !iRet ){
		if( isWrite == CONEXIO_CMM920_SET_READING_READ ){
			isEnable = (BYTE)value;
		}
	}else{
		DbgPrint("<conexio_cmm920_lsi_crc_calc_inverse>:Error : %x\n", iRet );
	}

	return iRet;
}

int conexio_cmm920_lsi_s_panid_filter( int isWrite, BYTE isEnable )
{
	unsigned long lsi_addr;
	unsigned short value;
	int iRet;

	if( isEnable > 1 ){
		DbgPrint("<conexio_cmm920_lsi_s_panid_filter>:Parameter Error : %x\n", iRet );
		return -1;
	}

	lsi_addr = CONEXIO_CMM920_LSIADDRESS_FILTER_S_PANID;
	value = (unsigned short) isEnable;

	iRet = conexio_cmm920_lsi( lsi_addr, isWrite, &value );

	if( !iRet ){
		if( isWrite == CONEXIO_CMM920_SET_READING_READ ){
			isEnable = (BYTE)value;
		}
	}else{
		DbgPrint("<conexio_cmm920_lsi_s_panid_filter>:Error : %x\n", iRet );
	}

	return iRet;
}
int conexio_cmm920_lsi_d_panid_filter( int isWrite, BYTE isEnable )
{
	unsigned long lsi_addr;
	unsigned short value;
	int iRet;

	if( isEnable > 1 ){
		DbgPrint("<conexio_cmm920_lsi_d_panid_filter>:Parameter Error : %x\n", iRet );
		return -1;
	}

	lsi_addr = CONEXIO_CMM920_LSIADDRESS_FILTER_D_PANID;
	value = (unsigned short) isEnable;

	iRet = conexio_cmm920_lsi( lsi_addr, isWrite, &value );

	if( !iRet ){
		if( isWrite == CONEXIO_CMM920_SET_READING_READ ){
			isEnable = (BYTE)value;
		}
	}else{
		DbgPrint("<conexio_cmm920_lsi_d_panid_filter>:Error : %x\n", iRet );
	}

	return iRet;
}
int conexio_cmm920_lsi_d_address_filter( int isWrite, BYTE isEnable )
{
	unsigned long lsi_addr;
	unsigned short value;
	int iRet;

	if( isEnable > 1 ){
		DbgPrint("<conexio_cmm920_lsi_d_address_filter>:Parameter Error : %x\n", iRet );
		return -1;
	}

	lsi_addr = CONEXIO_CMM920_LSIADDRESS_FILTER_D_ADDR;
	value = (unsigned short) isEnable;

	iRet = conexio_cmm920_lsi( lsi_addr, isWrite, &value );

	if( !iRet ){
		if( isWrite == CONEXIO_CMM920_SET_READING_READ ){
			isEnable = (BYTE)value;
		}
	}else{
		DbgPrint("<conexio_cmm920_lsi_d_address_filter>:Error : %x\n", iRet );
	}

	return iRet;
}
int conexio_cmm920_lsi_data_sfd( int isWrite, unsigned short address, BYTE sfd_no )
{
	unsigned long lsi_addr;
	unsigned short value;
	int iRet;

	lsi_addr = CONEXIO_CMM920_LSIADDRESS_SFD( sfd_no );
	value = address;

	iRet = conexio_cmm920_lsi( lsi_addr, isWrite, &value );

	if( !iRet ){
		if( isWrite == CONEXIO_CMM920_SET_READING_READ ){
			address = value;
		}
	}else{
		DbgPrint("<conexio_cmm920_lsi_data_sfd>:Error : %x\n", iRet );
	}
	
	return iRet;
}

// End Lsi Functions < Extension >

//int conexio_cmm920_data_send_single(BYTE buf[], int size, int hop, int send_mode, BYTE r_buf[] ) //2016.01.11 (2)
int conexio_cmm920_data_send_single(BYTE buf[], int size, int send_mode, BYTE r_buf[] )
{
	int iRet;
	BYTE antenna_mode = 0;

	if( antenna_mode == CONEXIO_CMM920_SET_ANTENNA_01 ){
		printf("CATION : SETTING ANTENNA MODE 1 CANNOT SEND DATA.< TELEC VIOLATION >");
		return 1;
	}

	iRet = SendTelegram(buf, size, CONEXIO_CMM920_HOP_SINGLE, send_mode , NULL, NULL, NULL, NULL );

	if( r_buf != NULL )
	{
		iRet = RecvTelegram(r_buf, &size, CONEXIO_CMM920_HOP_SINGLE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	}

	return iRet;
}

int conexio_cmm920_data_send_multi(BYTE buf[], int size, int send_mode, unsigned short dest_id, unsigned short src_id, long dest_addr, long src_addr, BYTE r_buf[])
{
	int iRet;

	iRet = SendTelegram(buf, size, CONEXIO_CMM920_HOP_MULTI, send_mode , &dest_id, &src_id, &dest_addr, &src_addr );

	if( r_buf != NULL )
	{
		iRet = RecvTelegram(r_buf, &size, CONEXIO_CMM920_HOP_MULTI, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	}

	return iRet;
}

int conexio_cmm920_data_recv(BYTE buf[], int *size, int hop, int *r_channel, int *rx_pwr, unsigned int *crc_val ){

	return RecvTelegram(buf, size, hop , r_channel, rx_pwr, crc_val, NULL, NULL, NULL, NULL );
}


int conexio_cmm920_data_recv_single(BYTE buf[], int *size, int *r_channel, int *rx_pwr, unsigned int *crc_val ){

	

	return RecvTelegram(buf, size, CONEXIO_CMM920_HOP_SINGLE ,r_channel, rx_pwr, crc_val, NULL, NULL, NULL, NULL );
}


int conexio_cmm920_data_recv_multi(BYTE buf[], int *size, int *r_channel, int *rx_pwr, unsigned int *crc_val, unsigned short *dest_id, unsigned short *src_id, long *dest_addr, long *src_addr ){
	return RecvTelegram(buf, size, CONEXIO_CMM920_HOP_MULTI , r_channel, rx_pwr, crc_val, dest_id, src_id, dest_addr, src_addr );
}

// 2016.01.11 (2) 
/// 920MHz Send Run-mode Packet function

int calcMHR(int dest_mode, int src_mode, int version, int panidcomp , unsigned short *fc)
{
	int size = 3; // 2016.01.15 (2) change

	*fc = CONEXIO_CMM920_MHR_FC(src_mode, version, dest_mode, 0, panidcomp, 0, 0, 0, 1);

	if( version < 0x02 ){

		switch( dest_mode ){
		case CONEXIO_CMM920_MHR_FC_DESTADDRMODE_8BIT:
			size += 3; // pan id (2 byte ) + addr (1 byte)
			break;
		case CONEXIO_CMM920_MHR_FC_DESTADDRMODE_16BIT:
			size += 4; // pan id (2 byte ) + addr (2 byte)
			break;
		}

		switch( src_mode ){
		case CONEXIO_CMM920_MHR_FC_SRCADDRMODE_8BIT:
			size += 3; // pan id (2 byte ) + addr (1 byte)
			break;
		case CONEXIO_CMM920_MHR_FC_SRCADDRMODE_16BIT:
			size += 4; // pan id (2 byte ) + addr (2 byte)
			break;
		}
	}

	return size;
}

int addMHR(BYTE pktBuf[] , unsigned short fc, unsigned short *dest_id, unsigned short *src_id, long *dest_addr, long *src_addr)
{
	unsigned int offset = 0;
	unsigned int val_base_addr = 6; // 2016.01.15 (2)

	pktBuf[3] = ( fc & 0xFF );
	pktBuf[4] = ( ( fc & 0xFF00 ) >> 8 );

	//seq number追加(仮)
	pktBuf[5] = global_seq_num++; // 2016.01.15 (2)

	if( dest_id != NULL ){
		pktBuf[val_base_addr + offset] = ( *dest_id & 0xFF );
		pktBuf[val_base_addr + 1 + offset] = ( ( *dest_id & 0xFF00 ) >> 8 );
		offset += 2;
	}
	if( dest_addr != NULL ){
		switch( CONEXIO_CMM920_MHR_FC_DESTADDRMODE( fc ) ){
		case CONEXIO_CMM920_MHR_FC_DESTADDRMODE_NONE:
			break;
		case CONEXIO_CMM920_MHR_FC_DESTADDRMODE_8BIT:
			pktBuf[val_base_addr + offset] = ( *dest_addr & 0xFF );
			offset += 1;
			break;
		case CONEXIO_CMM920_MHR_FC_DESTADDRMODE_16BIT:
			pktBuf[val_base_addr + offset] = ( *dest_addr & 0xFF );
			pktBuf[val_base_addr + 1 + offset] = ( ( *dest_addr & 0xFF00 ) >> 8 );
			offset += 2;
			break;
		}
	}
	if( src_id != NULL ){
		pktBuf[val_base_addr + offset] = ( *src_id & 0xFF );
		pktBuf[val_base_addr + 1 + offset] = ( ( *src_id & 0xFF00 ) >> 8 );
		offset += 2;
	}

	if( src_addr != NULL ){
		switch( CONEXIO_CMM920_MHR_FC_SRCADDRMODE( fc ) ){
		case CONEXIO_CMM920_MHR_FC_SRCADDRMODE_NONE:
			break;
		case CONEXIO_CMM920_MHR_FC_SRCADDRMODE_8BIT:
			pktBuf[val_base_addr + offset] = ( *src_addr & 0xFF );
			offset += 1;
			break;
		case CONEXIO_CMM920_MHR_FC_SRCADDRMODE_16BIT:
			pktBuf[val_base_addr + offset] = ( *src_addr & 0xFF );
			pktBuf[val_base_addr + 1 + offset] = ( ( *src_addr & 0xFF00 ) >> 8 );
			offset += 2;
			break;
		}
	}

	return offset;
}

int parseMHR(BYTE dataBuf[] , unsigned short *pFc, BYTE *pSeq_no, unsigned short *pDest_id, unsigned short *pSrc_id, long *pDest_addr, long *pSrc_addr)
{
	unsigned int offset = 3;

	unsigned short fc = 0;
	BYTE seq_no = 0;
	unsigned short dest_id;
	unsigned short src_id;
	long dest_addr;
	long src_addr;


	fc = ( dataBuf[1] << 8 ) | dataBuf[0];
	seq_no = dataBuf[2];
	
	{
		dest_id = ( dataBuf[ offset + 1 ] << 8 ) + dataBuf[ offset ];
		offset += 2;
	}

	{
		switch( CONEXIO_CMM920_MHR_FC_DESTADDRMODE( fc ) ){
		case CONEXIO_CMM920_MHR_FC_DESTADDRMODE_NONE:
			break;
		case CONEXIO_CMM920_MHR_FC_DESTADDRMODE_8BIT:
			dest_addr = dataBuf[ offset ];
			offset += 1;
			break;
		case CONEXIO_CMM920_MHR_FC_DESTADDRMODE_16BIT:
			dest_addr = ( dataBuf[ offset + 1 ] << 8 ) + dataBuf[ offset ];
			offset += 2;
			break;
		}
	}

	{
		src_id = ( dataBuf[ offset + 1 ] << 8 ) + dataBuf[ offset ];
		offset += 2;
	}

	{
		switch( CONEXIO_CMM920_MHR_FC_SRCADDRMODE( fc ) ){
		case CONEXIO_CMM920_MHR_FC_SRCADDRMODE_NONE:
			break;
		case CONEXIO_CMM920_MHR_FC_SRCADDRMODE_8BIT:
			src_addr = dataBuf[ offset ];
			offset += 1;
			break;
		case CONEXIO_CMM920_MHR_FC_SRCADDRMODE_16BIT:
			src_addr = ( dataBuf[ offset + 1 ] << 8 ) + dataBuf[ offset ];
			offset += 2;
			break;
		}
	}

	if( pFc != NULL ) *pFc = fc;
	if( pSeq_no != NULL )	*pSeq_no = seq_no;
	if( pDest_addr != NULL ) *pDest_addr = dest_addr;
	if( pDest_id != NULL )	*pDest_id = dest_id;
	if( pSrc_addr != NULL ) *pSrc_addr = src_addr;
	if( pSrc_id != NULL ) *pSrc_id = src_id;

	DbgPrint("<parseMHR> fc:%x seq_no:%d dest_id : %x src_id : %x dest_addr :%lx src_addr : %lx \n ",
		fc, seq_no, dest_id, src_id, dest_addr, src_addr);

	return offset;

}

int SendTelegram(BYTE buf[], int size, int hop, int send_mode, unsigned short *dest_id, unsigned short *src_id, long *dest_addr, long *src_addr)
{

	BYTE*	pktBuf;
	int pktSize;
	int i, ret;
	int offset = 0;
	unsigned short fc = 0;


	if( hop == CONEXIO_CMM920_HOP_MULTI ){
		offset = calcMHR(
			CONEXIO_CMM920_MHR_FC_DESTADDRMODE_16BIT,
			CONEXIO_CMM920_MHR_FC_SRCADDRMODE_16BIT,
			0x00,
			0x00,
			&fc
		);
	}

	pktBuf = (BYTE*)malloc( (size + offset + 3) * sizeof(BYTE) );
	pktSize = size + offset + 3;

	switch( send_mode ){
	case CONEXIO_CMM920_SENDDATA_MODE_NOACK_NORESP:
	case CONEXIO_CMM920_SENDDATA_MODE_ACK_NORESP:
	case CONEXIO_CMM920_SENDDATA_MODE_NOACK_RESP:
	case CONEXIO_CMM920_SENDDATA_MODE_ACK_RESP:
		break;
	default:
		return 0;
	}


	pktBuf[0] = send_mode;
	pktBuf[1] = ( size + offset + 4 ) >> 8;
	pktBuf[2] = ( size + offset + 4 ) % 256;

	if( hop == CONEXIO_CMM920_HOP_MULTI ){
		addMHR(pktBuf, fc, dest_id, src_id, dest_addr, src_addr);
	}

	for( i = 0; i < size ; i ++ ){
		pktBuf[3 + offset + i] = buf[i];
	}

	ret = (int)SendCommand(pktBuf, pktSize, CONEXIO_CMM920_MODE_RUN, CONEXIO_CMM920_SENDDATA);

	return ret;

}


int RecvTelegram(BYTE buf[], int *size , int hop, int *r_channel, int *rx_pwr, unsigned int *crc ,unsigned short *dest_id, unsigned short *src_id, long *dest_addr, long *src_addr)
{
	int iRet = 0;	
	int offset = 0;
	int i;
	int iSize = 0;


	if( hop == CONEXIO_CMM920_HOP_SINGLE )	return RecvTelegramSingleHop(buf, size, r_channel, rx_pwr, crc);

	iRet = RecvTelegramSingleHop(buf, &iSize, r_channel, rx_pwr, crc);

	if( iRet == 0 ){
		if( iSize > 0 ){
			offset = parseMHR(	 buf, NULL, NULL, dest_id, src_id, dest_addr, src_addr );

			for( i = offset; i < iSize; i ++ ){
				buf[i - offset] = buf[i];
				DbgPrint("%x", buf[i - offset]);
			}
			DbgPrint(":Length %d\n", iSize - offset );
			memset( &buf[iSize - offset], 0x00, offset );
			if( size != NULL )	*size = iSize - offset;
		}else{
			if( size != NULL )	*size = 0;
		}
	}


	return iRet;

}

int RecvTelegramSingleHop(BYTE buf[], int *size , int *r_channel, int *rx_pwr , unsigned int *crc )
{

	BYTE*	pktBuf;
	int pktSize;
	int i, ret, d_size, crc_size;
	int iRet;
	int head_size = 5; //recv enable/disable + rx_channel + rx_power + phr(2byte)
	int foot_size;

	crc_size = 32;// crc ( bit length )
	foot_size = 2 + (crc_size / 8); // crc + rssi + ant
	if( size != NULL && *size != 0 ){
		pktSize = *size + head_size + foot_size;
		pktBuf = (BYTE*)malloc( pktSize * sizeof(BYTE) );
		memset(pktBuf, 0, ( pktSize * sizeof(BYTE) ));	// 2016.01.08 (2)
	}else{
		pktSize = 0;
		pktBuf = (BYTE*)malloc( ( 512 + head_size + foot_size ) * sizeof(BYTE) );
		memset(pktBuf, 0, ( 512 + head_size + foot_size ) * sizeof(BYTE) ); // 2016.01.08 (2)	
	}
	// Data Received
	iRet = RecvCommandAck(pktBuf, &pktSize, CONEXIO_CMM920_MODE_RUN, CONEXIO_CMM920_SENDDATA);

	if( iRet ){
		free(pktBuf); // 2016.01.08 (1)
		return iRet;
	}


	if( pktSize > 0 ){
		if( pktBuf[0] == 0x01 ){
			
			if( pktSize >= 3 )	DbgRecvTelegramParam("pktBuf[2] = 0x%x\n", pktBuf[2]);

			if( r_channel != NULL )
				*r_channel = _calc_Hex2Bcd(pktBuf[1]);
			if( rx_pwr != NULL ){
				*rx_pwr = _conexio_cmm920_Hex2dBm(pktBuf[2]);
				DbgRecvTelegramParam("*rx_pwr = 0x%x\n", *rx_pwr);
			}
			d_size = pktBuf[3] * 256 + pktBuf[4];

			if( buf != NULL ){
				if( d_size < pktSize ){ 
					for( i = 0; i < d_size - (crc_size / 8) ; i ++ ){
						buf[i] = pktBuf[head_size + i];
					}
				}
			}
			//crc, rx_rssi, rx_ant
		
			if( size != NULL ) *size = d_size; // 2016.01.08 (3)
			// 2016.01.11 (3) start
			if( crc != NULL ){
				for( i = 0, *crc = 0; i < (crc_size / 8); i ++)
					*crc |= (*crc << 8) | pktBuf[head_size + d_size - (crc_size / 8) + i];
			}
			// 2016.01.11 (3) end
		}
	}
	
	free(pktBuf);// 2016.01.08 (1)
	return 0;
}

// Send Command CMM920
int SendCommand(BYTE buf[], int size, BYTE mode, BYTE command )
{
	BYTE *array;
	int length = 0;
	int i;
	PCONEXIO920PACKET pac;

	pac = allocConexioCMM920_packet(pac, mode, command, 1);

	if(pac == NULL) return -1;
	pac->data = (BYTE*)malloc(sizeof(BYTE) * size);
	if(pac->data == NULL)
	{
		printf("Memory allocation error\n");
		free(pac);
		return -2;
	}else{
		DbgAllocFreeCheck("<SendCommand> Alloc PAC data\n");

		for (i = 0; i < size; i++)
		{
			pac->data[i] = buf[i];
		}
	}

	array = pktGetBYTEArray( pac, size , &length);
	//length = pktGetLength(size);

	DbgPrint("Port %x, size :%d length :%d \n ",iPort, size, length);
	tcflush( iPort, TCIFLUSH );
	Serial_PutString(iPort, array, ( length * sizeof(BYTE) ) );

	DbgPrint("Send Data = ");

	for (i = 0; i < length; i++)
	{
		DbgPrint("%02X", array[i]);
	}

	DbgPrint("\n");

	// malloc縺ｧ遒ｺ菫昴＠縺溘Γ繝｢繝ｪ縺ｯ蠢�縺夊ｧ｣謾ｾ縺吶ｋ
	free(array);
	return 0;
}

int pktChkBYTEArray(PCONEXIO920PACKET pac, BYTE *array, int size )
{
	long cnt;
	int HeadSize, FootSize, pktSizeOffset;
	int iRet = 0;

	HeadSize = 8;	//header size 8byte
	FootSize = 3; //footer size 3byte
	pktSizeOffset = 5;

	if(array == NULL)
	{
		DbgPrint("<pktChkBYTEArray> Memory allocation error\n");
		return 1;
	}

	/*  Error Code */
	global_getLastError = (array[5] << 8) + array[6];

	/* header Check */
	if( array[0] != pac->dle || 
		array[1] != pac->stx ){
		DbgPrint("<pktChkBYTEArray> Header error\n");
		iRet |= 2;
	}
	else	if( array[2] != (BYTE)((size + pktSizeOffset) >> 8) ||
				array[3] != (BYTE)((size + pktSizeOffset) % 256) ){
			DbgPrint("<pktChkBYTEArray> size check error\n");
			iRet |= 4;
	}
	/* footer Check */
	else if( array[size + HeadSize + 1] != pac->dle ||
		array[size + HeadSize + 2] != pac->etx
	){
		DbgPrint("<pktChkBYTEArray> Footer Error (%x), (%x) \n", array[size + HeadSize + 1], array[size + HeadSize + 2]);
		iRet |= 8;
	}else{
		if( array[4] != ( pac->command[0] | CONEXIO_CMM920_RECVCOMMAND ) ||
				array[5] != pac->command[1]
		){
			DbgPrint("<pktChkBYTEArray> Myself command ack Error\n");
			iRet |= 16;
			if( array[4] == CONEXIO_CMM920_RECVCOMMAND && 
				array[5] == 0xFF )
			{
				DbgPrint("<pktChkBYTEArray> unusual Command \n");
			} 
		}

		/* check sum Check */
		for(pac->sum = 0, cnt = 2; cnt < size + HeadSize; cnt++)
		{
			pac->sum += array[cnt];
		}
		if( array[size + HeadSize] != (BYTE)((pac->sum ^ 0xFF) + 1) ){
			DbgPrint("<pktChkBYTEArray> Check Sum Error\n");
			iRet |= 32;
		}

		for (cnt = 0; cnt < size; cnt ++)
		{
			pac->data[cnt] = array[cnt + HeadSize];
		}
	}

	return iRet;
}

int RecvCommandAck( BYTE *buf, int *size , BYTE mode, BYTE command )
{

	BYTE *array;
	BYTE head_data[4] = {0};
	int length = 0;
	int i;
	int iRet = 0;
	int d_size = 0;
	int readlen = 0;
	PCONEXIO920PACKET pac;

	pac = allocConexioCMM920_packet(pac, 0, 0, 0);
	if(pac == NULL){
		DbgPrint("<RecvCommandAck> Memory Null Allocate.\n");
		return -1;
	}
	
	memset(head_data, 0, sizeof( BYTE ) * 4 ); // 2016.01.08 (2)

	//size get
	Serial_GetString(iPort, &head_data[0], 4 * sizeof(BYTE));

	d_size = (head_data[2] * 256 + head_data[3]);
	if( d_size < 5 || d_size > 512 ){
		if (d_size < 5 )
			DbgDataLength("<RecvCommandAck> Non Data Length \n");
		else if( d_size > 512 )
			DbgDataLength("<RecvCommandAck> Over Data Length \n");

		freeConexioCMM920_packet(pac);// 2016.01.08 (1)
//		free(pac);
		return -2;
	}
	d_size -= 5; // ヘッダサイズを引いて実データサイズを求める
	pac->data = (BYTE*)malloc(sizeof(BYTE) * (d_size) );
	if(pac->data == NULL)
	{
		DbgPrint("<RecvCommandAck> Memory allocation error\n");
		freeConexioCMM920_packet(pac);// 2016.01.08 (1)
//		free(pac);
		return -3;
	}
	DbgAllocFreeCheck("<RecvCommandAck> Alloc PAC data\n");

	array = pktGetBYTEArray(pac, d_size, &length);
	//length = pktGetLength(size);

	for( i = 0; i < 4 ; i ++)
		array[i] = head_data[i];

	// Get Data (without header)
	do {
		readlen += Serial_GetString(iPort, &array[4+readlen], (length - 4) * sizeof(BYTE));
		DbgPrint("Recvlen = %d\n", readlen);
	} while (readlen < (length - 4));

	DbgPrint("Recv Data = ");

	for (i = 0; i < length; i++)
	{
		DbgPrint("%02X", array[i]);
	}

	DbgPrint("\n");

	pac = allocConexioCMM920_packet(pac, mode, command, 1);
	if(pac == NULL) {
		DbgPrint("<RecvCommandAck> Memory Null Allocate.\n");
		free(array);
		return -4;
	}
	pac->data = (BYTE*)malloc(sizeof(BYTE) * (d_size) );
	DbgAllocFreeCheck("<RecvCommandAck> Alloc PAC data( check ) \n");
	if(pac->data == NULL)
	{
		DbgPrint("<RecvCommandAck> Memory allocation error\n");
		free(array);
		free(pac);
		return -5;
	}

	if(size != NULL){
		DbgPrint("<RecvCommandAck> size not NULL\n");
		if(*size != 0){
			DbgPrint("<RecvCommandAck> *size not 0 : %d\n", *size );
		}else{
			DbgPrint("<RecvCommandAck> size = 0 d_size %d\n", d_size );
		}
	}else{
		DbgPrint("<RecvCommandAck> size is NULL\n");
	}

	if( (size != NULL) && (*size != 0) ){
		iRet = pktChkBYTEArray( pac, array , *size);
	}else{
		iRet = pktChkBYTEArray( pac, array , d_size);
	}
	if( iRet ){
		DbgPrint("<RecvCommandAck> pkt Chk Error : %x\n", iRet );
		// 2016.01.08 (2) start
		free(array);
		freeConexioCMM920_packet(pac);
		//free(pac->data);
		//free(pac);
		// 2016.01.08 (2) end
		return -6;
		
	}
	free(array);

	for( i = 0; i < d_size; i ++ ){
		buf[i] = pac->data[i];
	}
	// 2016.01.08 (2) start
	freeConexioCMM920_packet(pac);
	//free(pac->data);
	//free(pac);
	// 2016.01.08 (2) end

	if(size != NULL)	*size = d_size;

	return 0;	

}

// Allocate 920MHz Packet
PCONEXIO920PACKET allocConexioCMM920_packet(PCONEXIO920PACKET pac, BYTE mode, BYTE com, BYTE isSend )
{
	pac = (PCONEXIO920PACKET)malloc(sizeof(CONEXIO920PACKET));

	if(pac == (PCONEXIO920PACKET)NULL) return pac;

	DbgAllocFreeCheck("<allocConexioCMM920_packet> Alloc PAC\n");
	// if you send the packet, it set header and footer parameters. 
	if( isSend ){
		pac->dle = 0x10;
		pac->stx = 0x02;
		pac->size[0] = 0x00;
		pac->size[1] = 0x00;
		pac->command[0] = mode;
		pac->command[1] = com;
		pac->result = 0x00;
		pac->resultCode = 0x00;
		pac->etx = 0x03;
	}
	pac->data = (BYTE*)NULL; // 2016.01.15 (5)
	return pac;
}

// Free memory 920MHz packet
void freeConexioCMM920_packet(PCONEXIO920PACKET pac)
{
	if(pac->data != (BYTE *)NULL){
		DbgAllocFreeCheck("<freeConexioCMM920_packet> Free PAC Data\n");
		free(pac->data); 
		pac->data = (BYTE *)NULL;//2016.01.15 (5)
	}
	if(pac != (PCONEXIO920PACKET)NULL ){
		DbgAllocFreeCheck("<freeConexioCMM920_packet> Free PAC \n");
		free(pac);
		pac = (PCONEXIO920PACKET)NULL;//2016.01.15 (5)
	}
}

// Make 920MHz packet
BYTE* pktGetBYTEArray( PCONEXIO920PACKET pac, int size , int *retSize)
{
	BYTE* retArray;
	long cnt;
	int HeadSize, FootSize, pktSizeOffset;

	HeadSize = 8;	//header size 8byte
	pktSizeOffset	= 5;
	FootSize = 3; //footer 3byte(locked)

	// malloc packet size 
	retArray = (BYTE *)malloc(sizeof(BYTE) * (HeadSize + FootSize + size ));
	if(retArray == (BYTE*)NULL)
	{
		DbgPrint("<pktGetBYTEArray> Memory allocation error\n");
		return (BYTE*)NULL;
	}

	*retSize = HeadSize + FootSize + size;

	/* header */
	retArray[0] = pac->dle;
	retArray[1] = pac->stx;
	retArray[2] = (BYTE)((size + pktSizeOffset) >> 8);
	retArray[3] = (BYTE)((size + pktSizeOffset) % 256);
	retArray[4] = pac->command[0];
	retArray[5] = pac->command[1];
	retArray[6] = pac->result;
	retArray[7] = pac->resultCode;

	/* Data */
	for(cnt = 0; cnt < size; cnt++)
	{
		retArray[HeadSize + cnt] = pac->data[cnt];
	}

	/* check sum */
	for(pac->sum = 0, cnt = 2; cnt < size + HeadSize; cnt++)
	{
		pac->sum += retArray[cnt];
	}
	retArray[size + HeadSize] = (BYTE)((pac->sum ^ 0xFF) + 1);

	/* footer */
	retArray[size + HeadSize + 1] = pac->dle;
	retArray[size + HeadSize + 2] = pac->etx;


	/* free memory packet */
	freeConexioCMM920_packet(pac);

	return retArray;
}
