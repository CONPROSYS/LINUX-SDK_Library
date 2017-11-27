/***

	libconexio_CMM920_wrap.c - conexio_CMM920_Wrapper functions library
	Copyright (C) 2016 Syunsuke Okamoto.<okamoto@contec.jp>.
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

	update 2016.01.08 (1) Fixed 100kBps values of conexio_cmm920_set_wireless function.
  update 2016.01.15 (1) Fixed conexio_cmm920_lsi_data_whitening function in conexio_cmm920_set_hop_mode sets from enable to disable.
***/

#include <stdio.h>
#include "libconexio_CMM920.h"

// wrapper
int conexio_cmm920_set_mode(int code)
{
	return conexio_cmm920_mode(
			CONEXIO_CMM920_SET_READING_WRITE,
			&code
	);
}

int conexio_cmm920_set_address(unsigned short panId, BYTE Addr[], unsigned short shortAddr )
{
	return conexio_cmm920_address(
			CONEXIO_CMM920_SET_READING_WRITE,
			&panId,
			Addr,
			&shortAddr
	);
}

int conexio_cmm920_set_wireless(int iBitrate, BYTE channel, BYTE power, char sendLv, char recvLv, unsigned short sendTim, BYTE sendNum, BYTE ackRetryNum, unsigned short ackWaitTim){
	BYTE bBitrate;
	BYTE bcdChannel;
	BYTE bcdSendLv;
	BYTE bcdRecvLv;

	switch(iBitrate){
	case 50000:
		bBitrate = CONEXIO_CMM920_SET_WIRELESS_BITRATE_50KBPS;
		break;
	case 100000:
		bBitrate = CONEXIO_CMM920_SET_WIRELESS_BITRATE_100KBPS;
		break;
	default:
		return 1;
	}
	bcdChannel = _calc_Bcd2Hex(channel);

	bcdSendLv = _conexio_cmm920_dBm2Hex((int)sendLv);
	bcdRecvLv = _conexio_cmm920_dBm2Hex((int)recvLv);

	return conexio_cmm920_wireless(
		CONEXIO_CMM920_SET_READING_WRITE,
		&bBitrate,
		&bcdChannel,
		&power,
		&bcdSendLv,
		&bcdRecvLv,
		&sendTim,
		&sendNum,
		&ackRetryNum,
		&ackWaitTim
	);


}
int conexio_cmm920_set_timer( unsigned short tim ){

	return conexio_cmm920_timer(
		CONEXIO_CMM920_SET_READING_WRITE,
		&tim
		);

}
int conexio_cmm920_set_auto_ack_frame( unsigned short phr, unsigned char fc_upper ){
	return conexio_cmm920_auto_ack_frame(
		CONEXIO_CMM920_SET_READING_WRITE,
		&phr,
		&fc_upper
	);
}
int conexio_cmm920_set_antenna( BYTE antennaMode ){
	return conexio_cmm920_antenna(
		CONEXIO_CMM920_SET_READING_WRITE,
		&antennaMode
	);
}

int conexio_cmm920_get_mode(int *code)
{
	return conexio_cmm920_mode(
			CONEXIO_CMM920_SET_READING_READ,
			code
	);
}

int conexio_cmm920_get_address(unsigned short *panId, BYTE Addr[], unsigned short *shortAddr )
{
	return conexio_cmm920_address(
			CONEXIO_CMM920_SET_READING_READ,
			panId,
			Addr,
			shortAddr
	);
}

