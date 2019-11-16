/*
 *  Lib for CONTEC CONPROSYS Analog I/O (CPS-AIO) Series.
 *
 *  Copyright (C) 2015 Syunsuke Okamoto.<okamoto@contec.jp>
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
*
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <malloc.h>
#include "cpsaio.h"

#ifdef CONFIG_CONPROSYS_SDK
 #include "../include/libcpsaio.h"
#else
 #include "libcpsaio.h"
#endif

#ifdef CONPROSYS_MAKEFILE_VERSION
	#define CONTEC_CPSAIO_LIB_VERSION CONPROSYS_MAKEFILE_VERSION
#else
	#define CONTEC_CPSAIO_LIB_VERSION	"1.2.3"
#endif

#define CONTEC_CPSAIO_LIB_EXCHANGE_NONE	0
#define CONTEC_CPSAIO_LIB_EXCHANGE_SINGLE	1
#define CONTEC_CPSAIO_LIB_EXCHANGE_MULTI	2

typedef struct __contec_aio_param__{

	short channel;
	double clock;	// ver.1.2.3
	short stopTrig;
	unsigned long stopTime;

}CONTEC_CPS_AIO_PARAMETER, *PCONTEC_CPS_AIO_PARAMETER;


typedef struct __contec_cps_aio_int_callback__
{
	PCONTEC_CPS_AIO_INT_CALLBACK func;
	CONTEC_CPS_AIO_INT_CALLBACK_DATA data;
}CONTEC_CPS_AIO_INT_CALLBACK_LIST, *PCONTEC_CPS_AIO_INT_CALLBACK_LIST;

CONTEC_CPS_AIO_INT_CALLBACK_LIST contec_cps_aio_cb_list[CPS_DEVICE_MAX_NUM];

/**
	@~English
	@brief callback process function.(The running process is called to receive user's signal.)
	@param signo : signal number
	@~Japanese
	@param signo : シグナルナンバー
	@brief コールバック内部関数　（ユーザシグナル受信で動作）
**/
void _contec_cpsaio_signal_proc( int signo )
{
	int cnt;
	if( signo == SIGUSR2 ){
		for( cnt = 0; cnt < CPS_DEVICE_MAX_NUM ; cnt ++){
			if( contec_cps_aio_cb_list[cnt].func != (PCONTEC_CPS_AIO_INT_CALLBACK)NULL ){
				

				contec_cps_aio_cb_list[cnt].func(
					contec_cps_aio_cb_list[cnt].data.id,
					AIOM_INTERRUPT,
					contec_cps_aio_cb_list[cnt].data.wParam,
					contec_cps_aio_cb_list[cnt].data.lParam,
					contec_cps_aio_cb_list[cnt].data.Param
				);
			}
		}
	}
}
/**
	@~English
	@brief set exchange function.
	@param Id : Device ID
	@param isOutput : "Analog Input" or "Analog Output" Flag
	@param isMulti : Get data type "Single" or "Multi" Channel.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief AI or AOのデータを単数チャネルで取得するか　複数チャネル分取得するかを設定する関数.
	@param Id : デバイスID
	@param isOutput : "アナログ入力"か"アナログ出力"か
	@param isMulti : "Single"チャネルか "Multi"チャネルか
	@return 成功:  AIO_ERR_SUCCESS
**/
unsigned long _contec_cpsaio_set_exchange( short Id, unsigned char isOutput, unsigned char isMulti )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long request = 0;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	/* Single or Multi */
	switch ( isMulti ){
		case CONTEC_CPSAIO_LIB_EXCHANGE_SINGLE:
			arg.val = 0;
			break;
		case CONTEC_CPSAIO_LIB_EXCHANGE_MULTI:
			arg.val = 1;
			break;
		default:
			return AIO_ERR_OTHER;
	}

	/* Ai or Ao */
	switch (isOutput)
	{
		case CPS_AIO_INOUT_AO:
			request = IOCTL_CPSAIO_SETTRANSFER_MODE_AO;
			break;
		case CPS_AIO_INOUT_AI:
			request = IOCTL_CPSAIO_SETTRANSFER_MODE_AI;
			break;	
		default:
			return AIO_ERR_OTHER;
	}

	// arg.inout = isOutput;
	iRet = ioctl( Id, request, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return AIO_ERR_SUCCESS;

}

/**
	@~English
	@brief Check Memory Flag function.
	@param Id : Device ID
	@param isCheckMemFlag : Memory Status Bit Flag
	@note Added Version 1.0.8.
	@par This is internal function.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief メモリのフラグを確認する関数
	@param Id : デバイスID
	@param isCheckMemFlag : メモリStatusビット確認用フラグ
	@note Ver1.0.8より追加
	@par この関数は内部関数です。
	@return 成功:  AIO_ERR_SUCCESS
**/
unsigned long _contec_cpsaio_check_memstatus( short Id, unsigned char isCheckMemFlag )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long count = 0;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	do{
		usleep( 1 );
		iRet = ioctl( Id, IOCTL_CPSAIO_GETMEMSTATUS , &arg);
		if( iRet < 0 ){
			ulRet = AIO_ERR_DLL_CALL_DRIVER;
			break;
		}		
		
		if( count >= 1000 ){
			ulRet = AIO_ERR_INTERNAL_TIMEOUT;
			break;
		}
		count ++;
	}while(!( arg.val & isCheckMemFlag ) );

	return ulRet;
}

/**
 * @~English
 * @brief 
 * @param Id : Device ID
 * @param isOutput : CPS_AIO_INOUT_AI or CPS_AIO_INOUT_AO
 * @param swapData : CONTEC_CPS_AIO_PARAMETER structure pointer
 * @par This is internal function.
 * @return Success: AIO_ERR_SUCCESS
 * @~Japanese
 * @brief 
 * @param Id : デバイスID 
 * @param isOutput : CPS_AIO_INOUT_AI or CPS_AIO_INOUT_AO
 * @param swapData : CONTEC_CPS_AIO_PARAMETER ポインタ構造体
 * @return 成功:  AIO_ERR_SUCCESS
 */
unsigned long _contec_cpsaio_singlemulti_getParam(short Id, unsigned char isOutput, PCONTEC_CPS_AIO_PARAMETER swapData )
{

	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;

	if( isOutput == CPS_AIO_INOUT_AO){

		//ulRet = ContecCpsAioGetAoChannels(Id, setData.channel);

		//ulRet = ContecCpsAioSetAoEventSamplingTimes( Id, setData.stopTrig );
	}else if(isOutput == CPS_AIO_INOUT_AI){
		// Get Ai Channels
		ulRet = ContecCpsAioGetAiChannels(Id, &(swapData->channel) );
		// Get Ai Sampling Clock
		if( ulRet == AIO_ERR_SUCCESS )
			ulRet = ContecCpsAioGetAiSamplingClock(Id, &(swapData->clock) );
		// Get Sampling Stop Trigger
		if( ulRet == AIO_ERR_SUCCESS )
			ulRet = ContecCpsAioGetAiStopTrigger(Id, &(swapData->stopTrig) );
		// Get StopTimes
		if( ulRet == AIO_ERR_SUCCESS )
			ulRet = ContecCpsAioGetAiStopTimes(Id, &(swapData->stopTime));
	}

	return ulRet;
}
/**
 * @~English
 * @brief 
 * @param Id : Device ID
 * @param isOutput : CPS_AIO_INOUT_AI or CPS_AIO_INOUT_AO
 * @param isMulti : 0...CONTEC_CPSAIO_LIB_EXCHANGE_NONE , 1...CONTEC_CPSAIO_LIB_EXCHANGE_SINGLE 2 ... CONTEC_CPSAIO_LIB_EXCHANGE_MULTI
 * @param setData : CONTEC_CPS_AIO_PARAMETER structure pointer
 * @par This is internal function.
 * @return Success: AIO_ERR_SUCCESS
 * @~Japanese
 * @brief 
 * @param Id : デバイスID 
 * @param isOutput : CPS_AIO_INOUT_AI or CPS_AIO_INOUT_AO
 * @param isMulti : 0...CONTEC_CPSAIO_LIB_EXCHANGE_NONE , 1...CONTEC_CPSAIO_LIB_EXCHANGE_SINGLE 2 ... CONTEC_CPSAIO_LIB_EXCHANGE_MULTI
 * @param setData : CONTEC_CPS_AIO_PARAMETER ポインタ構造体
 * @return 成功:  AIO_ERR_SUCCESS
 */
unsigned long _contec_cpsaio_singlemulti_storeParam(short Id, unsigned char isOutput, unsigned char isMulti, CONTEC_CPS_AIO_PARAMETER setData )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;

	if( isOutput == CPS_AIO_INOUT_AO ){
		if( isMulti == CONTEC_CPSAIO_LIB_EXCHANGE_SINGLE){
			
			_contec_cpsaio_set_exchange(Id, isOutput, isMulti);

			arg.val = setData.channel;
			ulRet = ioctl( Id, IOCTL_CPSAIO_SETCHANNEL_AO, &arg );

		}else{
			ulRet = ContecCpsAioSetAoChannels(Id, setData.channel);
		}
		if( ulRet == AIO_ERR_SUCCESS ){
			// SetSampling Clock
			ulRet = ContecCpsAioSetAoSamplingClock( Id, setData.clock );		
		}
		// SetSampling Trigger
		//ulRet = ContecCpsAioSetAoStopTrigger( Id, 0 );
		if( ulRet == AIO_ERR_SUCCESS ){
			// Set Sampling Number
			ulRet = ContecCpsAioSetAoEventSamplingTimes( Id, setData.stopTrig );
		}
	}else if(isOutput == CPS_AIO_INOUT_AI){
		// Set Ai Channel
		if( isMulti == CONTEC_CPSAIO_LIB_EXCHANGE_SINGLE ){
			// Exchange Transfer Mode Single Ai
			_contec_cpsaio_set_exchange( Id, isOutput, isMulti );
			arg.val = setData.channel;

			ulRet = ioctl( Id, IOCTL_CPSAIO_SETCHANNEL_AI, &arg );
		}else{
			ulRet = ContecCpsAioSetAiChannels(Id, setData.channel);
		}
		if( ulRet == AIO_ERR_SUCCESS ){
			// SetSampling Clock
			ulRet = ContecCpsAioSetAiSamplingClock( Id, setData.clock );		
		}
		if( ulRet == AIO_ERR_SUCCESS ){
			// SetSampling Trigger
			ulRet = ContecCpsAioSetAiStopTrigger( Id, setData.stopTrig );
		}
		if( ulRet == AIO_ERR_SUCCESS ){
			// Set Sampling Number
			ulRet = ContecCpsAioSetAiStopTimes( Id, setData.stopTime );
		}
	}

	return ulRet;
}


