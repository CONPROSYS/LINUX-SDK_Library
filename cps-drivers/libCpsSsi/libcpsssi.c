/*
 *  Lib for CONTEC ConProSys SenSor Input (CPS-SSI) Series.
 *
 *  Copyright (C) 2016 Syunsuke Okamoto.
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
   <http://www.gnu.org/licenses/>.  */
//#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include "cpsssi.h"
#include "../include/libcpsssi.h"

typedef struct __contec_cps_ssi_int_callback__
{
	PCONTEC_CPS_SSI_INT_CALLBACK func;
	CONTEC_CPS_SSI_INT_CALLBACK_DATA data;
}CONTEC_CPS_SSI_INT_CALLBACK_LIST, *PCONTEC_CPS_SSI_INT_CALLBACK_LIST;

CONTEC_CPS_SSI_INT_CALLBACK_LIST contec_cps_ssi_cb_list[CPS_DEVICE_MAX_NUM];

void _contec_cpsssi_signal_proc( int signo )
{
	int cnt;
	if( signo == SIGUSR2 ){
		for( cnt = 0; cnt < CPS_DEVICE_MAX_NUM ; cnt ++){
			if( contec_cps_ssi_cb_list[cnt].func != (PCONTEC_CPS_SSI_INT_CALLBACK)NULL ){
				
				contec_cps_ssi_cb_list[cnt].func(
					contec_cps_ssi_cb_list[cnt].data.id,
					SSIM_INTERRUPT,
					contec_cps_ssi_cb_list[cnt].data.wParam,
					contec_cps_ssi_cb_list[cnt].data.lParam,
					contec_cps_ssi_cb_list[cnt].data.Param
				);
			}
		}
	}
}

unsigned long ContecCpsSsiInit( char *DeviceName, short *Id )
{
	// open
	char Name[32];
	struct cpsssi_ioctl_arg	arg;

	strcpy(Name, "/dev/");
	strcat(Name, DeviceName);

	*Id = open( Name, O_RDWR );

	if( *Id <= 0 ) return SSI_ERR_DLL_CREATE_FILE;

	ioctl( *Id, IOCTL_CPSSSI_INIT, &arg );

	ContecCpsSsiSetSenceRegister( *Id, 2000.0 ); 
	
	return SSI_ERR_SUCCESS;

}

unsigned long ContecCpsSsiExit( short Id )
{
	struct cpsssi_ioctl_arg	arg;
	arg.val = 0;

	ioctl( Id, IOCTL_CPSSSI_EXIT, &arg );
	// close
	close( Id );
	return SSI_ERR_SUCCESS;

}
unsigned long ContecCpsSsiGetErrorStrings( unsigned long code, char *Str )
{

	return SSI_ERR_SUCCESS;
}

unsigned long ContecCpsSsiQueryDeviceName( short Id, char *DeviceName, char *Device )
{

	return SSI_ERR_SUCCESS;
}

unsigned long ContecCpsSsiSetChannel( short Id, short SsiChannel , unsigned int iWire, unsigned int iJpt )
{
	struct cpsssi_ioctl_arg	arg;
	unsigned char isWire = 0;
	unsigned char isCountry = 0;

	switch( iWire ){
	case SSI_CHANNEL_3WIRE:
		isWire = SSI_4P_CHANNEL_RTD_WIRE_3;
		break;
	case SSI_CHANNEL_4WIRE:
	default:
		isWire = SSI_4P_CHANNEL_RTD_WIRE_4;
		break;
	}

	switch( iJpt ){
	case SSI_CHANNEL_JPT:
		isCountry = SSI_4P_CHANNEL_STANDARD_JP;
		break;
	case SSI_CHANNEL_PT:
	default:
		isCountry = SSI_4P_CHANNEL_STANDARD_EU;
		break;
	}

	arg.ch = SsiChannel;
	arg.val = SSI_4P_CHANNEL_SET_RTD(
		SSI_4P_CHANNEL_RTD_PT_100,
		SSI_4P_CHANNEL_RTD_SENCE_POINTER_CH3TOCH2,
		isWire,
		SSI_4P_CHANNEL_RTD_EXCITATION_CURRENT_250UA,
		isCountry
	); // 0x60F5C000; // omajinai
	
	ioctl( Id, IOCTL_CPSSSI_SET_CHANNEL, &arg );

	return SSI_ERR_SUCCESS;
}

