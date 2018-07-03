#ifndef __LIB_CPS_AIO__
#define __LIB_CPS_AIO__
#include "cps_def.h"

/**
 @~English
 @name ERROR CODE (define)
 @~Japanese
 @name エラーコード
**/
/// @{

/**
 @name INI Group
**/
/// @{
#define AIO_ERR_SUCCESS		0		///< 正常終了
#define AIO_ERR_INI_RESOURCE	1
#define AIO_ERR_INI_INTERRUPT	2
#define AIO_ERR_INI_MEMORY	3 ///< メモリの割り当てに失敗しました
#define AIO_ERR_INI_REGISTRY	4	///< 設定ファイルのアクセスに失敗しました
#define AIO_ERR_INI_BOARD_ID	5
/// @}

/**
 @name DLL Group
**/
/// @{
#define AIO_ERR_DLL_DEVICE_NAME		10000	///< 設定ユーティリティに登録されていないデバイス名が指定されました
#define AIO_ERR_DLL_INVALID_ID		10001	///< 無効なIDが指定されました
#define AIO_ERR_DLL_CALL_DRIVER		10002
#define AIO_ERR_DLL_CREATE_FILE		10003	///< ハンドルの取得に失敗しました
#define AIO_ERR_DLL_CLOSE_FILE		10004	///< ハンドルのクローズに失敗しました
#define AIO_ERR_DLL_CREATE_THREAD	10005
#define	AIO_ERR_PTR_DEVICE_NAME			10007		///< DeviceNameのポインタがNULLです
#define	AIO_ERR_PTR_DEVICE				10008		///< DeviceのポインタがNULLです
#define	AIO_ERR_UNKNOWN_DEVICE			10013		///< デバイスの種類を判別できません
#define	AIO_ERR_PTR_DEVICE_TYPE			10015		///< DeviceTypeのポインタがNULLです

#define AIO_ERR_INFO_INVALID_DEVICE	10050	///< 指定したデバイス名称が見つかりません。スペルを確認してください。
#define AIO_ERR_INFO_NOT_FIND_DEVICE	10051 ///< 利用可能なデバイスが見つかりません
#define AIO_ERR_INFO_INVALID_INFOTYPE	10052

#define	AIO_ERR_NULL_DEVICE				10100		///< DeviceNameにデバイス名が格納されていません
#define	AIO_ERR_PTR_ID					10101		///< IdのポインタがNULLです
#define	AIO_ERR_PTR_ERR_STR				10180		///< ErrorStringのポインタがNULLです
#define	AIO_ERR_PTR_AI_RANGE			11051		///< AiRangeがNULLです
#define	AIO_ERR_AI_RANGE				11060		///< AiRangeの値が関数の指定範囲外です
#define	AIO_ERR_AI_CHANNEL				11420		///< AiChannelの値が関数の指定範囲外です
#define	AIO_ERR_PTR_AI_DATA				11421		///< AiDataのポインタがNULLです
#define AIO_ERR_PTR_AI_SAMPLINGTIMES	11580
#define	AIO_ERR_PTR_AI_MAX_CHANNELS		11720		///< AiMaxChannelsのポインタがNULLです
#define	AIO_ERR_PTR_AI_RESOLUTION		11740		///< AiResolutionのポインタがNULLです
#define	AIO_ERR_AO_CHANNEL				13000		///< AoChannelの値が関数の指定範囲外です
#define	AIO_ERR_AO_CHANNELS				13020		///< AoChannelsの値が関数の指定範囲外です
#define	AIO_ERR_PTR_AO_DATA				13021		///< AoDataのポインタがNULLです
#define	AIO_ERR_PTR_AO_RESOLUTION		13040		///< AoResolutionのポインタがNULLです
#define	AIO_ERR_PTR_AO_MAX_CHANNELS		13080		///< AoMaxChannelsのポインタがNULLです
#define	AIO_ERR_PTR_AO_RANGE			13111		///< AoRangeのポインタがNULLです
#define	AIO_ERR_AO_RANGE				13120		///< AoRangeの値が関数の指定範囲外です

#define AIO_ERR_PTR_AI_CHANNELS	19000 ///< AI Channel Null Pointer
#define AIO_ERR_PTR_AI_SAMPLING_CLOCK	19001 ///< AI Channel Null Pointer
 #define AIO_ERR_PTR_AO_STATUS	19002	///<
#define AIO_ERR_PTR_AI_STATUS	19003	///<
#define AIO_ERR_PTR_AI_SAMPLING_COUNT	19004	///<
/// @}

/**
 @name SYS Group
**/
/// @{

