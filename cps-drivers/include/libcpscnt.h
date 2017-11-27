#include "cps_def.h"

#define CNT_ERR_SUCCESS		0
#define CNT_ERR_INI_RESOURCE	1
#define CNT_ERR_INI_INTERRUPT	2
#define CNT_ERR_INI_MEMORY	3
#define CNT_ERR_INI_REGISTRY	4
#define CNT_ERR_INI_BOARD_ID	5

#define CNT_ERR_DLL_INVALID_ID		10001
#define CNT_ERR_DLL_CALL_DRIVER		10002
#define CNT_ERR_DLL_CREATE_FILE		10003
#define CNT_ERR_DLL_CLOSE_FILE		10004
#define CNT_ERR_DLL_CREATE_THREAD	10005

#define CNT_ERR_INFO_INVALID_DEVICE	10050
#define CNT_ERR_INFO_NOT_FIND_DEVICE	10051
#define CNT_ERR_INFO_INVALID_INFOTYPE	10052

#define CNTM_INTERRUPT	0x1300

#define CNT_ZPHASE_NOT_USE	1
#define CNT_ZPHASE_NEXT_ONE	2
#define CNT_ZPHASE_EVERY_TIME	3

#define CNT_ZLOGIC_POSITIVE	0
#define CNT_ZLOGIC_NEGATIVE	1

#define CNT_SIGTYPE_ISOLATE	0

#define CNT_DIR_DOWN	0
#define CNT_DIR_UP	1

#define CNT_MODE_1PHASE	0
#define CNT_MODE_2PHASE	1
#define CNT_MODE_GATECONTROL	2

#define CNT_MUL_X1 0
#define CNT_MUL_X2 1
#define CNT_MUL_X4 2

#define CNT_CLR_ASYNC	0
#define CNT_CLR_SYNC	1



/****  Structure ****/
typedef struct __contec_cps_cnt_int_callback_data__
{
	short id;
	short Message;
	short	wParam;
	long		lParam;
	void*	Param;

}CONTEC_CPS_CNT_INT_CALLBACK_DATA, *PCONTEC_CPS_CNT_INT_CALLBACK_DATA;

typedef void (*PCONTEC_CPS_CNT_INT_CALLBACK)(short, short, long, long, void *);

// Common Functions
extern unsigned long ContecCpsCntInit( char *DeviceName, short *Id );
extern unsigned long ContecCpsCntExit( short Id );
extern unsigned long ContecCpsCntGetErrorStrings( unsigned long code, char *Str );
extern unsigned long ContecCpsCntQueryDeviceName( short Id, char *DeviceName, char *Device );
extern unsigned long ContecCpsCntGetMaxChannels( short Id, short *maxChannel );

// Set Functions
extern unsigned long ContecCpsCntSetZMode( short Id, short ChNo, short Mode ); 
extern unsigned long ContecCpsCntSetZLogic( short Id, short ChNo, short Zlogic );
extern unsigned long ContecCpsCntSelectChannelSignal( short Id, short ChNo, short sigType );
extern unsigned long ContecCpsCntSetCountDirection( short Id, short ChNo, short Dir );
extern unsigned long ContecCpsCntSetOperationMode( short Id, short ChNo, short Phase, short Mul, short SyncDir );
extern unsigned long ContecCpsCntSetDigitalFilter( short Id, short ChNo, short FilterValue );
extern unsigned long ContecCpsCntSetPulseWidth( short Id, short ChNo, short PlsWidth );

// Get Functions
extern unsigned long ContecCpsCntGetZMode( short Id, short ChNo, short *Mode ); 
extern unsigned long ContecCpsCntGetZLogic( short Id, short ChNo, short *Zlogic );
extern unsigned long ContecCpsCntGetChannelSignal( short Id, short ChNo, short *sigType );
extern unsigned long ContecCpsCntGetCountDirection( short Id, short ChNo, short *Dir );
extern unsigned long ContecCpsCntGetOperationMode( short Id, short ChNo, short *Phase, short *Mul, short *SyncDir );
extern unsigned long ContecCpsCntGetDigitalFilter( short Id, short ChNo, short *FilterValue );
extern unsigned long ContecCpsCntGetPulseWidth( short Id, short ChNo, short *PlsWidth );

// Counter Running Functions
extern unsigned long ContecCpsCntStartCount( short Id, short ChNo[], short ChNum );
extern unsigned long ContecCpsCntStopCount( short Id, short ChNo[], short ChNum );
extern unsigned long ContecCpsCntPreset( short Id, short ChNo[], short ChNum, unsigned long PresetData[] );
extern unsigned long ContecCpsCntReadCount( short Id, short ChNo[], short ChNum, unsigned long CntDat[] );
extern unsigned long ContecCpsCntReadStatus( short Id, short ChNo, short *Status );


/**** INTERRUPT Event Functions ****/
extern unsigned long ContecCpsCntNotifyCountup( short Id, short BitNum, short Logic );
extern unsigned long ContecCpsCntSetInterruptCallBackProc( short Id, PCONTEC_CPS_CNT_INT_CALLBACK cb, void* Param );

