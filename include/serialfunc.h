/*
 * serialunc.h
 *
 *  Created on: 2016/01/03
 *      Author: contec
 */

#ifndef _SERIALFUNC_H_
#define _SERIALFUNC_H_

extern int Serial_PortOpen_Half( char *AsDev, long AlSpeed, int AiLength, int AiStop, int AiParity , int AiWait, int AiBlockMode);
extern int Serial_PortOpen_Func( char *AsDev, long AlSpeed, int AiLength, int AiStop, int AiParity ,int AiWait, int AiOpenMode );
extern void Serial_PortSetParameter(int AiPort, int AiSpeed, int AiLength, int AiStop, int AiParity, int AiWait);
extern void Serial_PortClose( int AiPort );
extern int Serial_PutChar( int AiPort, unsigned char AcChar );
extern int Serial_GetChar( int AiPort );
extern int Serial_GetString( int AiPort, char *AsBuffer, int AiLen );
extern int Serial_PutString( int AiPort, char *AsBuffer, int AiLen );



#define Serial_PortOpen_Full( AsDev, AlSpeed, AiLength, AiStop, AiParity, AiWait, AiBlockMode) \
	Serial_PortOpen_Func( AsDev, AlSpeed, AiLength, AiStop, AiParity, AiWait, AiBlockMode)

#define Serial_PortOpen( AsDev, AlSpeed, AiLength, AiStop, AiParity, AiWait, AiBlockMode) \
	Serial_PortOpen_Func( AsDev, AlSpeed, AiLength, AiStop, AiParity, AiWait, AiBlockMode)


#endif /* SERIALUNC_H_ */