/**
	@~English
	@brief AIO Library Initialize.
	@param DeviceName : Device node name ( cpsaioX )
	@param Id : Device Access Id
	@return Success: AIO_ERR_SUCCESS, Failed: otherwise AIO_ERR_SUCCESS
	@~Japanese
	@brief 初期化関数.
	@param DeviceName : デバイスノード名  ( cpsaioX )
	@param Id : デバイスID
	@return 成功: AIO_ERR_SUCCESS, 失敗: AIO_ERR_SUCCESS 以外

**/
unsigned long ContecCpsAioInit( char *DeviceName, short *Id )
{
	// open
	char Name[32];
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;	
	int fd = 0;	
	//unsigned char g = 0, o = 0;

	// NULL Pointer Checks
	if( DeviceName == ( char * )NULL )
		return AIO_ERR_PTR_DEVICE_NAME;
	if( Id == ( short * )NULL )
		return AIO_ERR_DLL_INVALID_ID;

	if( (strlen(DeviceName) + 5)  > 32 )
		return AIO_ERR_DLL_CREATE_FILE;

	strcpy(Name, "/dev/");
	strcat(Name, DeviceName);

	fd = open( Name, O_RDWR );

	if( fd < 0 ) return AIO_ERR_DLL_CREATE_FILE;

	*Id = fd;

	ulRet = ContecCpsAioResetDevice( *Id );

	// ioctl( *Id, IOCTL_CPSAIO_INIT, &arg);

	// ContecCpsAioSetEcuSignal(*Id, AIOECU_DEST_AI_CLK, AIOECU_SRC_AI_CLK );
	// ContecCpsAioSetEcuSignal(*Id, AIOECU_DEST_AI_START, AIOECU_SRC_START );
	// ContecCpsAioSetEcuSignal(*Id, AIOECU_DEST_AI_STOP, AIOECU_SRC_AI_STOP );

	// ContecCpsAioSetEcuSignal(*Id, AIOECU_DEST_AO_CLK, AIOECU_SRC_AO_CLK );
	// ContecCpsAioSetEcuSignal(*Id, AIOECU_DEST_AO_START, AIOECU_SRC_START );
	// ContecCpsAioSetEcuSignal(*Id, AIOECU_DEST_AO_STOP, AIOECU_SRC_AO_STOP_RING );


//	ContecCpsAioReadAiCalibrationData(*Id, 0, &g, &o);
//	ContecCpsAioSetAiCalibrationData(*Id, CPSAIO_AI_CALIBRATION_SELECT_OFFSET, 0, CPSAIO_AI_CALIBRATION_RANGE_PM10,  o);
//	ContecCpsAioSetAiCalibrationData(*Id, CPSAIO_AI_CALIBRATION_SELECT_GAIN, 0, CPSAIO_AI_CALIBRATION_RANGE_PM10,  g);
	return ulRet;

}

/**
	@~English
	@brief AIO Library Exit.
	@param Id : Device ID
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief 終了関数.
	@param Id : デバイスID
	@return 成功:  AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioExit( short Id )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	arg.val = 0;

	iRet = ioctl( Id, IOCTL_CPSAIO_EXIT, &arg );
	
	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	// close
	close( Id );
	return ulRet;

}

/**
	@~English
	@brief AIO Library output from ErrorNumber to ErrorString.
	@param code : Error Code
	@param Str : Error String
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief エラー文字列出力関数（未実装）
	@param code : エラーコード
	@param Str : エラー文字列
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetErrorStrings( unsigned long code, char *Str )
{
	unsigned long ulRet = AIO_ERR_SUCCESS;
	//int iRet = 0;

	return ulRet;
}

/**
	@~English
	@brief AIO Library query Device.
	@param Index : Device ID
	@param DeviceName : Device Node Name ( cpsaioX )
	@param Device : Device Name ( CPS-AI-1608LI , etc )
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief クエリデバイス関数
	@param Index : デバイスID
	@param DeviceName : デバイスノード名 ( cpsaioX )
	@param Device : デバイス型式名 (  CPS-AIO-1608LIなど )
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioQueryDeviceName( short Index, char *DeviceName, char *Device )
{
	struct cpsaio_ioctl_string_arg	arg;
	int len;

	char tmpDevName[16];
	char baseDeviceName[16]="cpsaio";
	char strNum[2]={0};
	int findNum=0, cnt;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	short tmpId = 0;
	int iRet = 0;

	// NULL Pointer Checks
	if( DeviceName == ( char * )NULL )	return AIO_ERR_PTR_DEVICE_NAME;
	if( Device == ( char * )NULL )	return AIO_ERR_PTR_DEVICE;

	for(cnt = 0;cnt < CPS_DEVICE_MAX_NUM ; cnt ++ ){
		sprintf(tmpDevName,"%s%x",baseDeviceName, cnt);
		ulRet = ContecCpsAioInit(tmpDevName, &tmpId);

		if( ulRet == AIO_ERR_SUCCESS ){
			iRet = ioctl(tmpId, IOCTL_CPSAIO_GET_DEVICE_NAME, &arg);
			ContecCpsAioExit(tmpId);

			if( iRet >= 0 ){

				if(findNum == Index){
					sprintf(DeviceName,"%s",tmpDevName);
					sprintf(Device,"%s", arg.str);
					return AIO_ERR_SUCCESS;
				}else{
					findNum ++;
				}
			}

			memset(&tmpDevName, 0x00, 16);
			memset(&arg.str, 0x00, sizeof(arg.str)/sizeof(arg.str[0]));

		}
	}
	
	return AIO_ERR_INFO_NOT_FIND_DEVICE;
}

unsigned long ContecCpsAioResetDevice(short Id){

	struct cpsaio_ioctl_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	iRet = ioctl( Id, IOCTL_CPSAIO_INIT, &arg);
	if( iRet < 0 )
		return AIO_ERR_DLL_CALL_DRIVER;

	ulRet = ContecCpsAioSetEcuSignal(Id, AIOECU_DEST_AI_CLK, AIOECU_SRC_AI_CLK );
	ulRet = ContecCpsAioSetEcuSignal(Id, AIOECU_DEST_AI_START, AIOECU_SRC_START );
//	ulRet = ContecCpsAioSetEcuSignal(Id, AIOECU_DEST_AI_STOP, AIOECU_SRC_AI_STOP );

	ulRet = ContecCpsAioSetAiStopTrigger( Id, CPSAIO_AI_STOPTRG_0 );

	ulRet = ContecCpsAioSetEcuSignal(Id, AIOECU_DEST_AO_CLK, AIOECU_SRC_AO_CLK );
	ulRet = ContecCpsAioSetEcuSignal(Id, AIOECU_DEST_AO_START, AIOECU_SRC_START );
	ulRet = ContecCpsAioSetEcuSignal(Id, AIOECU_DEST_AO_STOP, AIOECU_SRC_AO_STOP_RING );		

	return ulRet;
}


//---- Ai/Ao Get Resolution function ------------------
/**
	@~English
	@brief DIO Library get analog input resolution.
	@param Id : Device ID
	@param AiResolution : Resolution of AnalogInput.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力の分解能を取得します
	@param Id : デバイスID
	@param AiResolution : アナログ入力の分解能
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiResolution( short Id, unsigned short *AiResolution )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( AiResolution == ( unsigned short * )NULL )
		return AIO_ERR_PTR_AI_RESOLUTION;

	/* Get resolution */
	arg.inout = CPS_AIO_INOUT_AI;
	iRet = ioctl( Id, IOCTL_CPSAIO_GETRESOLUTION, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*AiResolution = (unsigned short)arg.val;

	return ulRet;
}

/**
	@~English
	@brief DIO Library get analog output resolution.
	@param Id : Device ID
	@param AoResolution : Resolution of Analog Output.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力の分解能を取得します
	@param Id : デバイスID
	@param AoResolution : アナログ出力の分解能
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAoResolution( short Id, unsigned short *AoResolution )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( AoResolution == ( unsigned short * )NULL )	return AIO_ERR_PTR_AO_RESOLUTION;

	/* Get resolution */
	arg.inout = CPS_AIO_INOUT_AO;
	iRet = ioctl( Id, IOCTL_CPSAIO_GETRESOLUTION, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*AoResolution = (unsigned short)arg.val;

	return ulRet;
}

//---- Ai Channel function ----------------------------
/**
	@~English
	@brief AIO Library get maximum analog input channels.
	@param Id : Device ID
	@param AiMaxChannels : analog input max channel number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスの最大の入力チャネル数を取得します
	@param Id : デバイスID
	@param AiMaxChannels :アナログ入力の最大チャネル数
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiMaxChannels( short Id, short *AiMaxChannels ){
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( AiMaxChannels == ( short * )NULL )
		return AIO_ERR_PTR_AI_MAX_CHANNELS;

	memset(&arg, 0, sizeof(struct cpsaio_ioctl_arg));
	arg.inout = CPS_AIO_INOUT_AI;

	iRet = ioctl( Id, IOCTL_CPSAIO_GETMAXCHANNEL, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else	
		*AiMaxChannels = arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library gets driver and library version.
	@param Id : Device ID
	@param libVer : library version
	@param drvVer : driver version
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログデバイスのドライバとライブラリのバージョン文字列を取得します。
	@param Id : デバイスID
	@param libVer : ライブラリバージョン
	@param drvVer : ドライババージョン
	@note Ver.1.0.8 Change from cpsaio_ioctl_arg to cpsaio_ioctl_string_arg.
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetVersion( short Id , unsigned char libVer[] , unsigned char drvVer[] )
{

	struct cpsaio_ioctl_string_arg	arg;
	int len;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	iRet = ioctl( Id, IOCTL_CPSAIO_GET_DRIVER_VERSION, &arg );

	if( iRet < 0 )
		return AIO_ERR_DLL_CALL_DRIVER;

	len = sizeof( arg.str ) / sizeof( arg.str[0] );
	memcpy(drvVer, arg.str, len);
//	strcpy_s(drvVer, arg.str);

	len = sizeof( CONTEC_CPSAIO_LIB_VERSION ) /sizeof( unsigned char );
	memcpy(libVer, CONTEC_CPSAIO_LIB_VERSION, len);

	return ulRet;

}

/**
	@~English
	@brief AIO Library set analog input channel.
	@param Id : Device ID
	@param AiChannels : analog input channel number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスを計測する入力チャネルを設定します
	@param Id : デバイスID
	@param AiChannels :　アナログ入力のチャネル番号
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetAiChannels( short Id, short AiChannels )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	if( AiChannels > 1){
		_contec_cpsaio_set_exchange( Id, CPS_AIO_INOUT_AI, CONTEC_CPSAIO_LIB_EXCHANGE_MULTI );
	}else{
		_contec_cpsaio_set_exchange( Id, CPS_AIO_INOUT_AI, CONTEC_CPSAIO_LIB_EXCHANGE_SINGLE );
	}
	/* Set Channel */
	arg.val = AiChannels - 1;
	iRet = ioctl( Id, IOCTL_CPSAIO_SETCHANNEL_AI, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;
}

