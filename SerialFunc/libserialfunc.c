/*!
 *  Lib for Serial Port Communication Functions.
 *
 *  Copyright (C) 2015 Syunsuke Okamoto, CONTEC.CO.,Ltd.
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

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include "serialfunc.h"

#define LIB_SERIAL_VERSION	"1.0.7"

static struct termios oldtio; //!< 現在のシリアルポートの設定を格納

/*!
 @~English
 @name DebugPrint macro
 @~Japanese
 @name デバッグ用表示マクロ
*/
/// @{

#if 0
#define DbgPrint(fmt...)	printf(fmt)
#else
#define DbgPrint(fmt...)	do { } while (0)
#endif

/// @}

//////////////////////////////////////////////////////////////////////////////
/// \brief   シリアルポートを半二重通信でオープンする関数
///
/// \return  オープンしたシリアルポートへのポインタ
/// \param   *AsDev  オープンするシリアルデバイス /dev/ttyS?
/// \param   AlSpeed      シリアルポートの速度 2400,4800,9600,19200,38400,57600,115200, 460800, 921600
/// \param   AiLength     シリアルポートのデータ長 7,8
/// \param   AiStop       シリアルポートのストップビット 0,1,2
/// \param   AiParity     シリアルポートのパリティ 0(n),1(e),2(o)
/// \param   AiWait       シリアルポートの受信待ち時間
/// \param   AiBlockMode   シリアルポートのオープン時のブロッキング(0:無効 1:有効)
////////////////////////////////////////////////////////////////////////////////
int Serial_PortOpen_Half( char *AsDev, long AlSpeed, int AiLength, int AiStop, int AiParity , int AiWait, int AiBlockMode)
{
	static int iPort;

	iPort = Serial_PortOpen_Func(AsDev, AlSpeed, AiLength, AiStop, AiParity, AiWait, AiBlockMode, 0);

	ioctl( iPort, TIOCSRS485, 1  ); // rs485 enable

	return iPort;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   シリアルポートをオープンする関数
///
/// \return  オープンしたシリアルポートへのポインタ
/// \param   *AsDev  オープンするシリアルデバイス /dev/ttyS?
/// \param   AlSpeed      シリアルポートの速度 2400,4800,9600,19200,38400,57600,115200,460800,921600
/// \param   AiLength     シリアルポートのデータ長 7,8
/// \param   AiStop       シリアルポートのストップビット 0,1,2
/// \param   AiParity     シリアルポートのパリティ 0(n),1(e),2(o)
/// \param   AiWait       シリアルポートの受信待ち時間
/// \param   AiOpenMode   シリアルポートのオープン時のブロッキング(0:無効 1:有効)
/// \param   AiFlow       シリアルポートのオープン時のフロー制御 ( 0:なし, 1:RTS/CTS, 2:DTR/DSR )
//////////////////////////////////////////////////////////////////////////////
int Serial_PortOpen_Func( char *AsDev, long AlSpeed, int AiLength, int AiStop, int AiParity ,int AiWait, int AiOpenMode, int AiFlow ){
	static struct termios newtio;
	static int iPort;
	static int iOpenMode;

	switch( AiOpenMode ){
		case 0: iOpenMode = O_RDWR | O_NOCTTY ; break;
		case 1: iOpenMode = O_RDWR | O_NOCTTY | O_NONBLOCK ; break;
	}

	/* 読み書きの為にモデムデバイスをオープンする。ノイズによってCTRL-Cが
		たまたま発生しても接続が切れないようにtty制御はしない */
	iPort = open( AsDev, iOpenMode );
	if( iPort < 0 ){
		perror( AsDev );
		return -1;
	}
	// 現在のシリアルポートの設定を保存(Close時に戻す為)
	tcgetattr( iPort, &oldtio );

	Serial_PortSetParameter(iPort, AlSpeed, AiLength, AiStop, AiParity, AiWait, AiFlow);

	return iPort;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   シリアルポートのパラメータを設定する関数
///
/// \param   AiPort       シリアルポートのファイルディスクリプタ
/// \param   AiSpeed      シリアルポートの速度 2400,4800,9600,19200,38400,57600,115200,460800,921600
/// \param   AiLength     シリアルポートのデータ長 7,8
/// \param   AiStop       シリアルポートのストップビット 0,1,2
/// \param   AiParity     シリアルポートのパリティ 0(n),1(e),2(o)
/// \param   AiWait       シリアルポートの受信待ち時間
//////////////////////////////////////////////////////////////////////////////
void Serial_PortSetParameter(int AiPort, int AiSpeed, int AiLength, int AiStop, int AiParity, int AiWait, int AiFlow)
{
	static struct termios newtio;

	// 制御コードの初期化を行う
	newtio.c_iflag = 0;
	newtio.c_oflag = 0;
	newtio.c_cflag = 0;
	newtio.c_lflag = 0;
	newtio.c_line = 0;
	bzero( newtio.c_cc, sizeof(newtio.c_cc) );

	///// c_cflagの設定 /////
	/* Setting for c_cflag
		B921600～B2400 : 通信速度
		CS5～CS8 : データビット長
		CSTOPB   : ストップビット長=2(付けなければ1)
		CPARENB  : パリティ有効(このままでは偶数)
		CPARODD  : パリティを奇数にする
		CLOCAL   : モデムの制御線を無視する
		CREAD    : 受信文字を有効にする
		CRTSCTS  : 出力のハードウェアフロー制御を有効にする
		HUPCL    : 最後のプロセスがクローズした後、モデムの制御線をLOWにする
	*/
	// 通信速度のセット
	switch( AiSpeed ){
		case 921600:
			newtio.c_cflag = B921600;
			break;
		case 460800:
			newtio.c_cflag = B460800;
			break;
		case 115200:
			newtio.c_cflag = B115200;
			break;
		case 57600:
			newtio.c_cflag = B57600;
			break;
		case 38400:
			newtio.c_cflag = B38400;
			break;
		case 19200:
			newtio.c_cflag = B19200;
			break;
		case 9600:
			newtio.c_cflag = B9600;
			break;
		case 4800:
			newtio.c_cflag = B4800;
			break;
		case 2400:
			newtio.c_cflag = B2400;
			break;
		default:
			newtio.c_cflag = B9600; // デフォルトは9600bps
			break;
	}
	// データビット長の設定
	switch( AiLength ){
		case 7:
			// CS7  : データ長を7ビットにする
			newtio.c_cflag = newtio.c_cflag | CS7 ;
			break;
		default:
			// CS8  : デフォルトではデータ長を8ビットにする
			newtio.c_cflag = newtio.c_cflag | CS8 ;
			break;
	}
	// ストップビットの設定
	switch( AiStop ){
		case 2:
			// CSTOPB   : ストップビットを2にする
			newtio.c_cflag = newtio.c_cflag | CSTOPB ; 
			break;
		default:
			// デフォルトでは1
			newtio.c_cflag = newtio.c_cflag & ~CSTOPB ;
			break;
	}
	// パリティのセット
	switch( AiParity ){
		case 1:
			// PARENB  : パリティを有効にする(標準では偶数)
			newtio.c_cflag = newtio.c_cflag | PARENB ;
			break;
		case 2:
			// PARENB  : パリティを有効にし奇数をセット
			newtio.c_cflag = newtio.c_cflag | PARENB | PARODD ;
			break;
		default:
			// NO PARITY
			break;
	}

	// Ver 1.0.3 Flow Control Added 
	// Setting of hardware flow 
	switch( AiFlow ){
		case 1:
			// CRTSCTS  : enables hardware flow
			newtio.c_cflag = newtio.c_cflag | CRTSCTS;
			break;
		case 0:	
		default:
			// no hardware flow
			break;
	}
	// Ver 1.0.3 End
	
//	newtio.c_cflag = newtio.c_cflag | CLOCAL | CREAD;
	newtio.c_cflag = newtio.c_cflag | CLOCAL | CREAD;

	///// c_iflagの設定 /////
//	newtio.c_iflag = IGNPAR; // IGNPAR : パリティエラーのデータは無視
	newtio.c_iflag = IGNPAR | IGNBRK; // IGNBRK : ブレーク信号は無視

	///// c_oflagの設定 /////
	newtio.c_oflag = 0;     // 0:Rawモードでの出力

	///// c_lflagの設定 /////
	newtio.c_lflag = 0;  // Set input mode (non-canonical,no echo,....)
	/*　ICANON : カノニカル入力を有効にする */
	newtio.c_cc[VTIME] = AiWait / 100; // 0:キャラクタタイマ ( AiWait(msec) / 100)
	newtio.c_cc[VMIN] = 0;  // 指定文字来るまで読み込みをブロック(0:しない 1:する)

	///// モデムラインをクリア /////
	tcflush( AiPort, TCIFLUSH );
// change start 2004/08/25 tkasuya,contec
//	// 新しい設定を適用する (TCSANOW：ただちに変更が有効となる)
//	tcsetattr( AiPort, TCSANOW, &newtio );
	// 新しい設定を適用する (TCSADRAIN：変更を出力がフラッシュされた後に反映)
	tcsetattr( AiPort, TCSADRAIN, &newtio );
// change end

}

//////////////////////////////////////////////////////////////////////////////
/// \brief   シリアルポートを閉じる関数
///
/// \return  void
/// \param   AiPort  シリアルポート記述子
//////////////////////////////////////////////////////////////////////////////
void Serial_PortClose( int AiPort )
{
	// シリアルポートの設定をポートオープン前に戻す
// change start 2004/08/25 tkasuya,contec
//	tcsetattr( AiPort, TCSANOW, &oldtio );
//	ioctl(AiPort, TIOCSRS485, 0); // rs485 enable
	tcsetattr( AiPort, TCSADRAIN, &oldtio );
// change end
	close(AiPort);
}

//////////////////////////////////////////////////////////////////////////////
/// \brief シリアルポートへ1バイトを書き込む関数
///
/// \return  出力結果 0 … 成功, -1 … 失敗
/// \param   AiPort  シリアルポート記述子
/// \param   AcChar  出力データ
//////////////////////////////////////////////////////////////////////////////
int Serial_PutChar( int AiPort, unsigned char AcChar )
{
	if( write( AiPort, &AcChar, 1 ) != 1 ){
		return -1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   シリアルポートから1文字読み込む関数
///
/// \return  受信データ(1バイト)
/// \param   AiPort     シリアルポート記述子
//////////////////////////////////////////////////////////////////////////////
unsigned char Serial_GetChar( int AiPort )
{
	static unsigned char cRet = 0xFF;
	static int iRet;

	iRet = read( AiPort, (char *)&cRet, 1 );
	return cRet;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief   シリアルポートから文字列を読込む関数
///
/// \return  読込み完了バイト数
/// \param   AiPort     シリアルポート記述子
/// \param   *AsBuffer  読込んだ文字列を格納するバッファへのポインタ
/// \param   AiLen       一度に読込むバイト数
//////////////////////////////////////////////////////////////////////////////
int Serial_GetString( int AiPort, unsigned char *AsBuffer, int AiLen )
{
	static int iRet = 0;
	struct serial_icounter_struct icount;

ioctl( AiPort, TIOCGICOUNT, &icount);
//printf("rx[%d] tx[%d] Error:frame[%d] overrun[%d] parity[%d] boverrun[%d] \n", icount.rx, icount.tx, icount.frame, icount.overrun, icount.parity, icount.buf_overrun);

	iRet = read( AiPort, (char *)AsBuffer, AiLen );

	return iRet;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   シリアルポートへ多バイトのデータを書き込む関数
///
/// \return  出力結果 0 … 成功, -1 … 失敗
/// \param   AiPort     シリアルポート記述子
/// \param   *AsBuffer  送信文字列へのポインタ
/// \param   AiLen      書込みバイト数
//////////////////////////////////////////////////////////////////////////////
int Serial_PutString( int AiPort, unsigned char *AsBuffer, int AiLen )
{
	//if( write( AiPort, AsBuffer, AiLen ) != 1 ){
	//	return -1;
	//}
	
	//return 0;
	DbgPrint("<Serial PutString AsBuf %s , Len : %d \n", AsBuffer, AiLen );

	return write( AiPort, (char *)AsBuffer, AiLen );
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   サムチェックを計算する関数
///
/// \return  サムチェック計算値
/// \param   *AsBuffer    サムチェック対象文字列
/// \param   AiLen        サムチェック対象文字数
/// \param   AiComplement サムチェックに2の補数を適用 1…適用,他…非適用
//////////////////////////////////////////////////////////////////////////////
int Serial_SumCheck( char *AsBuffer, int AiLen, int AiComplement ){
	int iRet = 0;
	int i = 0;

	for( i = 0 ; i < AiLen ; i++ ){
		iRet = iRet + AsBuffer[i];
	}

	if( AiComplement == 1 ){
		iRet ^= 0xff;
		iRet = iRet + 1;
	}
	iRet = iRet % 0x100;
	return iRet;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief   RTSに値をセットする関数
///
/// \return  void
/// \param   AiPort   シリアルポート記述子
/// \param   AiValue  セット値(0:OFF , 1:ON)
//////////////////////////////////////////////////////////////////////////////
void Serial_Set_Rts( int AiPort, int AiValue ){
        int a;
        int ioctl_ret;

        ioctl_ret=ioctl( AiPort, TIOCMGET, &a );

        // printf( "ret=(%d) a=[%x]\n", ioctl_ret, a );

        a &= ~TIOCM_RTS;
        if( AiValue ){
                a |= TIOCM_RTS;
        }
        ioctl( AiPort, TIOCMSET, &a);
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   DTRに値をセットする関数
///
/// \return  void
/// \param   AiPort   シリアルポート記述子
/// \param   AiValue  セット値(0:OFF , 1:ON)
//////////////////////////////////////////////////////////////////////////////
void Serial_Set_Dtr( int AiPort, int AiValue ){
        int a;

        ioctl( AiPort, TIOCMGET, &a );
        a &= ~TIOCM_DTR;
        if( AiValue ){
                a |= TIOCM_DTR;
        }
        ioctl( AiPort, TIOCMSET, &a );
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   LSRの値を取得する関数
///
/// \return  void
/// \param   AiPort   シリアルポート記述子
/// \param   AiValue  Lsrの値
//////////////////////////////////////////////////////////////////////////////
void Serial_Get_Lsr( int AiPort, int *AiValue ){
	int lsr;

	ioctl( AiPort, TIOCSERGETLSR, &lsr);

	if( lsr & LSR_FE )
		printf(" Framing Error!\n");
	if( lsr & LSR_OE )
		printf(" Rx Overrun Error!\n");
	if( lsr & LSR_PE )
		printf(" Parity Error!\n");

	*AiValue = lsr;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   RIの値を取得する関数
///
/// \return  int	取得値(0:OFF , 1:ON)
/// \param   AiPort   シリアルポート記述子
//////////////////////////////////////////////////////////////////////////////
int Serial_Get_Ri( int AiPort ){

	int a;

	ioctl( AiPort, TIOCMGET, &a );

	if( a & TIOCM_RI )
		return 1;
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   DCDの値を取得する関数
///
/// \return  int 取得値(0:OFF , 1:ON)
/// \param   AiPort   シリアルポート記述子
//////////////////////////////////////////////////////////////////////////////
int Serial_Get_Dcd( int AiPort ){

	int a;

	ioctl( AiPort, TIOCMGET, &a );

	if( a & TIOCM_CAR )
		return 1;
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   DSRの値を取得する関数
///
/// \return  int 取得値(0:OFF , 1:ON)
/// \param   AiPort   シリアルポート記述子
//////////////////////////////////////////////////////////////////////////////
int Serial_Get_Dsr( int AiPort ){

	int a;

	ioctl( AiPort, TIOCMGET, &a );

	if( a & TIOCM_DSR )
		return 1;
	else
		return 0;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   シリアルポートの入力バッファを取得します
///
/// \return  void
/// \param   AiPort   シリアルポート記述子
/// \param   AiValue  入力バッファの値
//////////////////////////////////////////////////////////////////////////////
void Serial_Get_In_Buffer( int AiPort, int *AiValue ){

	ioctl( AiPort, FIONREAD, AiValue);

}

//////////////////////////////////////////////////////////////////////////////
/// \brief   シリアルポートの出力バッファを取得します
///
/// \return  void
/// \param   AiPort   シリアルポート記述子
/// \param   AiValue  出力バッファの値
//////////////////////////////////////////////////////////////////////////////
void Serial_Get_Out_Buffer( int AiPort, int *AiValue ){

	ioctl( AiPort, TIOCOUTQ, AiValue);

}

//////////////////////////////////////////////////////////////////////////////
/// \brief   シリアルライブラリのバージョンを取得します
///
/// \return  void
/// \param   libVer   ライブラリのバージョン
//////////////////////////////////////////////////////////////////////////////
void Serial_Get_Lib_Version( char *libVer ){

	strcpy(libVer,LIB_SERIAL_VERSION);

}
