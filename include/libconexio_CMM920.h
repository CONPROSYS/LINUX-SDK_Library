/***

	libconexio_CMM920_func.c - conexio_CMM920_functions library

	Copyright (C) 2015 Tomoyuki Niimi, Syunsuke Okamoto.<okamoto@contec.jp>

***/

#ifndef __CONEXIO_CMM920__
#define __CONEXIO_CMM920__

// BOOL define
typedef unsigned int BOOL;
#define FALSE 0
#define TRUE 1

// BYTE define
typedef unsigned char BYTE,*LPBYTE;
typedef unsigned short WORD,*LPWORD;
typedef unsigned long DWORD,*LPDWORD;

// CMM920 Series
#define CONEXIO_CMM920_MODE_COMMON  0x00
#define CONEXIO_CMM920_MODE_RUN  0x01
#define CONEXIO_CMM920_MODE_TEST  0x0F
#define CONEXIO_CMM920_RECVCOMMAND  0x80

#define CONEXIO_CMM920_RESET  0xF2
#define CONEXIO_CMM920_SET_MODE  0x01
#define CONEXIO_CMM920_SET_WIRELESS  0x12
#define CONEXIO_CMM920_SET_ANTENNA  0x15
#define CONEXIO_CMM920_SET_ADDRESS  0x11
#define CONEXIO_CMM920_SET_ACK  0x14
#define CONEXIO_CMM920_SET_TIMER  0x13
#define CONEXIO_CMM920_SET_VERSION  0x31
#define CONEXIO_CMM920_SET_LSI  0xF1

#define CONEXIO_CMM920_SENDDATA  0x01

#define CONEXIO_CMM920_SET_MODE_STOP  0x00
#define CONEXIO_CMM920_SET_MODE_SETTING  0x01
#define CONEXIO_CMM920_SET_MODE_RUN  0x02
#define CONEXIO_CMM920_SET_MODE_TEST  0x03

#define CONEXIO_CMM920_SET_READING_READ  0x01
#define CONEXIO_CMM920_SET_READING_WRITE  0x02

#define CONEXIO_CMM920_SET_ANTENNA_00	0x00
#define CONEXIO_CMM920_SET_ANTENNA_01	0x01

#define CONEXIO_CMM920_SET_ANTENNA_INTERNAL  CONEXIO_CMM920_SET_ANTENNA_00
#define CONEXIO_CMM920_SET_ANTENNA_EXTERNAL  CONEXIO_CMM920_SET_ANTENNA_01

#define CONEXIO_CMM920_SENDDATA_MODE_ACK  0x01
#define CONEXIO_CMM920_SENDDATA_MODE_RESP  0x10
#define CONEXIO_CMM920_SENDDATA_MODE_NOACK_NORESP  0x00
#define CONEXIO_CMM920_SENDDATA_MODE_ACK_NORESP  CONEXIO_CMM920_SENDDATA_MODE_ACK
#define CONEXIO_CMM920_SENDDATA_MODE_NOACK_RESP  CONEXIO_CMM920_SENDDATA_MODE_RESP
#define CONEXIO_CMM920_SENDDATA_MODE_ACK_RESP   \
			(CONEXIO_CMM920_SENDDATA_MODE_ACK | CONEXIO_CMM920_SENDDATA_MODE_RESP )

#define CONEXIO_CMM920_HOP_SINGLE	0x00
#define CONEXIO_CMM920_HOP_MULTI	0x01

#define CONEXIO_CMM920_SET_WIRELESS_BITRATE_50KBPS  0x01
#define CONEXIO_CMM920_SET_WIRELESS_BITRATE_100KBPS  0x02
#define CONEXIO_CMM920_SET_WIRELESS_POWER_01MW  0x01
#define CONEXIO_CMM920_SET_WIRELESS_POWER_10MW  0x02
#define CONEXIO_CMM920_SET_WIRELESS_POWER_20MW  0x03

#define CONEXIO_CMM920_SET_ENABLE  0x01
#define CONEXIO_CMM920_SET_DISABLE   0x00