#define AIO_ERR_SYS_MEMORY										20000
#define AIO_ERR_SYS_NOT_SUPPORTED					20001	///< 使用しているデバイスではこの関数を使用することができません
#define AIO_ERR_SYS_BOARD_EXECUTING				20002
#define AIO_ERR_SYS_USING_OTHER_PROCESS	20003
#define	AIO_ERR_AI_DEVICE_RANGE			21060		///< AiRangeの値が使用しているデバイスの指定範囲外です
#define	AIO_ERR_AI_SET_RANGE			21063		///< レンジの設定に失敗しました
#define	AIO_ERR_AI_DEVICE_CHANNEL		21420		///< AiChannelの値が使用しているデバイスの範囲外です
#define	AIO_ERR_AD_ERROR				21421		///< AD変換エラーが発生しました
#define	AIO_ERR_AI_CHANNEL_NUM_OVER		21441		///< 変換チャネル数をオーバーしました、最大チャネルで変換します
#define AIO_ERR_DLL_AI_OVER_SAMLINGTIMES	21580
#define AIO_ERR_DLL_AI_FIFO_EMPTY	21584
#define	AIO_ERR_AO_DEVICE_CHANNEL		23000		///< AoChannelの値が使用しているデバイスの範囲外です
#define	AIO_ERR_DA_ERROR				23001		///< A変換エラーが発生しました
#define	AIO_ERR_AO_DATA_RANGE1			23002		///< AoDataに設定するデータはは0～4095の範囲で指定してください
#define	AIO_ERR_AO_DATA_RANGE2			23003		///< AoDataに設定するデータはは0～65535の範囲で指定してください
#define	AIO_ERR_AO_CHANNEL_NUM_OVER		23021		///< 変換チャネル数をオーバーしました。最大チャネルで変換します
#define	AIO_ERR_AO_DEVICE_RANGE			23120		///< AoRangeの値が使用しているデバイスの指定範囲外です
#define	AIO_ERR_AO_SET_RANGE			23121		///< レンジの設定に失敗しました
#define	AIO_ERR_START_ERROR				28001		///< モジュール起動時にエラーが発生しました。
#define	AIO_ERR_NOT_START				28002		///< デバイスはまだ起動していません。
#define	AIO_ERR_ACCESS_RIGHT			28003		///< アクセス権エラーです。
#define	AIO_ERR_DLL_TIMEOUT				28004		///< 通信タイムアウトが発生しました。
#define	AIO_ERR_COMPOSITION				28005		///< 機器構成エラーです。グループID=8以外の設定ではIOアシストサーバーが必要です。


#define AIO_ERR_OTHER		99900		///< マジックエラーナンバー

/// @}

#define AIOM_INTERRUPT	0x1300

/* Event Controller dest signal */
#define AIOECU_DEST_AI_CLK	4
#define AIOECU_DEST_AI_START	0
#define AIOECU_DEST_AI_STOP	2
#define AIOECU_DEST_AO_CLK	36
#define AIOECU_DEST_AO_START	32
#define AIOECU_DEST_AO_STOP	34

/* Event Controller src signal */
#define AIOECU_SRC_NON_CONNECT	0
#define AIOECU_SRC_AI_CLK	4
#define AIOECU_SRC_AI_STOP	17
#define AIOECU_SRC_AO_CLK	66
#define AIOECU_SRC_AO_STOP_RING	80
#define AIOECU_SRC_AO_STOP_FIFO	352
#define AIOECU_SRC_START	384

#define CPSAIO_ECU_DESTSRC_SIGNAL(dest, src)	( (src << 16) | dest )

/* Ai Calibration */
#define CPSAIO_AI_CALIBRATION_SELECT_OFFSET	0
#define CPSAIO_AI_CALIBRATION_SELECT_GAIN	1

#define CPSAIO_AI_CALIBRATION_RANGE_PM10	0
#define CPSAIO_AI_CALIBRATION_RANGE_P20MA	7

#define CPSAIO_AI_CALIBRATION_DATA( sel, channel, range, aisel, data ) \
	( ( (data & 0xFF) << 16 ) | ( (aisel & 0x01 ) << 4 ) | ( (range & 0x07) << 1 ) | ( sel & 0x01 ) )
#define CPSAIO_AI_CALIBRATION_GETSELECT( val )	(val & 0x00000001 )
#define CPSAIO_AI_CALIBRATION_GETRANGE( val )		((val & 0x0000000E ) >> 1)
#define CPSAIO_AI_CALIBRATION_GETAISEL( val )		((val & 0x00000010 ) >> 4)
#define CPSAIO_AI_CALIBRATION_GETDATA( val )		((val & 0x00FF0000 ) >> 16)

/* Ao Calibration */
#define CPSAIO_AO_CALIBRATION_SELECT_OFFSET	0
#define CPSAIO_AO_CALIBRATION_SELECT_GAIN	1