/**
	@~English
	@brief AIO Library get analog input channel.
	@param Id : Device ID
	@param AiChannels : analog input channel number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスを計測する入力チャネルを取得します
	@param Id : デバイスID
	@param AiChannels :　アナログ入力のチャネル番号
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiChannels( short Id, short *AiChannels )
{
	struct cpsaio_ioctl_arg	arg;
	int iRet = 0;
	unsigned long ulRet = AIO_ERR_SUCCESS;

	// NULL Pointer Checks
	if( AiChannels == ( short * )NULL )	return AIO_ERR_PTR_AI_CHANNELS;	// AiChannels Null Pointer

	arg.inout = CPS_AIO_INOUT_AI;

	/* Get Channel */
	iRet = ioctl( Id, IOCTL_CPSAIO_GETCHANNEL, &arg );
	if( iRet < 0 ){
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	}else{
		*AiChannels = (short)(arg.val + 1);
	}

	return ulRet;
}

/**
	@~English
	@brief AIO Library set analog input sampling clock.( set time micro second order.)
	@param Id : Device ID
	@param AiSamplingClock : analog input channel number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力のサンプリング時間を設定します。単位は usecです。
	@param Id : デバイスID
	@param AiSamplingClock :　サンプリング時間
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetAiSamplingClock( short Id, double AiSamplingClock )
{
	struct cpsaio_ioctl_arg	arg;
	int iRet = 0;
	unsigned long ulRet = AIO_ERR_SUCCESS;

	/* Set Channel */
	arg.val = (unsigned long) ( (AiSamplingClock * 1000.0 / 25.0) - 1 );
	iRet = ioctl( Id, IOCTL_CPSAIO_SET_CLOCK_AI, &arg );

	if( iRet < 0 ){
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	}

	return ulRet;
}

/**
	@~English
	@brief AIO Library get analog input sampling clock.( set time micro second order.)
	@param Id : Device ID
	@param AiSamplingClock : analog input sampling clock.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力のサンプリング時間を取得します。単位は usecです。
	@param Id : デバイスID
	@param AiSamplingClock :　サンプリング時間
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiSamplingClock( short Id, double *AiSamplingClock )
{
	struct cpsaio_ioctl_arg	arg;
	int iRet = 0;
	unsigned long ulRet = AIO_ERR_SUCCESS;	
	// NULL Pointer Checks
	if( AiSamplingClock == ( double * )NULL )
		return AIO_ERR_PTR_AI_SAMPLING_CLOCK;

	arg.inout = CPS_AIO_INOUT_AI;
	/* Get Channel */
	iRet = ioctl( Id, IOCTL_CPSAIO_GET_CLOCK, &arg );

	if( iRet < 0 ){
		ulRet = AIO_ERR_DLL_CALL_DRIVER; 
		*AiSamplingClock = 0;
	}else{
		*AiSamplingClock = (double) ( (arg.val + 1.0 ) * 25.0 ) / 1000.0;
	}

	return iRet;

}

/**
	@~English
	@brief AIO Library set analog input sampling number.
	@param Id : Device ID
	@param AiStopTrigger : Stop Trigger Pattern.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief 変換停止条件の設定を行います。
	@param Id : デバイスID
	@param AiStopTrigger : 変換停止条件
	@return 成功: AIO_ERR_SUCCESS				
**/
unsigned long ContecCpsAioSetAiStopTrigger( short Id, short AiStopTrigger )
{
	unsigned long ulRet = AIO_ERR_SUCCESS;
	struct cpsaio_ioctl_arg	arg;
	int iRet = 0;

	arg.val = AiStopTrigger;
	arg.inout = CPS_AIO_INOUT_AI;
	arg.isDerection = CPS_AIO_DERECTION_SET;

	iRet =  ioctl( Id, IOCTL_CPSAIO_STOPTRIGGER_TYPE, &arg );

	if( iRet < 0 ){
		ulRet = AIO_ERR_DLL_CALL_DRIVER; 
	}else{
		switch (AiStopTrigger)
		{
			case CPSAIO_AI_STOPTRG_0:
				ulRet = ContecCpsAioSetEcuSignal(Id, AIOECU_DEST_AI_STOP, AIOECU_SRC_AI_STOP );
				break;
			case CPSAIO_AI_STOPTRG_4:
	//			ulRet = ContecCpsAioSetEcuSignal(Id, AIOECU_DEST_AI_STOP, AIOECU_SRC_START );
				ulRet = ContecCpsAioSetEcuSignal(Id, AIOECU_DEST_AI_STOP, CPS_AIO_ECU_SRC_NON_CONNECT );
				break;
			default:
				ulRet = AIO_ERR_OUT_OF_VALUE_AI_STOPTRIGGER;
		}
	}
	return ulRet;
}

/**
	@~English
	@brief AIO Library set analog input sampling number.
	@param Id : Device ID
	@param AiSamplingTimes : analog input sample number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief 変換停止条件の取得を行います。
	@param Id : デバイスID
	@param AiSamplingTimes :　入力サンプリング数
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiStopTrigger( short Id, short *AiStopTrigger )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;	
	int iRet = 0;

	// NULL Pointer Checks
	if( AiStopTrigger == ( short * )NULL )	
		return AIO_ERR_PTR_AI_STOPTRIGGER;

	arg.inout = CPS_AIO_INOUT_AI;
	arg.isDerection = CPS_AIO_DERECTION_GET;

	iRet = ioctl( Id, IOCTL_CPSAIO_STOPTRIGGER_TYPE, &arg );
	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*AiStopTrigger = arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library get analog input sampling start trigger.
	@param Id : Device ID
	@param AiStartTrigger : analog input start trigger.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief サンプリング数の取得を行います。
	@param Id : デバイスID
	@param AiSamplingTimes :　サンプリング数
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetAiStartTrigger( short Id, short AiStartTrigger )
{
	unsigned long ulRet = AIO_ERR_SUCCESS;
	switch (AiStartTrigger)
	{
		case CPSAIO_AI_STARTTRG_SOFT:	//Soft Trigger
			ulRet = ContecCpsAioSetEcuSignal(Id, AIOECU_DEST_AI_START, AIOECU_SRC_START );
			break;
		default:
			ulRet = AIO_ERR_OUT_OF_VALUE_AI_STARTTRIGGER;
	}
	return ulRet;
}

/**
	@~English
	@brief AIO Library set analog input sampling number.
	@param Id : Device ID
	@param AiSamplingTimes : analog input sample number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief 入力サンプリング数の設定を行います。
	@param Id : デバイスID
	@param AiSamplingTimes :　入力サンプリング数
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetAiStopTimes( short Id, unsigned long AiSamplingTimes )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;	
	int iRet = 0;

	/* Set Sampling number( before Trigger ) */
	arg.val = AiSamplingTimes - 1;
	iRet = ioctl( Id, IOCTL_CPSAIO_SET_SAMPNUM_AI, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;

}

/**
	@~English
	@brief AIO Library get analog input sampling number.
	@param Id : Device ID
	@param AiSamplingTimes : analog input sample number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief サンプリング数の取得を行います。
	@param Id : デバイスID
	@param AiSamplingTimes :　サンプリング数
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiStopTimes( short Id, unsigned long *AiSamplingTimes )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( AiSamplingTimes == ( unsigned long * )NULL )
		return AIO_ERR_PTR_AI_SAMPLINGTIMES;

	arg.inout = CPS_AIO_INOUT_AI;
	/* Get Sampling number( before Trigger ) */
	iRet = ioctl( Id, IOCTL_CPSAIO_GET_SAMPNUM, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*AiSamplingTimes = arg.val + 1;

	return ulRet;

}