int conexio_cmm920_get_wireless(int *iBitrate, BYTE *channel, BYTE *power, char *sendLv, char *recvLv, unsigned short *sendTim, BYTE *sendNum, BYTE *ackRetryNum, unsigned short *ackWaitTim){
	BYTE bBitrate;
	BYTE bcdChannel;
	BYTE bcdSendLv;
	BYTE bcdRecvLv;
	int iRet;

	iRet = conexio_cmm920_wireless(
		CONEXIO_CMM920_SET_READING_READ,
		&bBitrate,
		&bcdChannel,
		power,
		&bcdSendLv,
		&bcdRecvLv,
		sendTim,
		sendNum,
		ackRetryNum,
		ackWaitTim
	);

	switch(bBitrate){
	case CONEXIO_CMM920_SET_WIRELESS_BITRATE_50KBPS:
		*iBitrate = 50000;
		break;
	case CONEXIO_CMM920_SET_WIRELESS_BITRATE_100KBPS:
		*iBitrate = 100000;
		break;
	}
	*channel = _calc_Hex2Bcd(bcdChannel);

	*sendLv = _conexio_cmm920_Hex2dBm(bcdSendLv);
	*recvLv = _conexio_cmm920_Hex2dBm(bcdRecvLv);

	return iRet;

}
int conexio_cmm920_get_timer( unsigned short *tim ){

	return conexio_cmm920_timer(
		CONEXIO_CMM920_SET_READING_READ,
		tim
		);

}
int conexio_cmm920_get_auto_ack_frame( unsigned short *phr, unsigned char *fc_upper ){
	return conexio_cmm920_auto_ack_frame(
		CONEXIO_CMM920_SET_READING_READ,
		phr,
		fc_upper
	);
}

int conexio_cmm920_get_antenna( BYTE *antennaMode ){
	return conexio_cmm920_antenna(
		CONEXIO_CMM920_SET_READING_READ,
		antennaMode
	);
}

void conexio_cmm920_set_hop_mode( BYTE hop )
{

	if( hop == CONEXIO_CMM920_HOP_MULTI ){

		conexio_cmm920_lsi_mhr_mode(
			CONEXIO_CMM920_SET_READING_WRITE,
			CONEXIO_CMM920_SET_ENABLE
		);

		conexio_cmm920_lsi_crc_calc_inverse(
				CONEXIO_CMM920_SET_READING_WRITE,
				CONEXIO_CMM920_SET_ENABLE
		);
		conexio_cmm920_lsi_s_panid_filter(
				CONEXIO_CMM920_SET_READING_WRITE,
				CONEXIO_CMM920_SET_ENABLE
		);

		conexio_cmm920_lsi_d_panid_filter(
				CONEXIO_CMM920_SET_READING_WRITE,
				CONEXIO_CMM920_SET_ENABLE
		);

		conexio_cmm920_lsi_d_address_filter(
				CONEXIO_CMM920_SET_READING_WRITE,
				CONEXIO_CMM920_SET_ENABLE
		);

		conexio_cmm920_lsi_data_preamble_bit_len(
				CONEXIO_CMM920_SET_READING_WRITE,
				CONEXIO_CMM920_PRELEN_MULTIHOP
		);

		conexio_cmm920_lsi_data_sfd(
				CONEXIO_CMM920_SET_READING_WRITE,
				CONEXIO_CMM920_SFD_MULTIHOP	,
				CONEXIO_CMM920_SFDNUM_0
		);

		conexio_cmm920_lsi_data_whitening(
				CONEXIO_CMM920_SET_READING_WRITE,
				//CONEXIO_CMM920_SET_ENABLE
				CONEXIO_CMM920_SET_DISABLE	// 2016.01.15 (1) 
		);

	}else{
		conexio_cmm920_reset();
	}

}

int conexio_cmm920_set_wireless_default(void)
{
	return conexio_cmm920_set_wireless(
			100000,	//100kbps
			60,		//60ch
			CONEXIO_CMM920_SET_WIRELESS_POWER_01MW,
			-85,	//-85 dbm
			-95,	// -95 dbm
			130,	// 130(usec)
			6,
			4,		// ACK 4回
			100		// 100(usec)
	);
}

int conexio_cmm920_set_auto_ack_frame_default(void)
{
	return conexio_cmm920_set_auto_ack_frame(
		0x07,
		0x00
	);

}

int conexio_cmm920_set_timer_default(void)
{
	return conexio_cmm920_set_timer(10000);

}

