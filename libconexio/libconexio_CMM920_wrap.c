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

/**
	@~English
	@brief CONEXIO 920MHz Module Set Mode function
	@param code : mode
	@return Success : 0 , Failed : From -1 to -15 : Parameter Error, From -16 to -31 Send Error, less than -32 : Receive Error
	@~Japanese
	@brief CONEXIO 920MHz　Module の モード設定関数
	@param code : mode
	@return 成功:  0 失敗 :  送信 エラー:  -1～-15 -16～-31,　受信エラー : -32～
**/
int conexio_cmm920_set_mode(int code)
{
	return conexio_cmm920_mode(
			CONEXIO_CMM920_SET_READING_WRITE,
			&code
	);
}

/**
	@~English
	@brief CONEXIO 920MHz Module Set Address function
	@param panId : PAN ID
	@param Addr : Long Address
	@param shortAddr : Short address
	@return Success : 0 , Failed : From -1 to -15 : Parameter Error, From -16 to -31 Send Error, less than -32 : Receive Error
	@~Japanese
	@brief CONEXIO 920MHz　Module の アドレス設定関数
	@param panId : PAN ID
	@param Addr : ロングアドレス
	@param shortAddr : ショートアドレス
	@return 成功:  0 失敗 :  送信 エラー:  -1～-15 -16～-31,　受信エラー : -32～
**/
int conexio_cmm920_set_address(unsigned short panId, BYTE Addr[], unsigned short shortAddr )
{
	return conexio_cmm920_address(
			CONEXIO_CMM920_SET_READING_WRITE,
			&panId,
			Addr,
			&shortAddr
	);
}

/**
	@~English
	@brief CONEXIO 920MHz Module Set Wireless function
	@param iBitrate : bit rate
	@param channel : channel
	@param power : send power
	@param sendLv : send sence level
	@param recvLv : receive sence level
	@param sendTim : send sence retry time
	@param sendNum : send sence retry number
	@param ackRetryNum : acknowledge retry number
	@param ackWaitTim : acknowledge retry time
	@return Success : 0 , Failed : From -1 to -15 : Parameter Error, From -16 to -31 Send Error, less than -32 : Receive Error
	@~Japanese
	@brief CONEXIO 920MHz　Module の 無線設定関数
	@param iBitrate : ビットレート
	@param channel : チャネル
	@param power : 送信出力
	@param sendLv : 送信センスレベル
	@param recvLv : 受信センスレベル
	@param sendTim : 送信センス再送時間
	@param sendNum : 送信センス再送回数
	@param ackRetryNum : ACKリトライ回数
	@param ackWaitTim : ACK待ち時間
	@return 成功:  0 失敗 :  送信 エラー:  -1～-15 -16～-31,　受信エラー : -32～
**/
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

/**
	@~English
	@brief CONEXIO 920MHz Module set timer function
	@param tim : time ( msec )
	@return Success : 0 , Failed : From -1 to -15 : Parameter Error, From -16 to -31 Send Error, less than -32 : Receive Error
	@~Japanese
	@brief CONEXIO 920MHz　Module の 時間設定関数
	@param tim : 時間 (msec)
	@return 成功:  0 失敗 :  送信 エラー:  -1～-15 -16～-31,　受信エラー : -32～
**/
int conexio_cmm920_set_timer( unsigned short tim ){

	return conexio_cmm920_timer(
		CONEXIO_CMM920_SET_READING_WRITE,
		&tim
		);

}

/**
	@~English
	@brief CONEXIO 920MHz Module Set Auto Acknowledge Frame function
	@param phr :
	@param fc_upper :
	@return Success : 0 , Failed : From -1 to -15 : Parameter Error, From -16 to -31 Send Error, less than -32 : Receive Error
	@~Japanese
	@brief CONEXIO 920MHz　Module の オートACKフレーム 設定 関数
	@param phr :
	@param fc_upper :
	@return 成功:  0 失敗 :  送信 エラー:  -1～-15 -16～-31,　受信エラー : -32～
**/
int conexio_cmm920_set_auto_ack_frame( unsigned short phr, unsigned char fc_upper ){
	return conexio_cmm920_auto_ack_frame(
		CONEXIO_CMM920_SET_READING_WRITE,
		&phr,
		&fc_upper
	);
}