/**
	@~English
	@brief AIO Library get analog input sampling number.
	@param Id : Device ID
	@param AiSamplingTimes : analog input sample number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief 指定サンプリング回数格納イベントを発生させるためのサンプリング数の設定を行います。
	@param Id : デバイスID
	@param AiSamplingTimes :　サンプリング数
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetAiEventSamplingTimes( short Id, unsigned long AiSamplingTimes )
{
	// struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;	
	// int iRet = 0;

	// arg.inout = CPS_AIO_INOUT_AI;
	// arg.val = AiSamplingTimes;
	// iRet = ioctl( Id, IOCTL_CPSAIO_SETXXXXX, &arg );

	//if( iRet < 0 )
	//	ulRet = AIO_ERR_DLL_CALL_DRIVER;
	

	return ulRet;
}

/**
	@~English
	@brief AIO Library get analog input sampling number.
	@param Id : Device ID
	@param AiSamplingTimes : analog input sample number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief 指定サンプリング回数格納イベントを発生させるためのサンプリング数の取得を行います。
	@param Id : デバイスID
	@param AiSamplingTimes :　サンプリング数
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiEventSamplingTimes( short Id, unsigned long *AiSamplingTimes )
{
	// struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;	
	// int iRet = 0;

	// NULL Pointer Checks
	if( AiSamplingTimes == ( unsigned long * )NULL )
		return AIO_ERR_PTR_AI_SAMPLINGTIMES;

	// arg.inout = CPS_AIO_INOUT_AI;
	// iRet = ioctl( Id, IOCTL_CPSAIO_GETXXXXX, &arg );

	//if( iRet < 0 )
	//	ulRet = AIO_ERR_DLL_CALL_DRIVER;
	//else
	//	*AiStopTrigger = arg.val;

	return ulRet;
}


//--- Memory Functions -------------------------
/**
	@~English
	@brief AIO Library start analog input sampling.
	@param Id : Device ID
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力メモリの設定。
	@param Id : デバイスID
	@note FIFO 固定のため、設定なし
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetAiMemoryType( short Id , unsigned short AiMemoryType)
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	arg.inout = CPS_AIO_INOUT_AI;
	arg.isDerection = CPS_AIO_DERECTION_SET;
	arg.val = AiMemoryType;
	iRet = ioctl( Id, IOCTL_CPSAIO_MEMORY_TYPE, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;
}

/**
	@~English
	@brief AIO Library start analog input sampling.
	@param Id : Device ID
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力メモリの取得。
	@param Id : デバイスID
	@note FIFOのみ。
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiMemoryType( short Id , unsigned short *AiMemoryType)
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( AiMemoryType == ( unsigned short * )NULL )
		return AIO_ERR_OTHER;

	arg.inout = CPS_AIO_INOUT_AI;
	arg.isDerection = CPS_AIO_DERECTION_GET;	
	iRet = ioctl( Id, IOCTL_CPSAIO_MEMORY_TYPE, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else	
		*AiMemoryType = arg.val;

	return AIO_ERR_SUCCESS;
}

//--- Running Functions ------------------------
/**
	@~English
	@brief AIO Library start analog input sampling.
	@param Id : Device ID
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力サンプリングの開始。
	@param Id : デバイスID
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioStartAi( short Id )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRepeatTime = 1;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;
	
//	arg.inout = CPS_AIO_INOUT_AI;
	iRet = ioctl( Id, IOCTL_CPSAIO_START_AI, 0 );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	// Ver.1.1.1 without CPS_AIO_AI_STATUS_START_DISABLE 
	// implement cpsaio driver 
	// do{
	// 	usleep( 1 );
	// 	ioctl( Id, IOCTL_CPSAIO_INSTATUS, &arg );
	// 	if( count >= 1000 ) return 1;
	// 	count ++;
	// }while( arg.val & CPS_AIO_AI_STATUS_START_DISABLE );
	//
	////softtrigger only out pulse start
	//ioctl( Id, IOCTL_CPSAIO_SET_OUTPULSE0, 0 );

	return ulRet;
}

/**
	@~English
	@brief AIO Library stop analog input sampling.
	@param Id : Device ID
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力サンプリングの停止。
	@param Id : デバイスID
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioStopAi( short Id )
{
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

//	arg.inout = CPS_AIO_INOUT_AI;	
	iRet = ioctl( Id, IOCTL_CPSAIO_STOP_AI, 0 );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return AIO_ERR_SUCCESS;
}

/**
	@~English
	@brief AIO Library get analog input status.
	@param Id : Device ID
	@param AiStatus : status of analog input
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力デバイスの状態の取得。
	@param Id : デバイスID
	@param AiStatus : アナログ入力のステータス
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiStatus( short Id, long *AiStatus )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( AiStatus == ( long * )NULL )
		return AIO_ERR_PTR_AI_STATUS;

//	arg.inout = CPS_AIO_INOUT_AI;

	iRet = ioctl( Id, IOCTL_CPSAIO_INSTATUS, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*AiStatus = (long)arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library get analog sampling count length.
	@param Id : Device ID
	@param AiSamplingCount : status of analog input sampling count length.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力デバイスのサンプリングデータ長を取得する。
	@param Id : デバイスID
	@param AiSamplingCount : アナログ入力のサンプリングデータ長
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiSamplingCount( short Id, long *AiSamplingCount )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( AiSamplingCount == (long *)NULL)
		return AIO_ERR_PTR_AI_SAMPLING_COUNT;

	iRet = ioctl( Id, IOCTL_CPSAIO_GET_SAMPLING_COUNT_AI, &arg );
	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*AiSamplingCount = (long)arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library get analog input sampling data.
	@param Id : Device ID
	@param AiSamplingTimes : set the Getting Data length
	@param AiData : get Data of analog input
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力デバイスのサンプリングデータを取得。(16bit)
	@param Id : デバイスID
	@param AiSamplingTimes : サンプリング数
	@param AiData : アナログ入力データ配列
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiSamplingData( short Id, long *AiSamplingTimes, long AiData[] )
{
	unsigned char *tmpAiData = (unsigned char *)NULL;
	long tmpAiCount = 0;
	int cnt = 0;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( AiSamplingTimes == (long *)NULL ){
		return AIO_ERR_PTR_AI_SAMPLINGTIMES;
	}
	if( AiData == (long*)NULL ){
		return AIO_ERR_PTR_AI_DATA;
	}

	ulRet = ContecCpsAioGetAiSamplingCount(Id, &tmpAiCount);

	if( ulRet == AIO_ERR_SUCCESS ){

		/* Sampling Count Checks */
		if( *AiSamplingTimes > tmpAiCount )
			*AiSamplingTimes = tmpAiCount;

		if( *AiSamplingTimes >= CPSAIO_MAX_BUFFER )
			*AiSamplingTimes = CPSAIO_MAX_BUFFER;

		tmpAiData = (unsigned char * )malloc( sizeof(unsigned char)* (*AiSamplingTimes) * 2 );

		if( tmpAiData == (unsigned char *) NULL ){
			return AIO_ERR_INI_MEMORY;
		}

		iRet = read( Id, tmpAiData , (size_t)(*AiSamplingTimes * 2) );

		if( iRet < 0 ){
			ulRet = AIO_ERR_DLL_CALL_DRIVER;
			free(tmpAiData);
			*AiSamplingTimes = 0;
			return ulRet;
		}

		if ( ulRet < *AiSamplingTimes * 2 ){
			*AiSamplingTimes = ulRet / 2;	// ucharのLengthのため ushortの数にあわせるため 2でわる
		}

		for( cnt = 0;cnt < *AiSamplingTimes * 2; cnt += 2 ){
			AiData[cnt/2] = (long) ( ( tmpAiData[cnt+1] << 8 ) | tmpAiData[cnt]);
		}
	}

	if( tmpAiData != (unsigned char *) NULL ){
		free(tmpAiData);
	}

	return ulRet;
}
/**
	@~English
	@brief AIO Library get analog input sampling data.( double type )
	@param Id : Device ID
	@param AiSamplingTimes : set the Getting Data length
	@param AiData : get Data of analog input
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力デバイスのサンプリングデータを取得。(浮動小数点型)
	@param Id : デバイスID
	@param AiSamplingTimes : サンプリング数
	@param AiData : アナログ入力データ配列
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiSamplingDataEx( short Id, long *AiSamplingTimes, double AiData[] )
{

	long *tmpAiData = (long *) NULL;
	long tmpAiCount = 0;
	unsigned short AiResolution = 0;
	int cnt = 0;
	double dblMin, dblMax;
	unsigned long ulRet = AIO_ERR_SUCCESS;

	// NULL Pointer Checks
	if( AiSamplingTimes == (long *)NULL ){
		return AIO_ERR_PTR_AI_SAMPLINGTIMES;
	}

	if( AiData == (double*)NULL ){
		return AIO_ERR_PTR_AI_DATA;
	}

	ulRet = ContecCpsAioGetAiSamplingCount(Id, &tmpAiCount);

	if( ulRet == AIO_ERR_SUCCESS ){
		/* Sampling Count Checks */
		if( *AiSamplingTimes > tmpAiCount )
			*AiSamplingTimes = tmpAiCount;

		if( *AiSamplingTimes >= CPSAIO_MAX_BUFFER )
			*AiSamplingTimes = CPSAIO_MAX_BUFFER;

		if( *AiSamplingTimes > tmpAiCount )
			*AiSamplingTimes = tmpAiCount;

		tmpAiData = (long*)malloc( sizeof(long) * (*AiSamplingTimes) );

		if( tmpAiData == (long *) NULL ){
			return AIO_ERR_INI_MEMORY;
		}

		ulRet = ContecCpsAioGetAiSamplingData( Id, AiSamplingTimes, tmpAiData );
	
		if( ulRet != AIO_ERR_SUCCESS ){
//			free(tmpAiData);
			*AiSamplingTimes = 0;
//			return ulRet;
		}
	}

	if( ulRet == AIO_ERR_SUCCESS ){
		ulRet = ContecCpsAioGetAiResolution( Id, &AiResolution );
	}

	if( ulRet == AIO_ERR_SUCCESS ){

/*
	ContecCpsAioGetAiRange( Id, &AiRange ); 
	switch( AiRange ){
		case PM10 :
*/
			dblMax = 10.0; dblMin = -10.0;
/*
			break;
	}
*/	
		for( cnt = 0;cnt < *AiSamplingTimes; cnt ++){
			AiData[cnt] =  (double)( tmpAiData[cnt] / pow(2.0,AiResolution) ) *(dblMax - dblMin) + dblMin;
		}
	}

	if( tmpAiData != (long *) NULL ){
		free(tmpAiData);
	}

	return ulRet;
}

/**
	@~English
	@brief AIO Library get channel of analog input device sampling one data.( unsigned short type )
	@param Id : Device ID
	@param AiChannel : set the Data Channel
	@param AiData : get Data of analog input
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力デバイスの指定したチャネルのサンプリングデータを一回取得。(16bit)
	@param Id : デバイスID
	@param AiChannel : チャネル番号
	@param AiData : アナログ入力データ
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSingleAi( short Id, short AiChannel, long *AiData )
{

	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;
	unsigned long count = 0;

	short AiMaxChannel = 0;
	CONTEC_CPS_AIO_PARAMETER swapData, singleData;

	// NULL Pointer Checks
	if( AiData == (long*)NULL )
		return AIO_ERR_PTR_AI_DATA;

	ulRet = ContecCpsAioGetAiMaxChannels(Id, &AiMaxChannel);

	if( ulRet != AIO_ERR_SUCCESS )
		return ulRet;

	if( AiChannel >= AiMaxChannel ){
		return AIO_ERR_OTHER;// Ai channel error
	}

	ulRet = _contec_cpsaio_singlemulti_getParam(Id, CPS_AIO_INOUT_AI, &swapData);
	
	if( ulRet == AIO_ERR_SUCCESS ){
		singleData.channel = AiChannel; // Set Ai Channel
		singleData.stopTrig = 0; // SetSampling Trigger
		singleData.stopTime = 1; // Set Sampling Number
		singleData.clock = 10.0; // 10 usec

		ulRet = _contec_cpsaio_singlemulti_storeParam(Id, CPS_AIO_INOUT_AI, CONTEC_CPSAIO_LIB_EXCHANGE_SINGLE, singleData);
		//_contec_cpsaio_set_exchange( Id, CPS_AIO_INOUT_AI, CONTEC_CPSAIO_LIB_EXCHANGE_SINGLE );

		//arg.val = AiChannel;
		//ulRet = ioctl( Id, IOCTL_CPSAIO_SETCHANNEL_AI, &arg );

		// SetSampling Trigger
		//ulRet = ContecCpsAioSetAiStopTrigger( Id, CPSAIO_AI_STOPTRG_0 );
		// Set Sampling Number
		//ContecCpsAioSetAiEventSamplingTimes( Id, 1 );
		//ulRet = ContecCpsAioSetAiStopTimes( Id, 1 );
	}

	// Memory Clear 
	if( ulRet == AIO_ERR_SUCCESS ){
		ulRet = ContecCpsAioResetAiMemory( Id );
	}

	if( ulRet == AIO_ERR_SUCCESS ){	
		// Ai Start
		ulRet = ContecCpsAioStartAi( Id );

		// Ver.1.1.1 Infinity Sampling, does not see motion end flag
		// iRet = ContecCpsAioGetAiRepeatTimes(Id, &ulRepeatTime);
	}

	if( ulRet == AIO_ERR_SUCCESS ){	
		count = 0;
		do{
			usleep( 1 );
			iRet = ioctl( Id, IOCTL_CPSAIO_GET_INTERRUPT_FLAG_AI , &arg);
			if( iRet < 0 ){
				ulRet = AIO_ERR_DLL_CALL_DRIVER;
				break;
			}
			if( count >= 1000 ){
				ulRet = AIO_ERR_INTERNAL_TIMEOUT;
				break;
			}
			count ++;
		}while(!( arg.val & CPS_AIO_AI_FLAG_MOTION_END ) );
	}

	if( ulRet == AIO_ERR_SUCCESS ){		
////////////////////// Ver.1.0.6
		if( arg.val & CPS_AIO_AI_FLAG_MOTION_END ) {
			arg.val = CPS_AIO_AI_FLAG_MOTION_END;
			iRet = ioctl( Id, IOCTL_CPSAIO_SET_INTERRUPT_FLAG_AI , &arg);
			if( iRet < 0 )
				ulRet = AIO_ERR_DLL_CALL_DRIVER;		
		}
////////////////////// Ver.1.0.6
	}

	if( ulRet == AIO_ERR_SUCCESS ){	
		// Single Ai の場合、DREフラグをチェックする
		ulRet = _contec_cpsaio_check_memstatus( Id, CPU_AIO_MEMSTATUS_DRE );
	}

	if( ulRet == AIO_ERR_SUCCESS ){		
		iRet = ioctl( Id, IOCTL_CPSAIO_INDATA, &arg );
		if( iRet < 0 )
			return AIO_ERR_DLL_CALL_DRIVER;
		else
			*AiData = (long)( arg.val );
	}

	ContecCpsAioStopAi( Id );

	_contec_cpsaio_singlemulti_storeParam(Id, CPS_AIO_INOUT_AI, CONTEC_CPSAIO_LIB_EXCHANGE_NONE, swapData);

	return ulRet;
}
/**
	@~English
	@brief AIO Library get channel of analog input device sampling one data.( double type )
	@param Id : Device ID
	@param AiChannel : set the Data Channel
	@param AiData : get Data of analog input
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力デバイスの指定したチャネルのサンプリングデータを一回取得。(浮動小数点型)
	@param Id : デバイスID
	@param AiChannel : チャネル番号
	@param AiData : アナログ入力データ
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSingleAiEx( short Id, short AiChannel, double *AiData )
{

	long tmpAiData = 0;
	unsigned short AiResolution = 0;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	double dblMin = 0.0, dblMax = 0.0;

	// NULL Pointer Checks
	if( AiData == (double *)NULL )
		return AIO_ERR_PTR_AI_DATA;

	ulRet = ContecCpsAioSingleAi( Id, AiChannel, &tmpAiData );

	if( ulRet == AIO_ERR_SUCCESS ){
		ulRet = ContecCpsAioGetAiResolution( Id, &AiResolution );
	}

	if( ulRet == AIO_ERR_SUCCESS ){
/*
	ContecCpsAioGetAiRange( Id, &AiRange ); 
	switch( AiRange ){
		case PM10 :
*/
			dblMax = 10.0; dblMin = -10.0;
