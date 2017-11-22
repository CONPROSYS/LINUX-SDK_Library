/*
 *  Lib for CONTEC CONPROSYS Digital I/O (CPS-DIO) Series.
 *
 *  Copyright (C) 2015 Syunsuke Okamoto.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "cpsdio.h"
#include "../include/libcpsdio.h"

typedef struct __contec_cps_dio_int_callback__
{
	PCONTEC_CPS_DIO_INT_CALLBACK func;
	CONTEC_CPS_DIO_INT_CALLBACK_DATA data;
}CONTEC_CPS_DIO_INT_CALLBACK_LIST, *PCONTEC_CPS_DIO_INT_CALLBACK_LIST;

CONTEC_CPS_DIO_INT_CALLBACK_LIST contec_cps_dio_cb_list[CPS_DEVICE_MAX_NUM];

void _contec_signal_proc( int signo )
{
	int cnt;
	if( signo == SIGUSR2 ){
		for( cnt = 0; cnt < CPS_DEVICE_MAX_NUM ; cnt ++){
			if( contec_cps_dio_cb_list[cnt].func != (PCONTEC_CPS_DIO_INT_CALLBACK)NULL ){
				
				contec_cps_dio_cb_list[cnt].func(
					contec_cps_dio_cb_list[cnt].data.id,
					DIOM_INTERRUPT,
					contec_cps_dio_cb_list[cnt].data.wParam,
					contec_cps_dio_cb_list[cnt].data.lParam,
					contec_cps_dio_cb_list[cnt].data.Param
				);
			}
		}
	}
}


unsigned long ContecCpsDioInit( char *DeviceName, short *Id )
{
	// open
	char Name[32];

	strcpy(Name, "/dev/");
	strcat(Name, DeviceName);

	*Id = open( Name, O_RDWR );

	if( *Id <= 0 ) return DIO_ERR_DLL_CREATE_FILE;

	return DIO_ERR_SUCCESS;

}

unsigned long ContecCpsDioExit( short Id )
{
	// close
	close( Id );
	return DIO_ERR_SUCCESS;

}
unsigned long ContecCpsDioGetErrorStrings( unsigned long code, char *Str )
{

	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioQueryDeviceName( short Id, char *DeviceName, char *Device )
{

	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioGetMaxPort( short Id, short *InPortNum, short *OutPortNum )
{

	struct cpsdio_ioctl_arg	arg;

	ioctl( Id, IOCTL_CPSDIO_GET_INP_PORTNUM, &arg );

	*InPortNum = (short)( arg.val );

	ioctl( Id, IOCTL_CPSDIO_GET_OUTP_PORTNUM, &arg );

	*OutPortNum = (short)( arg.val );

	return DIO_ERR_SUCCESS;
}


/**** Single Functions ****/
unsigned long ContecCpsDioInpByte( short Id, short Num, unsigned char *Data)
{
	struct cpsdio_ioctl_arg	arg;

	arg.port = Num;

	ioctl( Id, IOCTL_CPSDIO_INP_PORT, &arg );

	*Data = ( arg.val );

	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioInpBit( short Id, short Num, unsigned char *Data )
{

	struct cpsdio_ioctl_arg	arg;

	arg.port = Num / 8;

	ioctl( Id, IOCTL_CPSDIO_INP_PORT, &arg );

	*Data = ( arg.val  & (1 << (Num % 8) ) ) >> (1 << (Num % 8) );

	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioOutByte( short Id, short Num, unsigned char Data)
{


	struct cpsdio_ioctl_arg	arg;

	arg.port = Num;
	arg.val = Data;

	ioctl( Id, IOCTL_CPSDIO_OUT_PORT , &arg );

	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioOutBit( short Id, short Num, unsigned char Data )
{

	struct cpsdio_ioctl_arg	arg;

	ioctl( Id, IOCTL_CPSDIO_INP_PORT , &arg );	
	arg.port = Num / 8;
	arg.val = arg.val ^ (  ~(1 << (Num % 8) ) ) | ( Data << (Num % 8 ) );

	ioctl( Id, IOCTL_CPSDIO_OUT_PORT , &arg );

	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioEchoBackByte( short Id, short Num, unsigned char *Data)
{
	struct cpsdio_ioctl_arg	arg;

	arg.port = Num;

	ioctl( Id, IOCTL_CPSDIO_OUT_PORT_ECHO , &arg );	
		
	*Data = arg.val;


	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioEchoBackBit( short Id, short Num, unsigned char *Data)
{

	struct cpsdio_ioctl_arg	arg;

	arg.port = Num / 8;
		
	ioctl( Id, IOCTL_CPSDIO_OUT_PORT_ECHO, &arg );

	*Data = ( arg.val  & (1 << (Num % 8) ) ) >> (1 << (Num % 8) );

	return DIO_ERR_SUCCESS;
}


/**** Multi Functions ****/
unsigned long ContecCpsDioInpMultiByte( short Id, short Ports[], short PortsDimensionNum, unsigned char Data[] )
{
	short count;

	for( count = 0; count < PortsDimensionNum; count ++ )
	{		
		ContecCpsDioInpByte( Id, Ports[count], &Data[count] );
	}

	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioInpMultiBit( short Id, short Bits[],short BitsDimensionNum, char Data[])
{

	short count;

	for( count = 0; count < BitsDimensionNum; count ++ )
	{		
		ContecCpsDioInpBit( Id, Bits[count], &Data[count] );
	}

	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioOutMultiByte( short Id, short Ports[], short PortsDimensionNum, unsigned char Data[] )
{
	short count;

	for( count = 0; count < PortsDimensionNum; count ++ )
	{		
		ContecCpsDioOutByte( Id, Ports[count], Data[count] );
	}
	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioOutMultiBit( short Id, short Bits[],short BitsDimensionNum, char Data[])
{
	short count;

	for( count = 0; count < BitsDimensionNum; count ++ )
	{		
		ContecCpsDioOutBit( Id, Bits[count], Data[count] );
	}
	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioEchoBackMultiByte( short Id, short Ports[], short PortsDimensionNum, unsigned char Data[] )
{
	short count;

	for( count = 0; count < PortsDimensionNum; count ++ )
	{		
		ContecCpsDioEchoBackByte( Id, Ports[count], &Data[count] );
	}
	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioEchoBackMultiBit( short Id, short Bits[],short BitsDimensionNum, char Data[])
{
	short count;

	for( count = 0; count < BitsDimensionNum; count ++ )
	{		
		ContecCpsDioEchoBackBit( Id, Bits[count], &Data[count] );
	}

	return DIO_ERR_SUCCESS;
}


/**** Digital Filter Functions ****/
unsigned long ContecCpsDioSetDigitalFilter( short Id, unsigned char FilterValue )
{
	struct cpsdio_ioctl_arg	arg;
	
	arg.val = (FilterValue & 0x3F);

	ioctl( Id, IOCTL_CPSDIO_SET_FILTER	 , &arg );

	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioGetDigitalFilter( short Id, unsigned char *FilterValue )
{
	struct cpsdio_ioctl_arg	arg;

	ioctl( Id, IOCTL_CPSDIO_GET_FILTER, &arg );

	*FilterValue = ( arg.val );

	return DIO_ERR_SUCCESS;
}
/**** Interrupt Event Functions ****/
unsigned long ContecCpsDioNotifyInterrupt( short Id, short BitNum, short Logic )
{
	struct cpsdio_ioctl_arg	arg;

	/**** Mask Set ****/
	arg.val = ~(1 << BitNum);

	ioctl( Id, IOCTL_CPSDIO_SET_INT_MASK, &arg );

	/**** Egde Set ****/
	arg.val = (Logic << BitNum);

	ioctl( Id, IOCTL_CPSDIO_SET_INT_EGDE, &arg );

	/****  process_id Set ****/

	arg.val = getpid();

	ioctl( Id, IOCTL_CPSDIO_SET_CALLBACK_PROCESS, &arg );

	return DIO_ERR_SUCCESS;
}

unsigned long ContecCpsDioSetInterruptCallBackProc( short Id, PCONTEC_CPS_DIO_INT_CALLBACK cb, void* Param )
{
	struct cpsdio_ioctl_arg	arg;

	/*** signal ***/
	signal( SIGUSR2, _contec_signal_proc );

	return DIO_ERR_SUCCESS;
}