/**
	@~English
	@brief CONEXIO 920MHz Module Set Antenna mode function
	@param antennaMode :
	@return Success : 0 , Failed : From -1 to -15 : Parameter Error, From -16 to -31 Send Error, less than -32 : Receive Error
	@~Japanese
	@brief CONEXIO 920MHz　Module の アンテナモード 設定関数
	@param antennaMode :
	@return 成功:  0 失敗 :  送信 エラー:  -1～-15 -16～-31,　受信エラー : -32～
**/
int conexio_cmm920_set_antenna( BYTE antennaMode ){
	return conexio_cmm920_antenna(
		CONEXIO_CMM920_SET_READING_WRITE,
		&antennaMode
	);
}

/**
	@~English
	@brief CONEXIO 920MHz Module Get Mode function
	@param code : mode
	@return Success : 0 , Failed : From -1 to -15 : Parameter Error, From -16 to -31 Send Error, less than -32 : Receive Error
	@~Japanese
	@brief CONEXIO 920MHz　Module の モード取得関数
	@param code : mode
	@return 成功:  0 失敗 :  送信 エラー:  -1～-15 -16～-31,　受信エラー : -32～
**/
int conexio_cmm920_get_mode(int *code)
{
	return conexio_cmm920_mode(
			CONEXIO_CMM920_SET_READING_READ,
			code
	);
}

/**
	@~English
	@brief CONEXIO 920MHz Module Get Address function
	@param panId : PAN ID
	@param Addr : Long Address
	@param shortAddr : Short address
	@return Success : 0 , Failed : From -1 to -15 : Parameter Error, From -16 to -31 Send Error, less than -32 : Receive Error
	@~Japanese
	@brief CONEXIO 920MHz　Module の アドレス取得関数
	@param panId : PAN ID
	@param Addr : ロングアドレス
	@param shortAddr : ショートアドレス
	@return 成功:  0 失敗 :  送信 エラー:  -1～-15 -16～-31,　受信エラー : -32～
**/
int conexio_cmm920_get_address(unsigned short *panId, BYTE Addr[], unsigned short *shortAddr )
{
	return conexio_cmm920_address(
			CONEXIO_CMM920_SET_READING_READ,
			panId,
			Addr,
			shortAddr
	);
}

/**
	@~English
	@brief CONEXIO 920MHz Module Get Wireless function
	@param iBitrate : bit rate
	@param channel : channel
	@param power : send power
	@param sendLv : send sence level
	@param recvLv : receive sence level
	@param sendTim : send sence retry time
	@param sendNum : send sence retry number
	@param ackRetryNum : acknowledge retry number
	@param ackWaitTim : acknowledge retry time
	@return Success : 0 , Failed : From -1 to -15 : Parameter Error, From -16 to -31 Send Error, less than -32 : Receive Error
	@~Japanese
	@brief CONEXIO 920MHz　Module の 無線取得関数
	@param iBitrate : ビットレート
	@param channel : チャネル
	@param power : 送信出力
	@param sendLv : 送信センスレベル
	@param recvLv : 受信センスレベル
	@param sendTim : 送信センス再送時間
	@param sendNum : 送信センス再送回数
	@param ackRetryNum : ACKリトライ回数
	@param ackWaitTim : ACK待ち時間
	@return 成功:  0 失敗 :  送信 エラー:  -1～-15 -16～-31,　受信エラー : -32～
**/
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