/*
			break;
	}
*/	
		*AiData =  (double)( tmpAiData / pow(2.0,AiResolution) ) *(dblMax - dblMin) + dblMin;
	}

	return ulRet;
}
/**
	@~English
	@brief AIO Library set channel of analog input repeat times.
	@param Id : Device ID
	@param AiRepeatTimes : Repeat Time Number
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力デバイスのリピート回数を設定する関数。
	@param Id : デバイスID
	@param AiRepeatTimes : リピート回数
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetAiRepeatTimes( short Id, long AiRepeatTimes )
{
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// struct cpsaio_ioctl_arg	arg;

	// arg.inout = CPS_AIO_INOUT_AI;
	// arg.val = AiRepeatTimes;
	// iRet = ioctl( Id, IOCTL_CPSAIO_SETXXXXX, &arg );

	//	if( iRet < 0 )
	//	ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;
}
/**
	@~English
	@brief AIO Library get channel of analog input repeat times.
	@param Id : Device ID
	@param AiRepeatTimes : Repeat Time Number
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力デバイスのリピート回数を取得する関数。
	@param Id : デバイスID
	@param AiRepeatTimes : リピート回数
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiRepeatTimes( short Id, long *AiRepeatTimes ){
	// struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	// int iRet = 0;

	// NULL Pointer Checks
	// if( AiRepeatTimes == ( long * )NULL )	return ;

	// arg.inout = CPS_AIO_INOUT_AI;
	// iRet = ioctl( Id, IOCTL_CPSAIO_GETXXXXX, &arg );
	// if( iRet < 0 )
	//	ulRet = AIO_ERR_DLL_CALL_DRIVER;
	// else
	//	*AiRepeatTimes = arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library get multiple channels of analog input device sampling one data.( unsigned short type )
	@param Id : Device ID
	@param AiChannels : set the Data Channel
	@param AiData : get Data array of analog input
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力デバイスの複数チャネルのサンプリングデータを一回取得。(16bit)
	@param Id : デバイスID
	@param AiChannels : チャネル数
	@param AiData : アナログ入力データ配列
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioMultiAi( short Id, short AiChannels, long AiData[] )
{

	struct cpsaio_ioctl_arg	arg;
	int cnt;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;
	unsigned long count = 0;
	CONTEC_CPS_AIO_PARAMETER swapData, singleData;

	short AiMaxChannel;

	// NULL Pointer Checks
	if( AiData == (long*)NULL )
		return AIO_ERR_PTR_AI_DATA;

	ulRet = ContecCpsAioGetAiMaxChannels(Id, &AiMaxChannel);

	if( ulRet != AIO_ERR_SUCCESS )
		return ulRet;

	if( AiChannels > AiMaxChannel ){
		return AIO_ERR_OTHER;// Ai channel error
	}

	ulRet = _contec_cpsaio_singlemulti_getParam(Id, CPS_AIO_INOUT_AI, &swapData);

	if( ulRet == AIO_ERR_SUCCESS ){	
		singleData.channel = AiChannels; // Set Ai Channel
		singleData.stopTrig = 0; // SetSampling Trigger
		singleData.stopTime = 1; // Set Sampling Number
		singleData.clock = 10.0 * AiChannels; // 10 x AiChannels (usec)

		ulRet = _contec_cpsaio_singlemulti_storeParam(Id, CPS_AIO_INOUT_AI, CONTEC_CPSAIO_LIB_EXCHANGE_MULTI, singleData);


		//ulRet = ContecCpsAioSetAiChannels(Id, AiChannels );

		// SetSampling Trigger
		//ulRet = ContecCpsAioSetAiStopTrigger( Id, CPSAIO_AI_STOPTRG_0 );

		// Set Sampling Number
		//ContecCpsAioSetAiEventSamplingTimes( Id, 1 );
		//ulRet = ContecCpsAioSetAiStopTimes(Id, 1);
	}

	// Memory Clear 
	if( ulRet == AIO_ERR_SUCCESS ){
		ulRet = ContecCpsAioResetAiMemory( Id );
	}

	if( ulRet == AIO_ERR_SUCCESS ){	
		// Ai Start
		ulRet = ContecCpsAioStartAi( Id );
	}

	if( ulRet == AIO_ERR_SUCCESS ){
		// 
		count = 0;
		do{
			usleep( 1 );
			iRet = ioctl( Id, IOCTL_CPSAIO_GET_INTERRUPT_FLAG_AI , &arg);
			if( iRet < 0 ){
				ulRet = AIO_ERR_DLL_CALL_DRIVER;
				break;
			}
			if( count >= 1000 ){
				ulRet = AIO_ERR_INTERNAL_TIMEOUT;
				break;
			}
			count ++;
		}while(!( arg.val & CPS_AIO_AI_FLAG_MOTION_END ) );

	}

	if( ulRet == AIO_ERR_SUCCESS ){
////////////////////// Ver.1.0.6
		if( arg.val & CPS_AIO_AI_FLAG_MOTION_END ) {
			arg.val = CPS_AIO_AI_FLAG_MOTION_END;
			iRet = ioctl( Id, IOCTL_CPSAIO_SET_INTERRUPT_FLAG_AI , &arg);
			if( iRet < 0 )
				ulRet = AIO_ERR_DLL_CALL_DRIVER;			
		}
////////////////////// Ver.1.0.6
	}

	if( ulRet == AIO_ERR_SUCCESS ){	
		// Multi Ai の場合、MDREフラグをチェックする
		ulRet = _contec_cpsaio_check_memstatus( Id, CPU_AIO_MEMSTATUS_MDRE );
	}

	if( ulRet == AIO_ERR_SUCCESS ){		
		for( cnt = 0;cnt < AiChannels; cnt ++ ){
			iRet = ioctl( Id, IOCTL_CPSAIO_INDATA, &arg );
			if( iRet < 0 )
				ulRet = AIO_ERR_DLL_CALL_DRIVER;
			else if( ulRet == AIO_ERR_SUCCESS )			
				AiData[cnt] = (long)( arg.val );
		}
	}

	
	// Ai Stop
	ContecCpsAioStopAi( Id );

	_contec_cpsaio_singlemulti_storeParam(Id, CPS_AIO_INOUT_AI, CONTEC_CPSAIO_LIB_EXCHANGE_NONE, swapData);

	return AIO_ERR_SUCCESS;
}

/**
	@~English
	@brief AIO Library get multiple channels of analog input device sampling one data.( double type )
	@param Id : Device ID
	@param AiChannels : set the Data Channel
	@param AiData : get Data array of analog input
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力デバイスの複数チャネルのサンプリングデータを一回取得。(浮動小数点型)
	@param Id : デバイスID
	@param AiChannels : チャネル数
	@param AiData : アナログ入力データ配列
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioMultiAiEx( short Id, short AiChannels, double AiData[] )
{

	long *tmpAiData;
	unsigned short AiResolution = 0;
	int cnt;
	double dblMin, dblMax;
	unsigned long ulRet = AIO_ERR_SUCCESS;	

	// NULL Pointer Checks
	if( AiData == ( double * )NULL )
		return AIO_ERR_PTR_AI_DATA;

	tmpAiData = (long*)malloc( sizeof(long) * AiChannels );

	if( tmpAiData == (long *) NULL ){
		return AIO_ERR_INI_MEMORY;
	}

	ulRet = ContecCpsAioMultiAi( Id, AiChannels, tmpAiData );

	if( ulRet == AIO_ERR_SUCCESS ){
		ulRet = ContecCpsAioGetAiResolution( Id, &AiResolution );
	}

	if( ulRet == AIO_ERR_SUCCESS ){
/*
	ContecCpsAioGetAiRange( Id, &AiRange ); 
	switch( AiRange ){
		case PM10 :
*/
			dblMax = 10.0; dblMin = -10.0;
