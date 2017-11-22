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
*
* Change Log:
*  2016-01-20 : <Ver.1.0.1> 
                (1) Add ContecCpsAioSetAiCalibrationData function.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h> //Ver.1.0.2
#include <malloc.h> // Ver.1.0.2
#include "cpsaio.h"
#include "../include/libcpsaio.h"

typedef struct __contec_cps_aio_int_callback__
{
	PCONTEC_CPS_AIO_INT_CALLBACK func;
	CONTEC_CPS_AIO_INT_CALLBACK_DATA data;
}CONTEC_CPS_AIO_INT_CALLBACK_LIST, *PCONTEC_CPS_AIO_INT_CALLBACK_LIST;

CONTEC_CPS_AIO_INT_CALLBACK_LIST contec_cps_aio_cb_list[CPS_DEVICE_MAX_NUM];

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

unsigned long _contec_cpsaio_set_exchange( short Id, unsigned char isOutput, unsigned char isMulti )
{
	struct cpsaio_ioctl_arg	arg;

	/* Single or Multi */
	if( isMulti )	arg.val = 1;
	else	arg.val = 0;

	/* Ai or Ao */
	if( isOutput )
		ioctl( Id, IOCTL_CPSAIO_SETTRANSFER_MODE_AO, &arg );
	else
		ioctl( Id, IOCTL_CPSAIO_SETTRANSFER_MODE_AI, &arg );

	return AIO_ERR_SUCCESS;

}

unsigned long ContecCpsAioInit( char *DeviceName, short *Id )
{
	// open
	char Name[32];
	struct cpsaio_ioctl_arg	arg;
	unsigned char g = 0, o = 0;
	strcpy(Name, "/dev/");
	strcat(Name, DeviceName);

	*Id = open( Name, O_RDWR );

	if( *Id <= 0 ) return AIO_ERR_DLL_CREATE_FILE;

	ioctl( *Id, IOCTL_CPSAIO_INIT, &arg);

	ContecCpsAioSetEcuSignal(*Id, AIOECU_DEST_AI_CLK, AIOECU_SRC_AI_CLK );
	ContecCpsAioSetEcuSignal(*Id, AIOECU_DEST_AI_START, AIOECU_SRC_START );
	ContecCpsAioSetEcuSignal(*Id, AIOECU_DEST_AI_STOP, AIOECU_SRC_AI_STOP );

	ContecCpsAioSetEcuSignal(*Id, AIOECU_DEST_AO_CLK, AIOECU_SRC_AO_CLK );
	ContecCpsAioSetEcuSignal(*Id, AIOECU_DEST_AO_START, AIOECU_SRC_START );
	ContecCpsAioSetEcuSignal(*Id, AIOECU_DEST_AO_STOP, AIOECU_SRC_AO_STOP_RING );


//	ContecCpsAioReadAiCalibrationData(*Id, 0, &g, &o);
//	ContecCpsAioSetAiCalibrationData(*Id, CPSAIO_AI_CALIBRATION_SELECT_OFFSET, 0, CPSAIO_AI_CALIBRATION_RANGE_PM10,  o);
//	ContecCpsAioSetAiCalibrationData(*Id, CPSAIO_AI_CALIBRATION_SELECT_GAIN, 0, CPSAIO_AI_CALIBRATION_RANGE_PM10,  g);
	return AIO_ERR_SUCCESS;

}

