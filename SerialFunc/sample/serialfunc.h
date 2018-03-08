/*
 * serialunc.h
 *
 *  Created on: 2016/01/03
 *      Author: contec
 */

#ifndef _SERIALFUNC_H_
#define _SERIALFUNC_H_

extern int Serial_PortOpen_Half( char *AsDev, long AlSpeed, int AiLength, int AiStop, int AiParity , int AiWait, int AiBlockMode);
extern int Serial_PortOpen_Func( char *AsDev, long AlSpeed, int AiLength, int AiStop, int AiParity ,int AiWait, int AiOpenMode, int AiFlow);
extern void Serial_PortSetParameter(int AiPort, int AiSpeed, int AiLength, int AiStop, int AiParity, int AiWait, int AiFlow);
extern void Serial_PortClose( int AiPort );
extern int Serial_PutChar( int AiPort, unsigned char AcChar );
extern unsigned char Serial_GetChar( int AiPort );
extern int Serial_GetString( int AiPort, unsigned char *AsBuffer, int AiLen );
extern int Serial_PutString( int AiPort, unsigned char *AsBuffer, int AiLen );
extern int Serial_SumCheck( char *AsBuffer, int AiLen, int AiComplement );
extern void Serial_Set_Rts( int AiPort, int AiValue );
extern void Serial_Set_Dtr( int AiPort, int AiValue );
extern void Serial_Get_Lsr( int AiPort, int *AiValue );
extern int Serial_Get_Ri( int AiPort );
extern int Serial_Get_Dcd( int AiPort );
extern int Serial_Get_Dsr( int AiPort );

extern void Serial_Get_In_Buffer( int AiPort, int *AiValue );
extern void Serial_Get_Out_Buffer( int AiPort, int *AiValue );

#define Serial_PortOpen_Full( AsDev, AlSpeed, AiLength, AiStop, AiParity, AiWait, AiBlockMode, AiFlow) \
	Serial_PortOpen_Func( AsDev, AlSpeed, AiLength, AiStop, AiParity, AiWait, AiBlockMode, AiFlow)

#define Serial_PortOpen( AsDev, AlSpeed, AiLength, AiStop, AiParity, AiWait, AiBlockMode) \
	Serial_PortOpen_Half( AsDev, AlSpeed, AiLength, AiStop, AiParity, AiWait, AiBlockMode)

#ifndef TIOCSRS485
#define TIOCSRS485      0x542F
#endif

#define LSR_OE	0x02
#define LSR_PE	0x04
#define LSR_FE	0x08

#endif /* SERIALUNC_H_ */