/*
			break;
	}
*/	
		for( cnt = 0;cnt < AiChannels; cnt ++){
			AiData[cnt] =  (double)( tmpAiData[cnt] / pow(2.0,AiResolution) ) *(dblMax - dblMin) + dblMin;
		}
	}

	free(tmpAiData);

	return ulRet;
}

//--- Reset Functions ------------------------
/**
	@~English
	@brief AIO Library reset status of analog input.
	@param Id : Device ID
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力デバイスのステータスリセット。
	@param Id : デバイスID
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioResetAiStatus( short Id )
{
	struct cpsaio_ioctl_arg	arg;	
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	arg.inout = CPS_AIO_INOUT_AI;
	iRet = ioctl( Id, IOCTL_CPSAIO_RESET_STATUS, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;
}

/**
	@~English
	@brief AIO Library reset memory of analog input.
	@param Id : Device ID
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力デバイスのメモリリセット。
	@param Id : デバイスID
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioResetAiMemory( short Id )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	arg.inout = CPS_AIO_INOUT_AI;
	iRet = ioctl( Id, IOCTL_CPSAIO_RESET_MEMORY, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;
}

/* Ao Channel */
/**
	@~English
	@brief AIO Library get maximum analog output channels.
	@param Id : Device ID
	@param AoMaxChannels : analog output maximum channel number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスの最大の出力チャネル数を取得します
	@param Id : デバイスID
	@param AoMaxChannels :アナログ出力の最大チャネル数
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAoMaxChannels( short Id, short *AoMaxChannels ){
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( AoMaxChannels == ( short * )NULL )
		return AIO_ERR_PTR_AO_MAX_CHANNELS;

	arg.inout = CPS_AIO_INOUT_AO;
	iRet = ioctl( Id, IOCTL_CPSAIO_GETMAXCHANNEL, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*AoMaxChannels = arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library set analog output channel.
	@param Id : Device ID
	@param AoChannels : analog output channel number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスを計測する出力チャネルを設定します
	@param Id : デバイスID
	@param AoChannels :　アナログ出力のチャネル番号
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetAoChannels( short Id, short AoChannels )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	/* Multi only*/
	_contec_cpsaio_set_exchange( Id, CPS_AIO_INOUT_AO, CONTEC_CPSAIO_LIB_EXCHANGE_MULTI );

	/* Set Channel */
	arg.val = AoChannels - 1;
	iRet = ioctl( Id, IOCTL_CPSAIO_SETCHANNEL_AO, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;	

	return ulRet;
}

/**
	@~English
	@brief AIO Library set analog output sampling number.
	@param Id : Device ID
	@param AoSamplingTimes : analog output sample number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief 出力サンプリング数の設定を行います。
	@param Id : デバイスID
	@param AoSamplingTimes :　出力サンプリング数
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetAoEventSamplingTimes( short Id, unsigned long AoSamplingTimes )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	/* Set Channel */
	arg.val = AoSamplingTimes - 1 ;
	iRet = ioctl( Id, IOCTL_CPSAIO_SET_SAMPNUM_AO, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;

}


//----- Running Functions ------
/**
	@~English
	@brief AIO Library start analog output sampling.
	@param Id : Device ID
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力サンプリングの開始。
	@param Id : デバイスID
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioStartAo( short Id )
{

	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	iRet = ioctl( Id, IOCTL_CPSAIO_START_AO, NULL );
	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
//	do{
//		ioctl( Id, IOCTL_CPSAIO_OUTSTATUS, &arg );;
//	}while( arg.val & 0x10 );

	if( ulRet == AIO_ERR_SUCCESS ){
		//softtrigger only out pulse start
		iRet = ioctl( Id, IOCTL_CPSAIO_SET_OUTPULSE0, 0 );

		if( iRet < 0 )
			ulRet = AIO_ERR_DLL_CALL_DRIVER;	
	}

	return ulRet;

}

/**
	@~English
	@brief AIO Library stop analog output sampling.
	@param Id : Device ID
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力サンプリングの停止。
	@param Id : デバイスID
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioStopAo( short Id )
{
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	iRet = ioctl( Id, IOCTL_CPSAIO_STOP_AO, NULL );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;
}

/**
	@~English
	@brief AIO Library get analog output status.
	@param Id : Device ID
	@param AoStatus : status of analog output
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力デバイスの状態の取得。
	@param Id : デバイスID
	@param AoStatus : アナログ出力のステータス
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAoStatus( short Id, long *AoStatus )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( AoStatus == ( long * )NULL )
		return AIO_ERR_PTR_AO_STATUS;

	iRet = ioctl( Id, IOCTL_CPSAIO_OUTSTATUS, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*AoStatus = (long)arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library set analog output sampling clock.( set time micro second order.)
	@param Id : Device ID
	@param AoSamplingClock : analog output channel number.
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力のサンプリング時間を設定します。単位は usecです。
	@param Id : デバイスID
	@param AoSamplingClock :　サンプリング時間
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetAoSamplingClock( short Id, double AoSamplingClock )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	/* Set Clock */
	arg.val = (unsigned long) (AoSamplingClock * 1000.0 / 25.0) - 1 ;
	iRet = ioctl( Id, IOCTL_CPSAIO_SET_CLOCK_AO, &arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;

}

/**
	@~English
	@brief AIO Library get channel of analog output device sampling one data.( unsigned short type )
	@param Id : Device ID
	@param AoChannel : set the Data Channel
	@param AoData : get Data of analog output
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力デバイスの指定したチャネルのサンプリングデータを一回取得。(16bit)
	@param Id : デバイスID
	@param AoChannel : チャネル番号
	@param AoData : アナログ出力データ
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSingleAo( short Id, short AoChannel, long AoData )
{

	struct cpsaio_ioctl_arg	arg;
	long AoStatus = 0;
	short AoMaxChannel = 0;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	ulRet = ContecCpsAioGetAoMaxChannels(Id, &AoMaxChannel);

	if( ulRet != AIO_ERR_SUCCESS )
		return ulRet;

	if( AoChannel >= AoMaxChannel ){
		return AIO_ERR_PTR_AO_CHANNELS;// chaneel error
	}

	// Exchange Transfer Mode Single Ao
	ulRet = _contec_cpsaio_set_exchange( Id, CPS_AIO_INOUT_AO, CONTEC_CPSAIO_LIB_EXCHANGE_SINGLE );

	if( ulRet == AIO_ERR_SUCCESS ){
		// Set Ao Channel
		arg.val = AoChannel;
		iRet = ioctl( Id, IOCTL_CPSAIO_SETCHANNEL_AO, &arg );

		if( iRet < 0 )
			ulRet = AIO_ERR_DLL_CALL_DRIVER;
	}

	if( ulRet == AIO_ERR_SUCCESS ){
		// Set Sampling Number
		ulRet = ContecCpsAioSetAoEventSamplingTimes( Id, 1 );
	}

	if( ulRet == AIO_ERR_SUCCESS ){
		arg.val = AoData;
		iRet = ioctl( Id, IOCTL_CPSAIO_OUTDATA, &arg );
		if( iRet < 0 )
			ulRet = AIO_ERR_DLL_CALL_DRIVER;
	}

	if( ulRet == AIO_ERR_SUCCESS ){
		// Ao Start
		ulRet = ContecCpsAioStartAo( Id );
	}

	// Ao Stop
	ContecCpsAioStopAo( Id );

	return AIO_ERR_SUCCESS;
}

/**
	@~English
	@brief AIO Library get channel of analog output device sampling one data.( double type )
	@param Id : Device ID
	@param AoChannel : set the Data Channel
	@param AoData : get Data of analog output
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力デバイスの指定したチャネルのサンプリングデータを一回取得。(浮動小数点型)
	@param Id : デバイスID
	@param AoChannel : チャネル番号
	@param AoData : アナログ出力データ
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSingleAoEx( short Id, short AoChannel, double AoData )
{

	long tmpAoData = 0;
	unsigned short AoResolution = 0;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	double dblMin, dblMax;

	ulRet = ContecCpsAioGetAoResolution( Id, &AoResolution );

	if( ulRet == AIO_ERR_SUCCESS ){
/*
	ContecCpsAioGetAoRange( Id, &AoRange ); 
	switch( AoRange ){
		case PM10 :
*/
			dblMax = 20.0; dblMin = 0.0;
/*
			break;
	}
*/

		tmpAoData = (long)( (AoData - dblMin) * pow(2.0, AoResolution) / (dblMax - dblMin) );
	
		ulRet = ContecCpsAioSingleAo( Id, AoChannel, tmpAoData );
	}

	return ulRet;
}

/**
	@~English
	@brief AIO Library get multiple channels of analog input device sampling one data.( unsigned short type )
	@param Id : Device ID
	@param AoChannels : set the Data Channel
	@param AoData : get Data array of analog input
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力デバイスの複数チャネルのサンプリングデータを一回取得。(16bit)
	@param Id : デバイスID
	@param AoChannels : チャネル数
	@param AoData : アナログ出力データ配列
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioMultiAo( short Id, short AoChannels, long AoData[] )
{

	struct cpsaio_ioctl_arg	arg;
	int cnt = 0;
	short AoMaxChannel = 0;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( AoData == ( long * )NULL )
		return AIO_ERR_PTR_AO_DATA;

	ulRet = ContecCpsAioGetAoMaxChannels(Id, &AoMaxChannel);

	if( ulRet != AIO_ERR_SUCCESS )
		return ulRet;

	if( AoChannels > AoMaxChannel )
		return AIO_ERR_OTHER;// channel error

	ulRet = ContecCpsAioSetAoChannels(Id, AoChannels );

	if( ulRet == AIO_ERR_SUCCESS ){
		// Set Sampling Number
		ulRet = ContecCpsAioSetAoEventSamplingTimes( Id, 1 );
	}

	if( ulRet == AIO_ERR_SUCCESS ){	
		for( cnt = 0;cnt < AoChannels; cnt ++ ){
			arg.val = (long)AoData[cnt];
			iRet = ioctl( Id, IOCTL_CPSAIO_OUTDATA, &arg );
			if( iRet < 0 ){
				ulRet = AIO_ERR_DLL_CALL_DRIVER;
				break;
			}
		}
	}

	if( ulRet == AIO_ERR_SUCCESS ){	
		// Ao Start
		ulRet = ContecCpsAioStartAo( Id );
	}

	// Ao Stop
	ContecCpsAioStopAo( Id );

	return AIO_ERR_SUCCESS;
}

/**
	@~English
	@brief AIO Library get multiple channels of analog output device sampling one data.( double type )
	@param Id : Device ID
	@param AoChannels : set the Data Channel
	@param AoData : get Data array of analog output
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力デバイスの複数チャネルのサンプリングデータを一回取得。(浮動小数点型)
	@param Id : デバイスID
	@param AoChannels : チャネル数
	@param AoData : アナログ出力データ配列
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioMultiAoEx( short Id, short AoChannels, double AoData[] )
{

	long *tmpAoData;
	unsigned short AoResolution = 0;
	int cnt = 0;
	double dblMin = 0.0, dblMax = 0.0;
	unsigned long ulRet = AIO_ERR_SUCCESS;

	// NULL Pointer Checks
	if( AoData == ( double * )NULL )
		return AIO_ERR_PTR_AO_DATA;

	tmpAoData = (long*)malloc( sizeof(long) * AoChannels );

	if( tmpAoData == (long *) NULL ){
		return AIO_ERR_INI_MEMORY;
	}

	ulRet = ContecCpsAioGetAoResolution( Id, &AoResolution );

	if( ulRet == AIO_ERR_SUCCESS ){	

/*
	ContecCpsAioGetAoRange( Id, &AoRange ); 
	switch( AoRange ){
		case PM10 :
*/
			dblMax = 20.0; dblMin = 0.0;