#define CPSAIO_AO_CALIBRATION_RANGE_PM10	0
#define CPSAIO_AO_CALIBRATION_RANGE_P20MA	7

#define CPSAIO_AO_CALIBRATION_DATA( sel, channel, range, aosel, data ) \
	( ( (data & 0xFF) << 16 ) | ( (channel & 0x03 ) << 4 ) | ( (range & 0x07) << 1 ) | ( sel & 0x01 ) )
#define CPSAIO_AO_CALIBRATION_GETSELECT( val )	(val & 0x00000001 )
#define CPSAIO_AO_CALIBRATION_GETRANGE( val )		((val & 0x0000000E ) >> 1)
#define CPSAIO_AO_CALIBRATION_GETAISEL( val )		((val & 0x00000010 ) >> 4)
#define CPSAIO_AO_CALIBRATION_GETDATA( val )		((val & 0x00FF0000 ) >> 16)

/* CLEAR Calibration */
#define CPSAIO_AI_CALIBRATION_CLEAR_RAM		0x01
#define CPSAIO_AI_CALIBRATION_CLEAR_ROM		0x02

#define CPSAIO_AI_CALIBRATION_CLEAR_ALL		0x03

#define CPSAIO_AO_CALIBRATION_CLEAR_RAM		0x01
#define CPSAIO_AO_CALIBRATION_CLEAR_ROM		0x02

#define CPSAIO_AO_CALIBRATION_CLEAR_ALL		0x03

#define AIS_BUSY	( 0x00000001 )
#define AIS_START_TRG	( 0x00000002 )
#define AIS_DATA_NUM	( 0x00000010 )
#define AIS_OFERR	( 0x00010000 )
#define AIS_SCERR	( 0x00020000 )
#define AIS_AIERR	( 0x00040000 )

#define AOS_BUSY	( 0x00000001 )
#define AOS_START_TRG	( 0x00000002 )
#define AOS_DATA_NUM	( 0x00000010 )
#define AOS_SCERR	( 0x00020000 )
#define AOS_AIERR	( 0x00040000 )

/****  Structure ****/
typedef struct __contec_cps_aio_int_callback_data__
{
	short id;
	short Message;
	short	wParam;
	long		lParam;
	void*	Param;

}CONTEC_CPS_AIO_INT_CALLBACK_DATA, *PCONTEC_CPS_AIO_INT_CALLBACK_DATA;

typedef void (*PCONTEC_CPS_AIO_INT_CALLBACK)(short, short, long, long, void *);

/**** Common Functions ****/
extern unsigned long ContecCpsAioInit( char *DeviceName, short *Id );
extern unsigned long ContecCpsAioExit( short Id );
extern unsigned long ContecCpsAioGetErrorStrings( unsigned long code, char *Str );
extern unsigned long ContecCpsAioQueryDeviceName( short Index, char *DeviceName, char *Device );
extern unsigned long ContecCpsAioGetAiResolution( short Id, unsigned short *AiResolution );
extern unsigned long ContecCpsAioGetAoResolution( short Id, unsigned short *AoResolution );
extern unsigned long ContecCpsAioGetVersion( short Id , unsigned char libVer[] , unsigned char drvVer[] );

/**** Analog Input Functions ****/
extern unsigned long ContecCpsAioSetAiChannels( short Id, short AiChannels );
extern unsigned long ContecCpsAioGetAiChannels( short Id, short *AiChannels );
extern unsigned long ContecCpsAioStartAi( short Id );
extern unsigned long ContecCpsAioStopAi( short Id );
extern unsigned long ContecCpsAioGetAiSamplingCount( short Id, long *AiSamplingCount );
extern unsigned long ContecCpsAioGetAiSamplingData( short Id, long *AiSamplingTimes, long AiData[] );
extern unsigned long ContecCpsAioGetAiSamplingDataEx( short Id, long *AiSamplingTimes, double AiData[] );
extern unsigned long ContecCpsAioGetAiStatus( short Id, long *AiStatus );
extern unsigned long ContecCpsAioSingleAi( short Id, short AiChannel, long *AiData );
extern unsigned long ContecCpsAioSingleAiEx( short Id, short AiChannel, double *AiData );
extern unsigned long ContecCpsAioMultiAi( short Id, short AiChannels, long AiData[] );
extern unsigned long ContecCpsAioMultiAiEx( short Id, short AiChannels, double AiData[] );
extern unsigned long ContecCpsAioSetAiSamplingClock( short Id, double AiSamplingClock );
extern unsigned long ContecCpsAioGetAiSamplingClock( short Id, double *AiSamplingClock );
extern unsigned long ContecCpsAioSetAiRepeatTimes( short Id, long AiRepeatTimes );
extern unsigned long ContecCpsAioGetAiRepeatTimes( short Id, long *AiRepeatTimes );
extern unsigned long ContecCpsAioSetAiEventSamplingTimes( short Id, unsigned long AiSamplingTimes );
extern unsigned long ContecCpsAioGetAiEventSamplingTimes( short Id, unsigned long *AiSamplingTimes );
extern unsigned long ContecCpsAioSetAiCalibrationData( short Id, unsigned char select, unsigned char ch, unsigned char range, unsigned short data );
extern unsigned long ContecCpsAioGetAiCalibrationData( short Id, unsigned char *select, unsigned char *ch, unsigned char *range, unsigned short *data );