unsigned long ContecCpsAioExit( short Id )
{
	struct cpsaio_ioctl_arg	arg;
	arg.val = 0;

	ioctl( Id, IOCTL_CPSAIO_EXIT, &arg );
	// close
	close( Id );
	return AIO_ERR_SUCCESS;

}
unsigned long ContecCpsAioGetErrorStrings( unsigned long code, char *Str )
{

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioQueryDeviceName( short Index, char *DeviceName, char *Device )
{

	
	return AIO_ERR_SUCCESS;
}

/*** Ai/Ao Get Resolution function ***/

unsigned long ContecCpsAioGetAiResolution( short Id, unsigned short *AiResolution )
{
	struct cpsaio_ioctl_arg	arg;

	/* Get resolution */
	arg.inout = CPS_AIO_INOUT_AI;
	ioctl( Id, IOCTL_CPSAIO_GETRESOLUTION, &arg );

	*AiResolution = (unsigned short)arg.val;

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioGetAoResolution( short Id, unsigned short *AoResolution )
{
	struct cpsaio_ioctl_arg	arg;

	/* Get resolution */
	arg.inout = CPS_AIO_INOUT_AO;
	ioctl( Id, IOCTL_CPSAIO_GETRESOLUTION, &arg );

	*AoResolution = (unsigned short)arg.val;

	return AIO_ERR_SUCCESS;
}

/*** Ai Channel function ***/

unsigned long ContecCpsAioGetAiMaxChannels( short Id, short *AiMaxChannels ){
	struct cpsaio_ioctl_arg	arg;

	arg.inout = CPS_AIO_INOUT_AI;
	ioctl( Id, IOCTL_CPSAIO_GETMAXCHANNEL, &arg );
	*AiMaxChannels = arg.val;

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioSetAiChannels( short Id, short AiChannels )
{
	struct cpsaio_ioctl_arg	arg;

	_contec_cpsaio_set_exchange( Id, 0, 1 );

	/* Set Channel */
	arg.val = AiChannels - 1;
	ioctl( Id, IOCTL_CPSAIO_SETCHANNEL_AI, &arg );

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioGetAiChannels( short Id, short *AiChannels )
{
	struct cpsaio_ioctl_arg	arg;

	/* Get Channel */
	ioctl( Id, IOCTL_CPSAIO_GETCHANNEL_AI, &arg );

	*AiChannels = (short)(arg.val + 1);

	return AIO_ERR_SUCCESS;
}


unsigned long ContecCpsAioSetAiSamplingClock( short Id, double AiSamplingClock )
{
	struct cpsaio_ioctl_arg	arg;

	/* Set Channel */
	arg.val = (unsigned long) ( (AiSamplingClock * 1000.0 / 25.0) - 1 );
	ioctl( Id, IOCTL_CPSAIO_SET_CLOCK_AI, &arg );

	return AIO_ERR_SUCCESS;

}

unsigned long ContecCpsAioGetAiSamplingClock( short Id, double *AiSamplingClock )
{
	struct cpsaio_ioctl_arg	arg;

	/* Get Channel */
	ioctl( Id, IOCTL_CPSAIO_GET_CLOCK_AI, &arg );

	*AiSamplingClock = (double) ( (arg.val + 1.0 ) * 25.0 ) / 1000.0;

	return AIO_ERR_SUCCESS;

}

unsigned long ContecCpsAioSetAiEventSamplingTimes( short Id, unsigned long AiSamplingTimes )
{
	struct cpsaio_ioctl_arg	arg;

	/* Set Sampling number( before Trigger ) */
	arg.val = AiSamplingTimes - 1;
	ioctl( Id, IOCTL_CPSAIO_SET_SAMPNUM_AI, &arg );

	return AIO_ERR_SUCCESS;

}

unsigned long ContecCpsAioGetAiEventSamplingTimes( short Id, unsigned long *AiSamplingTimes )
{
	struct cpsaio_ioctl_arg	arg;

	/* Get Sampling number( before Trigger ) */
	ioctl( Id, IOCTL_CPSAIO_GET_SAMPNUM_AI, &arg );
	*AiSamplingTimes = arg.val + 1;

	return AIO_ERR_SUCCESS;

}

/**** Running Functions ****/
unsigned long ContecCpsAioStartAi( short Id )
{
	struct cpsaio_ioctl_arg	arg;
	unsigned count = 0;

	ioctl( Id, IOCTL_CPSAIO_START_AI, 0 );

	do{
		usleep( 1 );
		ioctl( Id, IOCTL_CPSAIO_INSTATUS, &arg );
		if( count >= 1000 ) return 1;
		count ++;
	}while( arg.val & CPS_AIO_AI_STATUS_START_DISABLE );

	//softtrigger only out pulse start
	ioctl( Id, IOCTL_CPSAIO_SET_OUTPULSE0, 0 );

	count = 0;
	do{
		usleep( 1 );
		ioctl( Id, IOCTL_CPSAIO_GET_INTERRUPT_FLAG_AI , &arg);
		if( count >= 1000 ) return 2;
		count ++;
	}while(!( arg.val & CPS_AIO_AI_STATUS_MOTION_END ) );

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioStopAi( short Id )
{
	ioctl( Id, IOCTL_CPSAIO_STOP_AI, 0 );
	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioGetAiStatus( short Id, long *AiStatus )
{
	struct cpsaio_ioctl_arg	arg;

	ioctl( Id, IOCTL_CPSAIO_INSTATUS, &arg );

	*AiStatus = (long)arg.val;

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioGetAiSamplingData( short Id, short AiSamplingTimes, long AiData[] )
{

	unsigned char *tmpAiData;
	int cnt;

	tmpAiData = (unsigned char * )malloc( sizeof(unsigned char)* AiSamplingTimes * 2 );

	read( Id, tmpAiData , (size_t)(AiSamplingTimes * 2) );

	for( cnt = 0;cnt < AiSamplingTimes * 2; cnt += 2 ){
		AiData[cnt/2] = (long) ( ( tmpAiData[cnt+1] << 8 ) | tmpAiData[cnt]);
	}

	free(tmpAiData);

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioGetAiSamplingDataEx( short Id, short AiSamplingTimes, double AiData[] )
{

	long *tmpAiData;
	unsigned short AiResolution = 0;
	int cnt;
	double dblMin, dblMax;

	tmpAiData = (long*)malloc( sizeof(long) * AiSamplingTimes );

	ContecCpsAioGetAiSamplingData( Id, AiSamplingTimes, tmpAiData );

	ContecCpsAioGetAiResolution( Id, &AiResolution );

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
	for( cnt = 0;cnt < AiSamplingTimes; cnt ++){
		AiData[cnt] =  (double)( tmpAiData[cnt] / pow(2.0,AiResolution) ) *(dblMax - dblMin) + dblMin;
	}
	free(tmpAiData);

}

unsigned long ContecCpsAioSingleAi( short Id, short AiChannel, long *AiData )
{

	struct cpsaio_ioctl_arg	arg;

	// Exchange Transfer Mode Single Ai
	_contec_cpsaio_set_exchange( Id, 0, 0 );

	// Set Ai Channel
	arg.val = AiChannel;
	ioctl( Id, IOCTL_CPSAIO_SETCHANNEL_AI, &arg );

	// Set Sampling Number
	ContecCpsAioSetAiEventSamplingTimes( Id, 1 );

	// Ai Start
	ContecCpsAioStartAi( Id );

	ioctl( Id, IOCTL_CPSAIO_INDATA, &arg );
	*AiData = (long)( arg.val );

	ContecCpsAioStopAi( Id );

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioSingleAiEx( short Id, short AiChannel, double *AiData )
{

	long tmpAiData = 0;
	unsigned short AiResolution = 0;

	double dblMin, dblMax;

	ContecCpsAioSingleAi( Id, AiChannel, &tmpAiData );

	ContecCpsAioGetAiResolution( Id, &AiResolution );

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

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioMultiAi( short Id, short AiChannels, long AiData[] )
{

	struct cpsaio_ioctl_arg	arg;
	int cnt;

	ContecCpsAioSetAiChannels(Id, AiChannels );

	// Set Sampling Number
	ContecCpsAioSetAiEventSamplingTimes( Id, 1 );

	// Ai Start
	ContecCpsAioStartAi( Id );

	for( cnt = 0;cnt < AiChannels; cnt ++ ){
		ioctl( Id, IOCTL_CPSAIO_INDATA, &arg );
		AiData[cnt] = (long)( arg.val );
	}

	// Ai Stop
	ContecCpsAioStopAi( Id );

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioMultiAiEx( short Id, short AiChannels, double AiData[] )
{

	long *tmpAiData;
	unsigned short AiResolution = 0;
	int cnt;
	double dblMin, dblMax;

	tmpAiData = (long*)malloc( sizeof(long) * AiChannels );

	ContecCpsAioMultiAi( Id, AiChannels, tmpAiData );

	ContecCpsAioGetAiResolution( Id, &AiResolution );

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
	free(tmpAiData);

	return AIO_ERR_SUCCESS;
}

/* Ao Channel */

unsigned long ContecCpsAioGetAoMaxChannels( short Id, short *AoMaxChannels ){
	struct cpsaio_ioctl_arg	arg;

	arg.inout = CPS_AIO_INOUT_AO;
	ioctl( Id, IOCTL_CPSAIO_GETMAXCHANNEL, &arg );
	*AoMaxChannels = arg.val;

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioSetAoChannels( short Id, short AoChannels )
{
	struct cpsaio_ioctl_arg	arg;

	/* Multi only*/
	_contec_cpsaio_set_exchange( Id, 1, 1 );

	/* Set Channel */
	arg.val = AoChannels - 1;
	ioctl( Id, IOCTL_CPSAIO_SETCHANNEL_AO, &arg );

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioSetAoEventSamplingTimes( short Id, unsigned long AoSamplingTimes )
{
	struct cpsaio_ioctl_arg	arg;

	/* Set Channel */
	arg.val = AoSamplingTimes - 1 ;
	ioctl( Id, IOCTL_CPSAIO_SET_SAMPNUM_AO, &arg );

	return AIO_ERR_SUCCESS;

}


/**** Running Functions ****/
unsigned long ContecCpsAioStartAo( short Id )
{

	struct cpsaio_ioctl_arg	arg;

	ioctl( Id, IOCTL_CPSAIO_START_AO, NULL );

//	do{
//		ioctl( Id, IOCTL_CPSAIO_OUTSTATUS, &arg );;
//	}while( arg.val & 0x10 );

	//softtrigger only out pulse start
	ioctl( Id, IOCTL_CPSAIO_SET_OUTPULSE0, 0 );

	return AIO_ERR_SUCCESS;

}

unsigned long ContecCpsAioStopAo( short Id )
{
	ioctl( Id, IOCTL_CPSAIO_STOP_AO, NULL );
	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioGetAoStatus( short Id, long *AoStatus )
{
	struct cpsaio_ioctl_arg	arg;

	ioctl( Id, IOCTL_CPSAIO_OUTSTATUS, &arg );

	*AoStatus = (long)arg.val;

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioSetAoSamplingClock( short Id, double AoSamplingClock )
{
	struct cpsaio_ioctl_arg	arg;

	/* Set Clock */
	arg.val = (unsigned long) (AoSamplingClock * 1000.0 / 25.0) - 1 ;
	ioctl( Id, IOCTL_CPSAIO_SET_CLOCK_AO, &arg );

	return AIO_ERR_SUCCESS;

}

unsigned long ContecCpsAioSingleAo( short Id, short AoChannel, long AoData )
{

	struct cpsaio_ioctl_arg	arg;
	long AoStatus = 0;

	// Exchange Transfer Mode Single Ao
	_contec_cpsaio_set_exchange( Id, 1, 0 );

	// Set Ao Channel
	arg.val = AoChannel;
	ioctl( Id, IOCTL_CPSAIO_SETCHANNEL_AO, &arg );

	// Set Sampling Number
	ContecCpsAioSetAoEventSamplingTimes( Id, 1 );

	arg.val = AoData;
	ioctl( Id, IOCTL_CPSAIO_OUTDATA, &arg );

	// Ao Start
	ContecCpsAioStartAo( Id );



	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioSingleAoEx( short Id, short AoChannel, double AoData )
{

	long tmpAoData = 0;
	unsigned short AoResolution = 0;

	double dblMin, dblMax;

	ContecCpsAioGetAoResolution( Id, &AoResolution );

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
	
	ContecCpsAioSingleAo( Id, AoChannel, tmpAoData );

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioMultiAo( short Id, short AoChannels, long AoData[] )
{

	struct cpsaio_ioctl_arg	arg;
	int cnt;

	ContecCpsAioSetAoChannels(Id, AoChannels );

	// Set Sampling Number
	ContecCpsAioSetAoEventSamplingTimes( Id, 1 );

	// Ai Start
	ContecCpsAioStartAo( Id );

	for( cnt = 0;cnt < AoChannels; cnt ++ ){
		arg.val = (long)AoData[cnt];
		ioctl( Id, IOCTL_CPSAIO_OUTDATA, &arg );
	}

	// Ai Stop
	ContecCpsAioStopAo( Id );

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioMultiAoEx( short Id, short AoChannels, double AoData[] )
{

	long *tmpAoData;
	unsigned short AoResolution = 0;
	int cnt;
	double dblMin, dblMax;

	tmpAoData = (long*)malloc( sizeof(long) * AoChannels );

	ContecCpsAioGetAoResolution( Id, &AoResolution );

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

	ContecCpsAioMultiAo( Id, AoChannels, tmpAoData );

	free(tmpAoData);

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioSetEcuSignal( short Id, unsigned short dest, unsigned short src )
{
	struct cpsaio_ioctl_arg	arg;

	arg.val = CPSAIO_ECU_DESTSRC_SIGNAL(dest, src);
	ioctl( Id, IOCTL_CPSAIO_SETECU_SIGNAL, &arg);

	return AIO_ERR_SUCCESS;
}

// 2016-01-20 (1) 
unsigned long ContecCpsAioSetAiCalibrationData( short Id, unsigned char select, unsigned char ch, unsigned char range, unsigned short data )
{
	int aisel = 0;
	struct cpsaio_ioctl_arg	arg;


	arg.ch = ch;
	arg.val = CPSAIO_AI_CALIBRATION_DATA(select, ch, range, aisel, data);
	ioctl( Id, IOCTL_CPSAIO_SET_CALIBRATION_AI, &arg);

	return AIO_ERR_SUCCESS;

} 

unsigned long ContecCpsAioGetAiCalibrationData( short Id, unsigned char *select, unsigned char *ch, unsigned char *range, unsigned short *data )
{
	struct cpsaio_ioctl_arg	arg;

	ioctl( Id, IOCTL_CPSAIO_GET_CALIBRATION_AI, &arg);

	*select = CPSAIO_AI_CALIBRATION_GETSELECT( arg.val );
	*range = CPSAIO_AI_CALIBRATION_GETRANGE( arg.val );	
	*data = CPSAIO_AI_CALIBRATION_GETDATA( arg.val );

	return AIO_ERR_SUCCESS;
} 

unsigned long ContecCpsAioWriteAiCalibrationData( short Id, unsigned char ch, unsigned char gain, unsigned char offset )
{
	int aisel = 0;
	struct cpsaio_ioctl_arg	arg;

	arg.val = ( gain << 8 )  | offset;
	ioctl( Id, IOCTL_CPSAIO_WRITE_EEPROM_AI, &arg);

	return AIO_ERR_SUCCESS;	

}

unsigned long ContecCpsAioReadAiCalibrationData( short Id, unsigned char ch, unsigned char *gain, unsigned char *offset )
{
	int aisel = 0;
	struct cpsaio_ioctl_arg	arg;

	ioctl( Id, IOCTL_CPSAIO_READ_EEPROM_AI, &arg);

	*gain = 	(arg.val & 0xFF00 ) >> 8;

	*offset = (arg.val & 0xFF );

	return AIO_ERR_SUCCESS;	

}

unsigned long ContecCpsAioClearAiCalibrationData( short Id, int iClear )
{

	if( iClear & CPSAIO_AI_CALIBRATION_CLEAR_RAM	 ){
		//FPGA all Clear
		ContecCpsAioSetAiCalibrationData( Id, CPSAIO_AI_CALIBRATION_SELECT_OFFSET, 0, CPSAIO_AI_CALIBRATION_RANGE_PM10, 0 );
		ContecCpsAioSetAiCalibrationData( Id, CPSAIO_AI_CALIBRATION_SELECT_GAIN, 0, CPSAIO_AI_CALIBRATION_RANGE_PM10, 0 );
	}

	if( iClear & CPSAIO_AI_CALIBRATION_CLEAR_ROM	 ){
		//FPGA ROM CLEAR
		ioctl( Id, IOCTL_CPSAIO_CLEAR_EEPROM, NULL);
	}

	return AIO_ERR_SUCCESS;	

}

// 2016-01-20 (1) 
unsigned long ContecCpsAioSetAoCalibrationData( short Id, unsigned char select, unsigned char ch, unsigned char range, unsigned short data )
{
	int aisel = 0;
	struct cpsaio_ioctl_arg	arg;

	arg.val = CPSAIO_AO_CALIBRATION_DATA(select, ch, range, aisel, data);
	ioctl( Id, IOCTL_CPSAIO_SET_CALIBRATION_AO, &arg);

	return AIO_ERR_SUCCESS;

} 

unsigned long ContecCpsAioGetAoCalibrationData( short Id, unsigned char *select, unsigned char *ch, unsigned char *range, unsigned short *data )
{
	struct cpsaio_ioctl_arg	arg;


	ioctl( Id, IOCTL_CPSAIO_GET_CALIBRATION_AO, &arg);

	*select = CPSAIO_AO_CALIBRATION_GETSELECT( arg.val );
	*range = CPSAIO_AO_CALIBRATION_GETRANGE( arg.val );	
	*data = CPSAIO_AO_CALIBRATION_GETDATA( arg.val );

	return AIO_ERR_SUCCESS;
} 


unsigned long ContecCpsAioWriteAoCalibrationData( short Id, unsigned char ch, unsigned char gain, unsigned char offset )
{
	int aisel = 0;
	struct cpsaio_ioctl_arg	arg;

	arg.ch = ch;
	arg.val = ( gain << 8 )  | offset;
	ioctl( Id, IOCTL_CPSAIO_WRITE_EEPROM_AO, &arg);

	return AIO_ERR_SUCCESS;	

}

unsigned long ContecCpsAioReadAoCalibrationData( short Id, unsigned char ch, unsigned char *gain, unsigned char *offset )
{
	int aisel = 0;
	struct cpsaio_ioctl_arg	arg;

	arg.ch = ch;

	ioctl( Id, IOCTL_CPSAIO_READ_EEPROM_AO, &arg);

	*gain = 	(arg.val & 0xFF00 ) >> 8;

	*offset = (arg.val & 0xFF );

	return AIO_ERR_SUCCESS;	

}

unsigned long ContecCpsAioClearAoCalibrationData( short Id, int iClear )
{
	int cnt;

	if( iClear & CPSAIO_AO_CALIBRATION_CLEAR_RAM	 ){
		//FPGA all Clear
		for( cnt = 0;cnt < 4 ;cnt ++ ){
			ContecCpsAioSetAoCalibrationData( Id, CPSAIO_AO_CALIBRATION_SELECT_OFFSET, cnt, CPSAIO_AO_CALIBRATION_RANGE_P20MA, 0 );
			ContecCpsAioSetAoCalibrationData( Id, CPSAIO_AO_CALIBRATION_SELECT_GAIN, cnt, CPSAIO_AO_CALIBRATION_RANGE_P20MA, 0 );
		}
	}
	if( iClear & CPSAIO_AO_CALIBRATION_CLEAR_ROM	 ){
		//FPGA ROM CLEAR
		ioctl( Id, IOCTL_CPSAIO_CLEAR_EEPROM, NULL);
	}

	return AIO_ERR_SUCCESS;	

}

/* Direct Input / Output (Debug) */


unsigned long ContecCpsAioInp( short Id, unsigned long addr, unsigned char *value )
{
	struct cpsaio_direct_arg arg;

	arg.addr = addr;
	ioctl( Id, IOCTL_CPSAIO_DIRECT_INPUT, arg );
	*value = (unsigned char)arg.val;

	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioInpW( short Id, unsigned long addr, unsigned short *value )
{
	struct cpsaio_direct_arg arg;
	
	arg.addr = addr;
	ioctl( Id, IOCTL_CPSAIO_DIRECT_INPUT, arg );
	*value = (unsigned short)arg.val;
	
	return AIO_ERR_SUCCESS;

}

unsigned long ContecCpsAioInpD( short Id, unsigned long addr, unsigned long *value )
{
	struct cpsaio_direct_arg arg;
	
	arg.addr = addr;
	ioctl( Id, IOCTL_CPSAIO_DIRECT_INPUT, arg );
	*value = (unsigned long)arg.val;
	
	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioOutp( short Id, unsigned long addr, unsigned char value )
{
	struct cpsaio_direct_arg arg;
	
	arg.addr = addr;
	arg.val = (unsigned long)value;
	ioctl( Id, IOCTL_CPSAIO_DIRECT_OUTPUT, arg );
	
	return AIO_ERR_SUCCESS;
}

unsigned long ContecCpsAioOutpW( short Id, unsigned long addr, unsigned short value )
{
	struct cpsaio_direct_arg arg;
	
	arg.addr = addr;
	arg.val = (unsigned long)value;
	ioctl( Id, IOCTL_CPSAIO_DIRECT_OUTPUT, arg );
	
	return AIO_ERR_SUCCESS;

}

unsigned long ContecCpsAioOutpD( short Id, unsigned long addr, unsigned long value )
{
	struct cpsaio_direct_arg arg;
	
	arg.addr = addr;
	arg.val = (unsigned long)value;
	ioctl( Id, IOCTL_CPSAIO_DIRECT_OUTPUT, arg );
	
	return AIO_ERR_SUCCESS;

}

unsigned long ContecCpsAioEcuInp( short Id, unsigned long addr, unsigned char *value )
{
	struct cpsaio_direct_command_arg arg;

	arg.addr = addr;
	arg.isEcu = 1;
	ioctl( Id, IOCTL_CPSAIO_DIRECT_INPUT, arg );
	*value = (unsigned char)arg.val;

	return AIO_ERR_SUCCESS;

}


unsigned long ContecCpsAioEcuInpW( short Id, unsigned long addr, unsigned short *value )
{

	return 0;
}

unsigned long ContecCpsAioEcuInpD( short Id, unsigned long addr, unsigned long *value )
{

	return 0;
}

unsigned long ContecCpsAioEcuOutp( short Id, unsigned long addr, unsigned char value )
{

	return 0;
}

unsigned long ContecCpsAioEcuOutpW( short Id, unsigned long addr, unsigned short value )
{

	return 0;
}

unsigned long ContecCpsAioEcuOutpD( short Id, unsigned long addr, unsigned long value )
{

	return 0;
}

unsigned long ContecCpsAioCommandInp( short Id, unsigned long addr, unsigned char *value )
{

	return 0;
}

unsigned long ContecCpsAioCommandInpW( short Id, unsigned long addr, unsigned short *value )
{

	return 0;
}

unsigned long ContecCpsAioCommandInpD( short Id, unsigned long addr, unsigned long *value )
{

	return 0;
}

unsigned long ContecCpsAioCommandOutp( short Id, unsigned long addr, unsigned char value )
{

	return 0;
}

unsigned long ContecCpsAioCommandOutpW( short Id, unsigned long addr, unsigned short value )
{

	return 0;
}

unsigned long ContecCpsAioCommandOutpD( short Id, unsigned long addr, unsigned long value )
{

	return 0;
}