/*
			break;
	}
*/	
		for( cnt = 0;cnt < AoChannels; cnt ++){
			tmpAoData[cnt] = (long)( (AoData[cnt] - dblMin) * pow(2.0, AoResolution) / (dblMax - dblMin) );
		}

		ulRet = ContecCpsAioMultiAo( Id, AoChannels, tmpAoData );
	}

	free(tmpAoData);

	return ulRet;
}

/**
	@~English
	@brief AIO Library set destination and source signals by E Control unit.
	@param Id : Device ID
	@param dest : Destination Signal
	@param src : Source Signal
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief ECUに接続元の信号と接続先の信号を設定する関数。
	@param Id : デバイスID
	@param dest : 接続先の信号
	@param src : 接続元の信号
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetEcuSignal( short Id, unsigned short dest, unsigned short src )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	arg.val = CPSAIO_ECU_DESTSRC_SIGNAL(dest, src);
	iRet = ioctl( Id, IOCTL_CPSAIO_SETECU_SIGNAL, &arg);

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return AIO_ERR_SUCCESS;
}

// 2016-01-20 (1) 
/**
	@~English
	@brief AIO Library set Calibration data by analog input.
	@param Id : Device ID
	@param select : select offset or gain
	@param ch : channel
	@param range : Range
	@param data : 16bit data
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力の補正データを設定する関数。
	@param Id : デバイスID
	@param select : オフセットかゲインか
	@param ch : チャネル番号
	@param range : レンジ
	@param data : Data( 16bit )
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetAiCalibrationData( short Id, unsigned char select, unsigned char ch, unsigned char range, unsigned short data )
{
	int aisel = 0;
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	arg.ch = ch;
	arg.val = CPSAIO_AI_CALIBRATION_DATA(select, ch, range, aisel, data);
	ioctl( Id, IOCTL_CPSAIO_SET_CALIBRATION_AI, &arg);

	return AIO_ERR_SUCCESS;

} 
/**
	@~English
	@brief AIO Library get Calibration data by analog input.
	@param Id : Device ID
	@param select : select offset or gain
	@param ch : channel
	@param range : Range
	@param data : 16bit data
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力の補正データを取得する関数。
	@param Id : デバイスID
	@param select : オフセットかゲインか
	@param ch : チャネル番号
	@param range : レンジ
	@param data : Data( 16bit )
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAiCalibrationData( short Id, unsigned char *select, unsigned char *ch, unsigned char *range, unsigned short *data )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( select == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;

	if( ch == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;

	if( range == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;

	if( data == ( unsigned short * )NULL )
		return AIO_ERR_OTHER;

	iRet = ioctl( Id, IOCTL_CPSAIO_GET_CALIBRATION_AI, &arg);

	if( iRet < 0 ){
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	}else{
		*select = CPSAIO_AI_CALIBRATION_GETSELECT( arg.val );
		*range = CPSAIO_AI_CALIBRATION_GETRANGE( arg.val );	
		*data = CPSAIO_AI_CALIBRATION_GETDATA( arg.val );
	}

	return AIO_ERR_SUCCESS;
} 
/**
	@~English
	@brief AIO Library write analog input calibration data to ROM.
	@param Id : Device ID
	@param ch : channel
	@param gain : gain value (8bit)
	@param offset : offset value (8bit)
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力の補正データをROMへ書き込む関数。
	@param Id : デバイスID
	@param ch : チャネル番号
	@param gain : ゲインの値 (  8bit )
	@param offset : オフセットの値(  8bit )
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioWriteAiCalibrationData( short Id, unsigned char ch, unsigned char gain, unsigned char offset )
{
	int aisel = 0;
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	arg.val = ( gain << 8 )  | offset;
	iRet = ioctl( Id, IOCTL_CPSAIO_WRITE_EEPROM_AI, &arg);

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return AIO_ERR_SUCCESS;	

}

/**
	@~English
	@brief AIO Library read analog input calibration data to ROM.
	@param Id : Device ID
	@param ch : channel
	@param gain : gain value (8bit)
	@param offset : offset value (8bit)
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ入力の補正データをROMから読み出す関数。
	@param Id : デバイスID
	@param ch : チャネル番号
	@param gain : ゲインの値 (  8bit )
	@param offset : オフセットの値(  8bit )
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioReadAiCalibrationData( short Id, unsigned char ch, unsigned char *gain, unsigned char *offset )
{
	int aisel = 0;
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( gain == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;

	if( offset == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;


	iRet = ioctl( Id, IOCTL_CPSAIO_READ_EEPROM_AI, &arg);

	if( iRet < 0 ){
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	}else{
		*gain = 	(arg.val & 0xFF00 ) >> 8;

		*offset = (arg.val & 0xFF );
	}

	return ulRet;	

}
/**
	@~English
	@brief AIO Library　clear analog input calibration data from ROM( or RAM).
	@param Id : Device ID
	@param iClear : ROM , RAM Flags
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのROM/RAMからアナログ入力補正データを消去する関数。
	@param Id : デバイスID
	@param iClear : ROM もしくは　RAM
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioClearAiCalibrationData( short Id, int iClear )
{
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	if( iClear & CPSAIO_AI_CALIBRATION_CLEAR_RAM ){
		//FPGA all Clear
		ulRet = ContecCpsAioSetAiCalibrationData( Id, CPSAIO_AI_CALIBRATION_SELECT_OFFSET, 0, CPSAIO_AI_CALIBRATION_RANGE_PM10, 0 );

		if( ulRet == AIO_ERR_SUCCESS )
			ulRet = ContecCpsAioSetAiCalibrationData( Id, CPSAIO_AI_CALIBRATION_SELECT_GAIN, 0, CPSAIO_AI_CALIBRATION_RANGE_PM10, 0 );
	}

	if( ulRet == AIO_ERR_SUCCESS ){
		if( iClear & CPSAIO_AI_CALIBRATION_CLEAR_ROM ){
			//FPGA ROM CLEAR
			iRet = ioctl( Id, IOCTL_CPSAIO_CLEAR_EEPROM, NULL);

			if( iRet < 0 )
				ulRet = AIO_ERR_DLL_CALL_DRIVER;			
		}
	}

	return ulRet;	

}

// 2016-01-20 (1) 
/**
	@~English
	@brief AIO Library set Calibration data by analog output.
	@param Id : Device ID
	@param select : select offset or gain
	@param ch : channel
	@param range : Range
	@param data : 16bit data
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力の補正データを設定する関数。
	@param Id : デバイスID
	@param select : オフセットかゲインか
	@param ch : チャネル番号
	@param range : レンジ
	@param data : Data( 16bit )
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioSetAoCalibrationData( short Id, unsigned char select, unsigned char ch, unsigned char range, unsigned short data )
{
	int aisel = 0;
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	arg.val = CPSAIO_AO_CALIBRATION_DATA(select, ch, range, aisel, data);
	iRet = ioctl( Id, IOCTL_CPSAIO_SET_CALIBRATION_AO, &arg);

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;

} 

/**
	@~English
	@brief AIO Library get Calibration data by analog output.
	@param Id : Device ID
	@param select : select offset or gain
	@param ch : channel
	@param range : Range
	@param data : 16bit data
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力の補正データを取得する関数。
	@param Id : デバイスID
	@param select : オフセットかゲインか
	@param ch : チャネル番号
	@param range : レンジ
	@param data : Data( 16bit )
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioGetAoCalibrationData( short Id, unsigned char *select, unsigned char *ch, unsigned char *range, unsigned short *data )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( select == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;

	if( ch == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;

	if( range == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;

	if( data == ( unsigned short * )NULL )
		return AIO_ERR_OTHER;

	iRet = ioctl( Id, IOCTL_CPSAIO_GET_CALIBRATION_AO, &arg);

	if( iRet < 0 ){
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	}else{
		*select = CPSAIO_AO_CALIBRATION_GETSELECT( arg.val );
		*range = CPSAIO_AO_CALIBRATION_GETRANGE( arg.val );	
		*data = CPSAIO_AO_CALIBRATION_GETDATA( arg.val );
	}

	return ulRet;
} 

/**
	@~English
	@brief AIO Library write Calibration analog output data to ROM.
	@param Id : Device ID
	@param ch : channel
	@param gain : gain value (8bit)
	@param offset : offset value (8bit)
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力の補正データをROMへ書き込む関数。
	@param Id : デバイスID
	@param ch : チャネル番号
	@param gain : ゲインの値 (  8bit )
	@param offset : オフセットの値(  8bit )
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioWriteAoCalibrationData( short Id, unsigned char ch, unsigned char gain, unsigned char offset )
{
	int aisel = 0;
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	arg.ch = ch;
	arg.val = ( gain << 8 )  | offset;
	iRet = ioctl( Id, IOCTL_CPSAIO_WRITE_EEPROM_AO, &arg);

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;	

}
/**
	@~English
	@brief AIO Library read analog output calibration data to ROM.
	@param Id : Device ID
	@param ch : channel
	@param gain : gain value (8bit)
	@param offset : offset value (8bit)
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief アナログ出力の補正データをROMから読み出す関数。
	@param Id : デバイスID
	@param ch : チャネル番号
	@param gain : ゲインの値 (  8bit )
	@param offset : オフセットの値(  8bit )
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioReadAoCalibrationData( short Id, unsigned char ch, unsigned char *gain, unsigned char *offset )
{
	int aisel = 0;
	struct cpsaio_ioctl_arg	arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( gain == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;

	if( offset == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;

	arg.ch = ch;

	iRet = ioctl( Id, IOCTL_CPSAIO_READ_EEPROM_AO, &arg);

	if( iRet < 0 ){
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	}else{

		*gain = 	(arg.val & 0xFF00 ) >> 8;

		*offset = (arg.val & 0xFF );
	}

	return ulRet;	

}

/**
	@~English
	@brief AIO Library　clear analog output calibration data from ROM( or RAM).
	@param Id : Device ID
	@param iClear : ROM , RAM Flags
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのROM/RAMからアナログ出力補正データを消去する関数。
	@param Id : デバイスID
	@param iClear : ROM もしくは　RAM
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioClearAoCalibrationData( short Id, int iClear )
{
	int cnt;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	if( iClear & CPSAIO_AO_CALIBRATION_CLEAR_RAM ){
		//FPGA all Clear
		for( cnt = 0;cnt < 4 ;cnt ++ ){
			ulRet = ContecCpsAioSetAoCalibrationData( Id, CPSAIO_AO_CALIBRATION_SELECT_OFFSET, cnt, CPSAIO_AO_CALIBRATION_RANGE_P20MA, 0 );
			if( ulRet == AIO_ERR_SUCCESS )
				ulRet = ContecCpsAioSetAoCalibrationData( Id, CPSAIO_AO_CALIBRATION_SELECT_GAIN, cnt, CPSAIO_AO_CALIBRATION_RANGE_P20MA, 0 );
		}
	}
	if( ulRet == AIO_ERR_SUCCESS ){
		if( iClear & CPSAIO_AO_CALIBRATION_CLEAR_ROM ){
			//FPGA ROM CLEAR
			iRet = ioctl( Id, IOCTL_CPSAIO_CLEAR_EEPROM, NULL);

			if( iRet < 0 )
				ulRet = AIO_ERR_DLL_CALL_DRIVER;	
		}
	}
	return ulRet;	

}

/* Direct Input / Output (Debug) */

