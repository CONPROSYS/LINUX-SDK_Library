/*
 *  Lib for Serial Port Communication Functions.
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

#include<stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include "serialfunc.h"

static struct termios oldtio; //!< ���݂̃V���A���|�[�g�̐ݒ���i�[

//////////////////////////////////////////////////////////////////////////////
/// \brief   �V���A���|�[�g���I�[�v������֐�
///
/// \return  �I�[�v�������V���A���|�[�g�ւ̃|�C���^
/// \param   *AsDev  �I�[�v������V���A���f�o�C�X /dev/ttyS?
/// \param   AlSpeed      �V���A���|�[�g�̑��x 2400,4800,9600,19200,38400,57600,115200
/// \param   AiLength     �V���A���|�[�g�̃f�[�^�� 7,8
/// \param   AiStop       �V���A���|�[�g�̃X�g�b�v�r�b�g 0,1,2
/// \param   AiParity     �V���A���|�[�g�̃p���e�B 0(n),1(e),2(o)
/// \param   AiWait       �V���A���|�[�g�̎�M�҂�����
/// \param   AiBlockMode   �V���A���|�[�g�̃I�[�v�����̃u���b�L���O(0:���� 1:�L��)
////////////////////////////////////////////////////////////////////////////////
int Serial_PortOpen_Half( char *AsDev, long AlSpeed, int AiLength, int AiStop, int AiParity , int AiWait, int AiBlockMode)
{
	static int iPort;

	iPort = Serial_PortOpen_Func(AsDev, AlSpeed, AiLength, AiStop, AiParity, AiWait, AiBlockMode);

	ioctl( iPort, TIOCSRS485, 1  ); // rs485 enable

	return iPort;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   �V���A���|�[�g���I�[�v������֐�
///
/// \return  �I�[�v�������V���A���|�[�g�ւ̃|�C���^
/// \param   *AsDev  �I�[�v������V���A���f�o�C�X /dev/ttyS?
/// \param   AlSpeed      �V���A���|�[�g�̑��x 2400,4800,9600,19200,38400,57600,115200
/// \param   AiLength     �V���A���|�[�g�̃f�[�^�� 7,8
/// \param   AiStop       �V���A���|�[�g�̃X�g�b�v�r�b�g 0,1,2
/// \param   AiParity     �V���A���|�[�g�̃p���e�B 0(n),1(e),2(o)
//////////////////////////////////////////////////////////////////////////////
int Serial_PortOpen_Func( char *AsDev, long AlSpeed, int AiLength, int AiStop, int AiParity ,int AiWait, int AiOpenMode ){
	static struct termios newtio;
	static int iPort;
	static int iOpenMode;

	switch( AiOpenMode ){
		case 0: iOpenMode = O_RDWR | O_NOCTTY ; break;
		case 1: iOpenMode = O_RDWR | O_NOCTTY | O_NONBLOCK ; break;
	}

	/* �ǂݏ����ׂ̈Ƀ��f���f�o�C�X���I�[�v������B�m�C�Y�ɂ����CTRL-C��
		���܂��ܔ������Ă��ڑ����؂�Ȃ��悤��tty����͂��Ȃ� */
	iPort = open( AsDev, iOpenMode );
	if( iPort < 0 ){
		perror( AsDev );
		return -1;
	}
	// ���݂̃V���A���|�[�g�̐ݒ��ۑ�(Close���ɖ߂���)
	tcgetattr( iPort, &oldtio );

	Serial_PortSetParameter(iPort, AlSpeed, AiLength, AiStop, AiParity, AiWait);

	return iPort;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   �V���A���|�[�g���I�[�v������֐�
///
/// \return  �I�[�v�������V���A���|�[�g�ւ̃|�C���^
/// \param   AiPort       �V���A���|�[�g�L�q�q
/// \param   AiSpeed      �V���A���|�[�g�̑��x 2400,4800,9600,19200,38400,57600,115200
/// \param   AiLength     �V���A���|�[�g�̃f�[�^�� 7,8
/// \param   AiStop       �V���A���|�[�g�̃X�g�b�v�r�b�g 0,1,2
/// \param   AiParity     �V���A���|�[�g�̃p���e�B 0(n),1(e),2(o)
//////////////////////////////////////////////////////////////////////////////
void Serial_PortSetParameter(int AiPort, int AiSpeed, int AiLength, int AiStop, int AiParity, int AiWait)
{
	static struct termios newtio;

	// ����R�[�h�̏��������s��
	newtio.c_iflag = 0;
	newtio.c_oflag = 0;
	newtio.c_cflag = 0;
	newtio.c_lflag = 0;
	newtio.c_line = 0;
	bzero( newtio.c_cc, sizeof(newtio.c_cc) );

	///// c_cflag�̐ݒ� /////
	/* Setting for c_cflag
		B115200�`B2400 : �ʐM���x
		CS5�`CS8 : �f�[�^�r�b�g��
		CSTOPB   : �X�g�b�v�r�b�g��=2(�t���Ȃ����1)
		CPARENB  : �p���e�B�L��(���̂܂܂ł͋���)
		CPARODD  : �p���e�B����ɂ���
		CLOCAL   : ���f���̐�����𖳎�����
		CREAD    : ��M������L���ɂ���
		CRTSCTS  : �o�͂̃n�[�h�E�F�A�t���[�����L���ɂ���
		HUPCL    : �Ō�̃v���Z�X���N���[�Y������A���f���̐������LOW�ɂ���
	*/
	// �ʐM���x�̃Z�b�g
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
			newtio.c_cflag = B9600; // �f�t�H���g��9600bps
			break;
	}
	// �f�[�^�r�b�g���̐ݒ�
	switch( AiLength ){
		case 7:
			// CS7  : �f�[�^����7�r�b�g�ɂ���
			newtio.c_cflag = newtio.c_cflag | CS7 ;
			break;
		default:
			// CS8  : �f�t�H���g�ł̓f�[�^����8�r�b�g�ɂ���
			newtio.c_cflag = newtio.c_cflag | CS8 ;
			break;
	}
	// �X�g�b�v�r�b�g�̐ݒ�
	switch( AiStop ){
		case 2:
			// CSTOPB   : �X�g�b�v�r�b�g��2�ɂ���
			newtio.c_cflag = newtio.c_cflag | CSTOPB ; 
			break;
		default:
			// �f�t�H���g�ł�1
			newtio.c_cflag = newtio.c_cflag & ~CSTOPB ;
			break;
	}
	// �p���e�B�̃Z�b�g
	switch( AiParity ){
		case 1:
			// PARENB  : �p���e�B��L���ɂ���(�W���ł͋���)
			newtio.c_cflag = newtio.c_cflag | PARENB ;
			break;
		case 2:
			// PARENB  : �p���e�B��L���ɂ�����Z�b�g
			newtio.c_cflag = newtio.c_cflag | PARENB | PARODD ;
			break;
	}
//	newtio.c_cflag = newtio.c_cflag | CLOCAL | CREAD;
	newtio.c_cflag = newtio.c_cflag | CLOCAL | CRTSCTS | CREAD;

	///// c_iflag�̐ݒ� /////
//	newtio.c_iflag = IGNPAR; // IGNPAR : �p���e�B�G���[�̃f�[�^�͖���
	newtio.c_iflag = IGNPAR | IGNBRK; // IGNPAR : �p���e�B�G���[�̃f�[�^�͖���

	///// c_oflag�̐ݒ� /////
	newtio.c_oflag = 0;     // 0:Raw���[�h�ł̏o��

	///// c_lflag�̐ݒ� /////
	newtio.c_lflag = 0;  // Set input mode (non-canonical,no echo,....)
	/*�@ICANON : �J�m�j�J�����͂�L���ɂ��� */
	newtio.c_cc[VTIME] = AiWait * 2; // 0:�L�����N�^�^�C�}
	newtio.c_cc[VMIN] = 0;  // �w�蕶������܂œǂݍ��݂��u���b�N(0:���Ȃ� 1:����)

	///// ���f�����C�����N���A /////
	tcflush( AiPort, TCIFLUSH );
// change start 2004/08/25 tkasuya,contec
//	// �V�����ݒ��K�p���� (TCSANOW�F�������ɕύX���L���ƂȂ�)
//	tcsetattr( iPort, TCSANOW, &newtio );
	// �V�����ݒ��K�p���� (TCSADRAIN�F�ύX���o�͂��t���b�V�����ꂽ��ɔ��f)
	tcsetattr( AiPort, TCSADRAIN, &newtio );
// change end

}

//////////////////////////////////////////////////////////////////////////////
/// \brief   �V���A���|�[�g�����֐�
///
/// \return  void
/// \param   Ai_Port  �V���A���|�[�g�L�q�q
//////////////////////////////////////////////////////////////////////////////
void Serial_PortClose( int AiPort )
{
	// �V���A���|�[�g�̐ݒ���|�[�g�I�[�v���O�ɖ߂�
// change start 2004/08/25 tkasuya,contec
//	tcsetattr( AiPort, TCSANOW, &oldtio );
//	ioctl(AiPort, TIOCSRS485, 0); // rs485 enable
	tcsetattr( AiPort, TCSADRAIN, &oldtio );
// change end
	close(AiPort);
}

//////////////////////////////////////////////////////////////////////////////
/// \brief �V���A���|�[�g��1�o�C�g���������ފ֐�
///
/// \return  �o�͌��� 0 �c ����, -1 �c ���s
/// \param   Ai_Port  �V���A���|�[�g�L�q�q
/// \param   Ac_Char  �o�̓f�[�^
//////////////////////////////////////////////////////////////////////////////
int Serial_PutChar( int AiPort, unsigned char AcChar )
{
	if( write( AiPort, &AcChar, 1 ) != 1 ){
		return -1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   �V���A���|�[�g����1�����ǂݍ��ފ֐�
///
/// \return  ��M�f�[�^(1�o�C�g)
/// \param   Ai_Port     �V���A���|�[�g�L�q�q
//////////////////////////////////////////////////////////////////////////////
int Serial_GetChar( int AiPort )
{
	static unsigned char cRet;
	static int iRet;

	iRet = read( AiPort, (char *)&cRet, 1 );
	return cRet;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief   �V���A���|�[�g���當�����Ǎ��ފ֐�
///
/// \return  �Ǎ��݊����o�C�g��
/// \param   Ai_Port     �V���A���|�[�g�L�q�q
/// \param   *As_Buffer  �Ǎ��񂾕�������i�[����o�b�t�@�ւ̃|�C���^
/// \param   Ai_Len       ��x�ɓǍ��ރo�C�g��
//////////////////////////////////////////////////////////////////////////////
int Serial_GetString( int AiPort, char *AsBuffer, int AiLen )
{
	static int iRet = 0;
	struct serial_icounter_struct icount;

ioctl( AiPort, TIOCGICOUNT, &icount);
//printf("rx[%d] tx[%d] Error:frame[%d] overrun[%d] parity[%d] boverrun[%d] \n", icount.rx, icount.tx, icount.frame, icount.overrun, icount.parity, icount.buf_overrun);

	iRet = read( AiPort, AsBuffer, AiLen );

	return iRet;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief   �V���A���|�[�g�֑��o�C�g�̃f�[�^���������ފ֐�
///
/// \return  �o�͌��� 0 �c ����, -1 �c ���s
/// \param   Ai_Port     �V���A���|�[�g�L�q�q
/// \param   *As_Buffer  ���M������ւ̃|�C���^
/// \param   Ai_Len      �����݃o�C�g��
//////////////////////////////////////////////////////////////////////////////
int Serial_PutString( int AiPort, char *AsBuffer, int AiLen )
{
	//if( write( AiPort, AsBuffer, AiLen ) != 1 ){
	//	return -1;
	//}
	
	//return 0;

	return write( AiPort, AsBuffer, AiLen );
}