unsigned long ContecCpsSsiGetChannel( short Id, short SsiChannel , unsigned int *iWire, unsigned int *iJpt )
{
	struct cpsssi_ioctl_arg	arg;
	unsigned char isWire = 0;
	unsigned char isCountry = 0;

	arg.ch = SsiChannel;
	ioctl( Id, IOCTL_CPSSSI_GET_CHANNEL, &arg );

	isWire = SSI_4P_CHANNEL_GET_RTD_WIRE_MODE( arg.val );
	isCountry = SSI_4P_CHANNEL_GET_RTD_STANDARD( arg.val );

	if( iWire != NULL ){
		switch( isWire ){
		case SSI_4P_CHANNEL_RTD_WIRE_3:
			*iWire = SSI_CHANNEL_3WIRE;
			break;
		case SSI_4P_CHANNEL_RTD_WIRE_4:
		default:
			*iWire = SSI_CHANNEL_4WIRE;
			break;
		}
	}

	if( iJpt != NULL ){
		switch( isCountry ){
		case SSI_4P_CHANNEL_STANDARD_EU :
			*iJpt = SSI_CHANNEL_PT;
			break;
		case SSI_4P_CHANNEL_STANDARD_JP :
			*iJpt = SSI_CHANNEL_JPT;
			break;
		}
	}

	return SSI_ERR_SUCCESS;
}


unsigned long ContecCpsSsiSetSenceRegister( short Id, double sence )
{
	struct cpsssi_ioctl_arg	arg;
	unsigned long ulSence;

	ulSence = (unsigned long)( sence * pow(2.0, 10.0 ) );
	
	arg.val = SSI_4P_CHANNEL_SET_SENCE(
		ulSence
	); // 0xe81f4000; // omajinai


	ioctl( Id, IOCTL_CPSSSI_SET_SENCE_RESISTANCE, &arg );

	return SSI_ERR_SUCCESS;
}

unsigned long ContecCpsSsiGetSenceRegister( short Id, double *sence )
{
	struct cpsssi_ioctl_arg	arg;
	unsigned long ulSence;

	ioctl( Id, IOCTL_CPSSSI_GET_SENCE_RESISTANCE, &arg );

	ulSence = SSI_4P_CHANNEL_GET_SENCE( arg.val );

	*sence = ( (double)ulSence ) /  pow(2.0, 10.0 ) ;

	return SSI_ERR_SUCCESS;
}

/**** Running Functions ****/

unsigned long ContecCpsSsiGetStatus( short Id, unsigned long *SsiStatus )
{
	struct cpsssi_ioctl_arg	arg;

	ioctl( Id, IOCTL_CPSSSI_INSTATUS, &arg );

	*SsiStatus = arg.val;

	return SSI_ERR_SUCCESS;
}

/***
	@note The task script is called function.
***/
unsigned long ContecCpsSsiStart( short Id, short SsiChannel )
{
	struct cpsssi_ioctl_arg	arg;

	arg.ch = SsiChannel;
	arg.val = 0;

	// Start SSI Channel
	ioctl( Id, IOCTL_CPSSSI_START, &arg );
	return SSI_ERR_SUCCESS;
}

/***
	@note The task script is called function.
***/
unsigned long ContecCpsIsConversionStartBusyStatus( short Id, unsigned long *SsiStatus )
{
	struct cpsssi_ioctl_arg	arg;

	ioctl( Id, IOCTL_CPSSSI_STARTBUSYSTATUS, &arg );

	*SsiStatus = arg.val;

	return SSI_ERR_SUCCESS;
}