#define CONEXIO_CMM920_LSIADDRESS_PRELEN	( 0x000105F8 )
#define CONEXIO_CMM920_LSIADDRESS_WHITENING	( 0x00010032 )
#define CONEXIO_CMM920_LSIADDRESS_DIVER_ENABLE	( 0x000126FF )
#define CONEXIO_CMM920_LSIADDRESS_MHR_MODE	( 0x00000055 )
#define CONEXIO_CMM920_LSIADDRESS_CRC_CALC_INVERSE	( 0x00000EAA )
#define CONEXIO_CMM920_LSIADDRESS_FILTER_S_PANID	( 0x00001033 )
#define CONEXIO_CMM920_LSIADDRESS_FILTER_D_PANID	( 0x00001022 )
#define CONEXIO_CMM920_LSIADDRESS_FILTER_D_ADDR	( 0x00001011 )
#define CONEXIO_CMM920_LSIADDRESS_SFD(n)	( 0x000106F0 + (n * 0x100) )

#define CONEXIO_CMM920_SFD_MULTIHOP		( 0x7209 )
#define CONEXIO_CMM920_PRELEN_MULTIHOP	( 0x0C )

#define CONEXIO_CMM920_SFDNUM_0	( 0 )
#define CONEXIO_CMM920_SFDNUM_1	( 1 )

typedef struct __conexioCMM920_packet{
	BYTE dle;
	BYTE stx;
	BYTE size[2];
	BYTE command[2];
	BYTE result;
	BYTE resultCode;
	BYTE *data;
	BYTE sum;
	BYTE etx;
} CONEXIO920PACKET, *PCONEXIO920PACKET;

/* FC */
#define CONEXIO_CMM920_MHR_FC( srcmode, ver, destmode, sqnsupp, panidcomp, ar, pending, sec, type )	\
	( (WORD) (srcmode << 14)| (ver << 12) | \
			(destmode << 10) | (sqnsupp << 8) | \
			(panidcomp << 6) | (ar << 5) | \
			(pending << 4) | (sec << 3)| \
			(type) \
	)

#define CONEXIO_CMM920_MHR_FC_DESTADDRMODE( fc )	( (fc & 0xC000) >> 14 )

#define CONEXIO_CMM920_MHR_FC_DESTADDRMODE_NONE	0
#define CONEXIO_CMM920_MHR_FC_DESTADDRMODE_8BIT	1
#define CONEXIO_CMM920_MHR_FC_DESTADDRMODE_16BIT	2
#define CONEXIO_CMM920_MHR_FC_DESTADDRMODE_64BIT	3

#define CONEXIO_CMM920_MHR_FC_SRCADDRMODE( fc )	( (fc & 0x0C00) >> 10 )

#define CONEXIO_CMM920_MHR_FC_SRCADDRMODE_NONE	0
#define CONEXIO_CMM920_MHR_FC_SRCADDRMODE_8BIT	1
#define CONEXIO_CMM920_MHR_FC_SRCADDRMODE_16BIT	2
#define CONEXIO_CMM920_MHR_FC_SRCADDRMODE_64BIT	3

// function
extern int conexio_cmm920_init(char *DevName);
extern int conexio_cmm920_exit();
extern int conexio_cmm920_reset();
extern int conexio_cmm920_mode(BYTE isWrite , int *code);
extern int conexio_cmm920_address(BYTE isWrite, unsigned short *panId, BYTE Addr[], unsigned short *shortAddr );
extern int conexio_cmm920_wireless(BYTE isWrite, BYTE *bitrate, BYTE *channel, BYTE *power, char *sendSenceLvl, char *recvSenceLvl, unsigned short *sendSenceTim, BYTE *sendSenceNum, BYTE *ackRetryNum, unsigned short *ackWaitTim);
extern int conexio_cmm920_timer( BYTE isWrite, unsigned short *tim );
extern int conexio_cmm920_auto_ack_frame( BYTE isWrite, unsigned short *phr, unsigned char *fc_upper );
extern int conexio_cmm920_antenna( BYTE isWrite, BYTE *antennaMode );
extern int conexio_cmm920_version(int *ver, int *rev);
extern int conexio_cmm920_lsi(unsigned long lsi_addr, int isWrite, unsigned short *value);