/**
	@~English
	@brief CONEXIO 920MHz Module get timer function
	@param tim : time ( msec )
	@return Success : 0 , Failed : From -1 to -15 : Parameter Error, From -16 to -31 Send Error, less than -32 : Receive Error
	@~Japanese
	@brief CONEXIO 920MHz　Module の 時間取得関数
	@param tim : 時間 (msec)
	@return 成功:  0 失敗 :  送信 エラー:  -1～-15 -16～-31,　受信エラー : -32～
**/
int conexio_cmm920_get_timer( unsigned short *tim ){

	return conexio_cmm920_timer(
		CONEXIO_CMM920_SET_READING_READ,
		tim
		);

}

/**
	@~English
	@brief CONEXIO 920MHz Module Get Auto Acknowledge Frame function
	@param phr :
	@param fc_upper :
	@return Success : 0 , Failed : From -1 to -15 : Parameter Error, From -16 to -31 Send Error, less than -32 : Receive Error
	@~Japanese
	@brief CONEXIO 920MHz　Module の オートACKフレーム 取得 関数
	@param phr :
	@param fc_upper :
	@return 成功:  0 失敗 :  送信 エラー:  -1～-15 -16～-31,　受信エラー : -32～
**/
int conexio_cmm920_get_auto_ack_frame( unsigned short *phr, unsigned char *fc_upper ){
	return conexio_cmm920_auto_ack_frame(
		CONEXIO_CMM920_SET_READING_READ,
		phr,
		fc_upper
	);
}
/**
	@~English
	@brief CONEXIO 920MHz Module Get Antenna mode function
	@param antennaMode :
	@return Success : 0 , Failed : From -1 to -15 : Parameter Error, From -16 to -31 Send Error, less than -32 : Receive Error
	@~Japanese
	@brief CONEXIO 920MHz　Module の アンテナモード 取得関数
	@param antennaMode :
	@return 成功:  0 失敗 :  送信 エラー:  -1～-15 -16～-31,　受信エラー : -32～
**/
int conexio_cmm920_get_antenna( BYTE *antennaMode ){
	return conexio_cmm920_antenna(
		CONEXIO_CMM920_SET_READING_READ,
		antennaMode
	);
}

/**
	@~English
	@brief CONEXIO 920MHz Module Set Hop mode function
	@param hop :
	@~Japanese
	@brief CONEXIO 920MHz　Module の ホップモード 設定関数
	@param hop :
**/
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

/**
	@~English
	@brief CONEXIO 920MHz Module Set Wireless default setting function
	@par Default Parameter : 100kBps, 60channel, 1mW , Send Level -85dBm, Receive Level -95dBm, Send Time 130(usec), Send Num 6, Ack Retry Num 4, Ack Time 100(usec)
	@~Japanese
	@brief CONEXIO 920MHz　Module の 無線  通常設定関数
	@par パラメータ : 100kBps, 60チャネル, 1mW , 送信レベル -85dBm, 受信レベル -95dBm, 送信時間 130(usec), 送信回数 6回, ACKリトライ回数 4回, ACKリトライ時間 100 (usec)
**/
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

/**
	@~English
	@brief CONEXIO 920MHz Module Set Auto Acknowledge Frame default setting function
	@par Default Parameter :
	@~Japanese
	@brief CONEXIO 920MHz　Module の 自動ACKフレーム 通常設定関数
	@par パラメータ :
**/
int conexio_cmm920_set_auto_ack_frame_default(void)
{
	return conexio_cmm920_set_auto_ack_frame(
		0x07,
		0x00
	);

}

/**
	@~English
	@brief CONEXIO 920MHz Module Set Timer default setting function
	@par Default Parameter : 10 sec
	@~Japanese
	@brief CONEXIO 920MHz　Module の タイムアウト 通常設定関数
	@par パラメータ :  10秒
**/
int conexio_cmm920_set_timer_default(void)
{
	return conexio_cmm920_set_timer(10000);

}