/***
	@note The task script is called function.
***/
unsigned long ContecCpsSsiGetData( short Id, short SsiChannel, long *SsiData )
{
	struct cpsssi_ioctl_arg	arg;

	arg.ch = SsiChannel;
	arg.val = 0;

	ioctl( Id, IOCTL_CPSSSI_INDATA, &arg );
	*SsiData = (long)( arg.val );

	return SSI_ERR_SUCCESS;
}

unsigned long ContecCpsSsiSingle( short Id, short SsiChannel, long *SsiData )
{

	struct cpsssi_ioctl_arg	arg;
	long status = 0;

	arg.ch = SsiChannel;
	arg.val = 0;

	// Start SSI Channel
	ioctl( Id, IOCTL_CPSSSI_START, &arg );
	// SSI Status Check
	//do{
	//	ContecCpsStartBusyStatus( Id, &status );
	//}while( !(status & 0x40) );
	// Get SSI Data
	ioctl( Id, IOCTL_CPSSSI_INDATA, &arg );
	*SsiData = (long)( arg.val );

	return SSI_ERR_SUCCESS;
}

unsigned long ContecCpsSsiSingleTemperature( short Id, short SsiChannel, double *SsiTempData )
{
	long val;
	double tmpVal;

	ContecCpsSsiSingle( Id, SsiChannel, &val );

	tmpVal = (double) ( ( val & 0x007FFFFF ) - (val & 0x00800000 ) );

	*SsiTempData = (tmpVal / 1024.0); 

	return SSI_ERR_SUCCESS;

}

unsigned long ContecCpsSsiSingleRegistance( short Id, short SsiChannel, double *SsiRegistance )
{

	double tmpVal;

	double R0 = 100.0;
	double alpha;
	double a;
	double b;
	double c;
	int iJpt;

	ContecCpsSsiSingleTemperature( Id, SsiChannel, &tmpVal );

	ContecCpsSsiGetChannel( Id, SsiChannel, NULL, &iJpt );

	switch( iJpt ){

	case	SSI_CHANNEL_JPT	:
		alpha = 0.003916;
		a = 3.97390 * pow( 10.0, -3.0 );
		b = -5.870 * pow( 10.0, -7.0 );
		c = -4.40 * pow( 10.0, -12.0 );
		break;
	case	SSI_CHANNEL_PT	:
	default:
		alpha = 0.00385;
		a = 3.9083 * pow( 10.0, -3.0 );
		b = -5.775 * pow( 10.0, -7.0 );
		c = -4.183 * pow( 10.0, -12.0 );
		break;
	}

	if( tmpVal > 0.0 ){
		*SsiRegistance = R0 * ( 1.0 + a * tmpVal + b * pow( tmpVal, 2.0 ) ); 
	}else if( tmpVal < 0.0 ){
		*SsiRegistance = R0 * ( 1.0 + a * tmpVal + b * pow( tmpVal, 2.0 ) + ( tmpVal -100.0 ) * c * pow( tmpVal, 3.0 ) ); 
	}else{
		*SsiRegistance = R0;
	}

	return SSI_ERR_SUCCESS;
}


unsigned char _contec_cpsssi_4p_offset_double2uchar( double data , double pw )
{
	unsigned char cVal;
	cVal = (unsigned char)( data * pw );
	if( data >= 0.0 )
		cVal = (unsigned char)( data * pw );
	else
		cVal = ((unsigned char)( data * -1 * pw ) ^ 0xFF ) + 1;

	return cVal;

}

unsigned short _contec_cpsssi_4p_gain_double2ushort( double data , double pw )
{
	unsigned short sVal;
	sVal = (unsigned short)( data * pw );
	if( data >= 0.0 )
		sVal = (unsigned short)( data * pw );
	else
		sVal = ((unsigned short)( data * -1 * pw ) ^ 0xFFFF ) + 1;

	return sVal;

}