extern int conexio_cmm920_lsi_data_preamble_bit_len(int isWrite, BYTE length);
extern int conexio_cmm920_lsi_data_whitening( int isWrite, BYTE isEnable );
extern int conexio_cmm920_lsi_diversity_enable( int isWrite, BYTE isEnable );
extern int conexio_cmm920_lsi_mhr_mode( int isWrite, BYTE isEnable );
extern int conexio_cmm920_lsi_crc_calc_inverse( int isWrite, BYTE isEnable );
extern int conexio_cmm920_lsi_s_panid_filter( int isWrite, BYTE isEnable );
extern int conexio_cmm920_lsi_d_panid_filter( int isWrite, BYTE isEnable );
extern int conexio_cmm920_lsi_d_address_filter( int isWrite, BYTE isEnable );
extern int conexio_cmm920_lsi_data_sfd( int isWrite, WORD addr, BYTE sfd_no);

extern int conexio_cmm920_data_send_single(BYTE buf[], int size, int send_mode, BYTE r_buf[]);
extern int conexio_cmm920_data_send_multi(BYTE buf[], int size, int send_mode, unsigned short dest_id, unsigned short src_id, long dest_addr, long src_addr, BYTE r_buf[]);
extern int conexio_cmm920_data_recv(BYTE buf[], int *size, int hop, int *r_channel, int *rx_pwr , unsigned int *crc);
extern int conexio_cmm920_data_recv_multi(BYTE buf[], int *size, int *r_channel, int *rx_pwr, unsigned int *crc_val, unsigned short *dest_id, unsigned short *src_id, long *dest_addr, long *src_addr );
// wrapper function
extern int conexio_cmm920_set_mode(int code);
extern int conexio_cmm920_set_address(unsigned short panId, BYTE Addr[], unsigned short shortAddr );
extern int conexio_cmm920_set_wireless(int iBitrate, BYTE channel, BYTE power, char sendLv, char recvLv, unsigned short sendTim, BYTE sendNum, BYTE ackRetryNum, unsigned short ackWaitTim);
extern int conexio_cmm920_set_timer( unsigned short tim );
extern int conexio_cmm920_set_auto_ack_frame( unsigned short phr, unsigned char fc_upper );
extern int conexio_cmm920_set_antenna( BYTE antennaMode );

extern int conexio_cmm920_get_mode(int *code);
extern int conexio_cmm920_get_address(unsigned short *panId, BYTE Addr[], unsigned short *shortAddr );
extern int conexio_cmm920_get_wireless(int *iBitrate, BYTE *channel, BYTE *power, char *sendLv, char *recvLv, unsigned short *sendTim, BYTE *sendNum, BYTE *ackRetryNum, unsigned short *ackWaitTim);
extern int conexio_cmm920_get_timer( unsigned short *tim );
extern int conexio_cmm920_get_auto_ack_frame( unsigned short *phr, unsigned char *fc_upper );
extern int conexio_cmm920_get_antenna( BYTE *antennaMode );

extern void conexio_cmm920_set_hop_mode( BYTE hop );

//extern int conexio_cmm920_set_address_default();
extern int conexio_cmm920_set_wireless_default();
extern int conexio_cmm920_set_timer_default();
extern int conexio_cmm920_set_auto_ack_frame_default();


// Internal( private function )

BYTE _calc_Hex2Bcd( BYTE hex );
BYTE _calc_Bcd2Hex( BYTE bcd );
BYTE _conexio_cmm920_dBm2Hex( int dBm );
int _conexio_cmm920_Hex2dBm( BYTE hex );


int SendCommand(BYTE buf[], int size, BYTE mode, BYTE command);
int SendTelegram(BYTE buf[], int size, int hop, int send_mode, unsigned short *dest_id, unsigned short *src_id, long *dest_addr, long *src_addr);


int RecvTelegramSingleHop(BYTE buf[], int *size , int *r_channel, int *rx_pwr , unsigned int *crc);
int RecvTelegram(BYTE buf[], int *size , int hop, int *r_channel, int *rx_pwr , unsigned int *crc, unsigned short *dest_id, unsigned short *src_id, long *dest_addr, long *src_addr);
int RecvCommandAck( BYTE *buf, int *size , BYTE mode, BYTE command );

PCONEXIO920PACKET allocConexioCMM920_packet(PCONEXIO920PACKET pac, BYTE mode, BYTE com, BYTE isSend);
void freeConexioCMM920_packet(PCONEXIO920PACKET pac);
BYTE* pktGetBYTEArray(PCONEXIO920PACKET pac, int size,int *retSize);

#endif