/**
	@~English
	@brief AIO Library the register of address read data.(1byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのアドレスレジスタを読み出す関数。(1byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioInp( short Id, unsigned long addr, unsigned char *value )
{
	struct cpsaio_direct_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( value == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;

	memset(&arg, 0, sizeof(struct cpsaio_direct_arg));

	arg.addr = addr;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_INPUT, arg );
	
	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*value = (unsigned char)arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of address read data.(2byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのアドレスレジスタを読み出す関数。(2byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioInpW( short Id, unsigned long addr, unsigned short *value )
{
	struct cpsaio_direct_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( value == ( unsigned short * )NULL )
		return AIO_ERR_OTHER;

	memset(&arg, 0, sizeof(struct cpsaio_direct_arg));

	arg.addr = addr;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_INPUT, arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*value = (unsigned short)arg.val;
	
	return ulRet;

}

/**
	@~English
	@brief AIO Library the register of address read data.(4byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのアドレスレジスタを読み出す関数。(4byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioInpD( short Id, unsigned long addr, unsigned long *value )
{
	struct cpsaio_direct_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( value == ( unsigned long * )NULL )
		return AIO_ERR_OTHER;

	memset(&arg, 0, sizeof(struct cpsaio_direct_arg));	

	arg.addr = addr;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_INPUT, arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*value = (unsigned long)arg.val;
	
	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of address write data.(1byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのアドレスレジスタへ書き出す関数。(1byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioOutp( short Id, unsigned long addr, unsigned char value )
{
	struct cpsaio_direct_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	memset(&arg, 0, sizeof(struct cpsaio_direct_arg));

	arg.addr = addr;
	arg.val = (unsigned long)value;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_OUTPUT, arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of address write data.(2byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのアドレスレジスタへ書き出す関数。(2byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioOutpW( short Id, unsigned long addr, unsigned short value )
{
	struct cpsaio_direct_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	memset(&arg, 0, sizeof(struct cpsaio_direct_arg));

	arg.addr = addr;
	arg.val = (unsigned long)value;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_OUTPUT, arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;

}

/**
	@~English
	@brief AIO Library the register of address write data.(4byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのアドレスレジスタへ書き出す関数。(4byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioOutpD( short Id, unsigned long addr, unsigned long value )
{
	struct cpsaio_direct_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	memset(&arg, 0, sizeof(struct cpsaio_direct_arg));

	arg.addr = addr;
	arg.val = (unsigned long)value;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_OUTPUT, arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;

}

/**
	@~English
	@brief AIO Library the register of ecu address read data.(1byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのECUアドレスレジスタを読み出す関数。(1byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioEcuInp( short Id, unsigned long addr, unsigned char *value )
{
	struct cpsaio_direct_command_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( value == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;

	memset(&arg, 0, sizeof(struct cpsaio_direct_command_arg));

	arg.addr = addr;
	arg.isEcu = 1;
	arg.size = 1;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_COMMAND_INPUT, arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*value = (unsigned char)arg.val;

	return ulRet;

}

/**
	@~English
	@brief AIO Library the register of ecu address read data.(2byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのECUアドレスレジスタを読み出す関数。(2byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioEcuInpW( short Id, unsigned long addr, unsigned short *value )
{

	struct cpsaio_direct_command_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( value == ( unsigned short * )NULL )
		return AIO_ERR_OTHER;

	memset(&arg, 0, sizeof(struct cpsaio_direct_command_arg));

	arg.addr = addr;
	arg.isEcu = 1;
	arg.size = 2;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_COMMAND_INPUT, arg );
	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*value = (unsigned short)arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of ecu address read data.(4byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのECUアドレスレジスタを読み出す関数。(4byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioEcuInpD( short Id, unsigned long addr, unsigned long *value )
{

	struct cpsaio_direct_command_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( value == ( unsigned long * )NULL )
		return AIO_ERR_OTHER;

	memset(&arg, 0, sizeof(struct cpsaio_direct_command_arg));

	arg.addr = addr;
	arg.isEcu = 1;
	arg.size = 4;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_COMMAND_INPUT, arg );
	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*value = (unsigned long)arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of ecu address write data.(1byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのECUアドレスレジスタへ書き出す関数。(1byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioEcuOutp( short Id, unsigned long addr, unsigned char value )
{
	struct cpsaio_direct_command_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	memset(&arg, 0, sizeof(struct cpsaio_direct_command_arg));

	arg.addr = addr;
	arg.isEcu = 1;
	arg.size = 1;
	arg.val = value;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_COMMAND_OUTPUT, arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of ecu address write data.(2byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのECUアドレスレジスタへ書き出す関数。(2byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioEcuOutpW( short Id, unsigned long addr, unsigned short value )
{
	struct cpsaio_direct_command_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	memset(&arg, 0, sizeof(struct cpsaio_direct_command_arg));

	arg.addr = addr;
	arg.isEcu = 1;
	arg.size = 2;
	arg.val = (unsigned long)value;

	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_COMMAND_OUTPUT, arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of ecu address write data.(4byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのECUアドレスレジスタへ書き出す関数。(4byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioEcuOutpD( short Id, unsigned long addr, unsigned long value )
{

	struct cpsaio_direct_command_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	memset(&arg, 0, sizeof(struct cpsaio_direct_command_arg));

	arg.addr = addr;
	arg.isEcu = 1;
	arg.size = 4;
	arg.val = value;

	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_COMMAND_OUTPUT, arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of command address read data.(1byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのCOMMANDアドレスレジスタを読み出す関数。(1byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioCommandInp( short Id, unsigned long addr, unsigned char *value )
{

	struct cpsaio_direct_command_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( value == ( unsigned char * )NULL )
		return AIO_ERR_OTHER;

	memset(&arg, 0, sizeof(struct cpsaio_direct_command_arg));

	arg.addr = addr;
	arg.isEcu = 0;
	arg.size = 1;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_COMMAND_INPUT, arg );
	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*value = (unsigned char)arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of command address read data.(2byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのCOMMANDアドレスレジスタを読み出す関数。(2byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioCommandInpW( short Id, unsigned long addr, unsigned short *value )
{

	struct cpsaio_direct_command_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( value == ( unsigned short * )NULL )
		return AIO_ERR_OTHER;

	memset(&arg, 0, sizeof(struct cpsaio_direct_command_arg));

	arg.addr = addr;
	arg.isEcu = 0;
	arg.size = 2;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_COMMAND_INPUT, arg );
	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*value = (unsigned short)arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of command address read data.(4byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのCOMMANDアドレスレジスタを読み出す関数。(4byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioCommandInpD( short Id, unsigned long addr, unsigned long *value )
{

	struct cpsaio_direct_command_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	// NULL Pointer Checks
	if( value == ( unsigned long * )NULL )
		return AIO_ERR_OTHER;

	memset(&arg, 0, sizeof(struct cpsaio_direct_command_arg));

	arg.addr = addr;
	arg.isEcu = 0;
	arg.size = 4;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_COMMAND_INPUT, arg );
	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	else
		*value = (unsigned long)arg.val;

	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of command address write data.(1byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのCOMMANDアドレスレジスタへ書き出す関数。(1byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioCommandOutp( short Id, unsigned long addr, unsigned char value )
{
	struct cpsaio_direct_command_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	memset(&arg, 0, sizeof(struct cpsaio_direct_command_arg));

	arg.addr = addr;
	arg.isEcu = 0;
	arg.size = 1;
	arg.val = (unsigned long)value;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_COMMAND_OUTPUT, arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of command address write data.(2byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのCOMMANDアドレスレジスタへ書き出す関数。(2byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioCommandOutpW( short Id, unsigned long addr, unsigned short value )
{
	struct cpsaio_direct_command_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	memset(&arg, 0, sizeof(struct cpsaio_direct_command_arg));

	arg.addr = addr;
	arg.isEcu = 0;
	arg.size = 2;
	arg.val = (unsigned long)value;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_COMMAND_OUTPUT, arg );
	
	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;
	
	return ulRet;
}

/**
	@~English
	@brief AIO Library the register of command address write data.(4byte)
	@param Id : Device ID
	@param addr : Address
	@param value : Values
	@return Success: AIO_ERR_SUCCESS
	@~Japanese
	@brief デバイスのCOMMANDアドレスレジスタへ書き出す関数。(4byte)
	@param Id : デバイスID
	@param addr : アドレス
	@param value : 値
	@return 成功: AIO_ERR_SUCCESS
**/
unsigned long ContecCpsAioCommandOutpD( short Id, unsigned long addr, unsigned long value )
{
	struct cpsaio_direct_command_arg arg;
	unsigned long ulRet = AIO_ERR_SUCCESS;
	int iRet = 0;

	memset(&arg, 0, sizeof(struct cpsaio_direct_command_arg));

	arg.addr = addr;
	arg.isEcu = 0;
	arg.size = 4;
	arg.val = value;
	iRet = ioctl( Id, IOCTL_CPSAIO_DIRECT_COMMAND_OUTPUT, arg );

	if( iRet < 0 )
		ulRet = AIO_ERR_DLL_CALL_DRIVER;

	return ulRet;
}