extern unsigned long ContecCpsAioWriteAiCalibrationData( short Id, unsigned char ch, unsigned char gain, unsigned char offset );
extern unsigned long ContecCpsAioReadAiCalibrationData( short Id, unsigned char ch, unsigned char *gain, unsigned char *offset );
extern unsigned long ContecCpsAioClearAiCalibrationData( short Id, int iClear );

/**** Analog Output Functions ****/
extern unsigned long ContecCpsAioSetAoChannels( short Id, short AoChannels );
extern unsigned long ContecCpsAioStartAo( short Id );
extern unsigned long ContecCpsAioStopAo( short Id );
extern unsigned long ContecCpsAioGetAoStatus( short Id, long *AoStatus );
extern unsigned long ContecCpsAioSingleAo( short Id, short AoChannel, long AoData );
extern unsigned long ContecCpsAioSingleAoEx( short Id, short AoChannel, double AoData );
extern unsigned long ContecCpsAioMultiAo( short Id, short AoChannels, long AoData[] );
extern unsigned long ContecCpsAioMultiAoEx( short Id, short AoChannels, double AoData[] );
extern unsigned long ContecCpsAioSetAoSamplingClock( short Id, double AoSamplingClock );
extern unsigned long ContecCpsAioSetAoEventSamplingTimes( short Id, unsigned long AoSamplingTimes );
extern unsigned long ContecCpsAioSetAoCalibrationData( short Id, unsigned char select, unsigned char ch, unsigned char range, unsigned short data );
extern unsigned long ContecCpsAioGetAoCalibrationData( short Id, unsigned char *select, unsigned char *ch, unsigned char *range, unsigned short *data );

extern unsigned long ContecCpsAioWriteAoCalibrationData( short Id, unsigned char ch, unsigned char gain, unsigned char offset );
extern unsigned long ContecCpsAioReadAoCalibrationData( short Id, unsigned char ch, unsigned char *gain, unsigned char *offset );
extern unsigned long ContecCpsAioClearAoCalibrationData( short Id, int iClear );

/**** Event Controller Functions ****/
extern unsigned long ContecCpsAioSetEcuSignal( short Id, unsigned short dest, unsigned short src );

// Direct Input / Output Functions(Debug)
extern unsigned long ContecCpsAioInp( short Id, unsigned long addr, unsigned char *value );
extern unsigned long ContecCpsAioInpW( short Id, unsigned long addr, unsigned short *value );
extern unsigned long ContecCpsAioInpD( short Id, unsigned long addr, unsigned long *value );

extern unsigned long ContecCpsAioOutp( short Id, unsigned long addr, unsigned char value );
extern unsigned long ContecCpsAioOutpW( short Id, unsigned long addr, unsigned short value );
extern unsigned long ContecCpsAioOutpD( short Id, unsigned long addr, unsigned long value );

extern unsigned long ContecCpsAioEcuInp( short Id, unsigned long addr, unsigned char *value );
extern unsigned long ContecCpsAioEcuInpW( short Id, unsigned long addr, unsigned short *value );
extern unsigned long ContecCpsAioEcuInpD( short Id, unsigned long addr, unsigned long *value );

extern unsigned long ContecCpsAioEcuOutp( short Id, unsigned long addr, unsigned char value );
extern unsigned long ContecCpsAioEcuOutpW( short Id, unsigned long addr, unsigned short value );
extern unsigned long ContecCpsAioEcuOutpD( short Id, unsigned long addr, unsigned long value );

extern unsigned long ContecCpsAioCommandInp( short Id, unsigned long addr, unsigned char *value );
extern unsigned long ContecCpsAioCommandInpW( short Id, unsigned long addr, unsigned short *value );
extern unsigned long ContecCpsAioCommandInpD( short Id, unsigned long addr, unsigned long *value );

extern unsigned long ContecCpsAioCommandOutp( short Id, unsigned long addr, unsigned char value );
extern unsigned long ContecCpsAioCommandOutpW( short Id, unsigned long addr, unsigned short value );
extern unsigned long ContecCpsAioCommandOutpD( short Id, unsigned long addr, unsigned long value );


#endif