double _contec_cpsssi_4p_offset_uchar2double( unsigned char cVal, double pw )
{
	double dblVal;

	if( cVal & 0x80 )
		dblVal = 	(-1.0) * (double)( 0x80 - (cVal & 0x7F) ) / pw;
	else
		dblVal = 	(double)( cVal & 0x7F ) / pw;

	return dblVal;
}

double _contec_cpsssi_4p_gain_ushort2double( unsigned short data, double pw )
{
	double dblVal;
	if( data & 0x8000 )
		dblVal = ( -1.0 ) * (double)( 0x8000 - (data & 0x7FFF) ) / pw; 
	else
		dblVal = (double)( data & 0x7FFF ) / pw;

	return dblVal;
}


unsigned long ContecCpsSsiSetCalibrationOffsetToUShort( short Id, unsigned char ch, unsigned int iWire, unsigned short data )
{
	struct cpsssi_ioctl_arg	arg;
	unsigned int iTmpJpt = 0, iTmpWire = 0;

	ContecCpsSsiGetChannel( Id, ch , &iTmpWire, &iTmpJpt );
	ContecCpsSsiSetChannel( Id, ch , iWire, iTmpJpt );

	arg.ch = ch;
	arg.val = data;
	ioctl( Id, IOCTL_CPSSSI_SET_OFFSET, &arg);

	ContecCpsSsiSetChannel( Id, ch , iTmpWire, iTmpJpt );

	return SSI_ERR_SUCCESS;

} 

unsigned long ContecCpsSsiSetCalibrationOffset( short Id, unsigned char ch, unsigned int iWire, double data )
{
	unsigned char cVal = 0;
	
	cVal = _contec_cpsssi_4p_offset_double2uchar( data , 32.0 );

	return ContecCpsSsiSetCalibrationOffsetToUShort( Id, ch, iWire, cVal );

} 

unsigned long ContecCpsSsiGetCalibrationOffsetToUShort( short Id, unsigned char ch, unsigned int iWire, unsigned short *data )
{
	struct cpsssi_ioctl_arg	arg;
	unsigned int iTmpJpt = 0, iTmpWire = 0;

	ContecCpsSsiGetChannel( Id, ch , &iTmpWire, &iTmpJpt );
	ContecCpsSsiSetChannel( Id, ch , iWire, iTmpJpt );

	arg.ch = ch;
	ioctl( Id, IOCTL_CPSSSI_GET_OFFSET, &arg);
	*data = 	arg.val;

	ContecCpsSsiSetChannel( Id, ch , iTmpWire, iTmpJpt );

	return SSI_ERR_SUCCESS;
} 

unsigned long ContecCpsSsiGetCalibrationOffset( short Id, unsigned char ch, unsigned int iWire, double *data )
{
	unsigned short usData = 0;
	unsigned long lRet = 0;

	lRet = ContecCpsSsiGetCalibrationOffsetToUShort( Id, ch, iWire, &usData );
	*data = _contec_cpsssi_4p_offset_uchar2double((unsigned char)usData , 32.0 );

	return lRet;
} 

unsigned long ContecCpsSsiSetCalibrationGain( short Id, double data )
{
	return ContecCpsSsiSetSenceRegister( Id, (data + SSI_4P_SENCE_DEFAULT_VALUE) );
} 

unsigned long ContecCpsSsiSetCalibrationGainToUShort( short Id, unsigned short data )
{
	double sence;

	sence = _contec_cpsssi_4p_gain_ushort2double( data, pow( 2.0, 10.0 ) );

	return ContecCpsSsiSetSenceRegister( Id, (sence + SSI_4P_SENCE_DEFAULT_VALUE) );

} 

unsigned long ContecCpsSsiGetCalibrationGain( short Id, double *data )
{
	double tmpData = 0.0;
	unsigned long lRet;

	lRet = ContecCpsSsiGetSenceRegister( Id, &tmpData );
	*data = ( tmpData - SSI_4P_SENCE_DEFAULT_VALUE );
	return lRet;
} 

unsigned long ContecCpsSsiWriteCalibrationGainToUShort( short Id, unsigned short value )
{
	struct cpsssi_ioctl_arg	arg;

	arg.ch = 0;
	arg.val = value;
	ioctl( Id, IOCTL_CPSSSI_WRITE_EEPROM_SSI, &arg);

	return SSI_ERR_SUCCESS;	

}

unsigned long ContecCpsSsiWriteCalibrationGain( short Id, double dblVal )
{

	unsigned short value;

	value = _contec_cpsssi_4p_gain_double2ushort( dblVal , pow( 2.0, 10.0 ) );

	return ContecCpsSsiWriteCalibrationGainToUShort( Id, value );

}

unsigned long ContecCpsSsiWriteCalibrationOffsetToUChar( short Id, unsigned char ch, unsigned char cWire3Val, unsigned char cWire4Val )
{
	struct cpsssi_ioctl_arg	arg;

	arg.ch = ch + 1;
	arg.val = ( cWire3Val << 8 )  | cWire4Val;
	ioctl( Id, IOCTL_CPSSSI_WRITE_EEPROM_SSI, &arg);

	return SSI_ERR_SUCCESS;	
}


unsigned long ContecCpsSsiWriteCalibrationOffset( short Id, unsigned char ch, double wire3Value, double wire4Value )
{
	unsigned char cWire3Val = 0;
	unsigned char cWire4Val = 0;

	cWire3Val = _contec_cpsssi_4p_offset_double2uchar( wire3Value , 32.0 );
	cWire4Val = _contec_cpsssi_4p_offset_double2uchar( wire4Value , 32.0 );

	return ContecCpsSsiWriteCalibrationOffsetToUChar( Id, ch, cWire3Val, cWire4Val );
}

unsigned long ContecCpsSsiReadCalibrationGain( short Id, double *dblVal )
{
	int aisel = 0;
	struct cpsssi_ioctl_arg	arg;
	unsigned short value;
	arg.ch = 0;

	ioctl( Id, IOCTL_CPSSSI_READ_EEPROM_SSI, &arg);

	value = arg.val;

	*dblVal = _contec_cpsssi_4p_gain_ushort2double(value ,pow( 2.0, 10.0 ) );

	return SSI_ERR_SUCCESS;	

}

unsigned long ContecCpsSsiReadCalibrationOffset( short Id, unsigned char ch, double *wire3Value, double *wire4Value )
{
	unsigned char tmpVal;
	double *dblVal;
	unsigned int cnt;
	struct cpsssi_ioctl_arg	arg;

	arg.ch = ch + 1;

	ioctl( Id, IOCTL_CPSSSI_READ_EEPROM_SSI, &arg);

	for( cnt = 0; cnt < 2; cnt ++ ){

		switch( cnt ){
		case 0: dblVal = wire4Value; break;
		case 1: dblVal = wire3Value; break;
		}

		if( dblVal == NULL ) continue;

		tmpVal = (arg.val & (0xFF << (8 * cnt) ) ) >> (8 * cnt);
		
		*dblVal = _contec_cpsssi_4p_offset_uchar2double( tmpVal, 32.0 );
	}

	return SSI_ERR_SUCCESS;	

}

unsigned long ContecCpsSsiClearCalibrationData( short Id, int iClear )
{
	int cnt;

	if( iClear & CPSSSI_CALIBRATION_CLEAR_RAM ){
		//all Clear
		ContecCpsSsiSetCalibrationGain( Id, 0.0 );

		for( cnt = 0; cnt < 4 ; cnt ++ ){
			ContecCpsSsiSetCalibrationOffset( Id, cnt , SSI_CHANNEL_3WIRE, 0.0);
			ContecCpsSsiSetCalibrationOffset( Id, cnt , SSI_CHANNEL_4WIRE, 0.0);
		}
	}
	if( iClear & CPSSSI_CALIBRATION_CLEAR_ROM ){
		//FPGA ROM CLEAR
		ioctl( Id, IOCTL_CPSSSI_CLEAR_EEPROM, NULL);
	}

	return SSI_ERR_SUCCESS;	

}

