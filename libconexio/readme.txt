conexio_cmm920_init	初期化
	DevName：　シリアルポート名
conexio_cmm920_exit	終了

conexio_cmm920_reset	リセット

conexio_cmm920_mode	モード設定・取得
	isWrite:	読み込みか書き込みか
		CONEXIO_CMM920_SET_READING_READ : 読み込み
		CONEXIO_CMM920_SET_READING_WRITE : 書き込み
	code：モード
		CONEXIO_CMM920_SET_MODE_STOP　：停止モード
		CONEXIO_CMM920_SET_MODE_SETTING : 設定モード
		CONEXIO_CMM920_SET_MODE_RUN　： RUNモード
		CONEXIO_CMM920_SET_MODE_TEST　：　テストモード

conexio_cmm920_address	アドレス設定・取得
	isWrite		:読み込みか書き込みか
		CONEXIO_CMM920_SET_READING_READ : 読み込み
		CONEXIO_CMM920_SET_READING_WRITE : 書き込み
	panId		：PAN ID
	Addr,		：アドレス

conexio_cmm920_wireless	920MHz無線設定
	isWrite		:読み込みか書き込みか
		CONEXIO_CMM920_SET_READING_READ : 読み込み
		CONEXIO_CMM920_SET_READING_WRITE : 書き込み
	bitrate		:ビットレート
		CONEXIO_CMM920_SET_WIRELESS_BITRATE_50KBPS  0x01
		CONEXIO_CMM920_SET_WIRELESS_BITRATE_100KBPS  0x02

	channel		：チャネル番号
	power		：出力
		CONEXIO_CMM920_SET_WIRELESS_POWER_01MW
		CONEXIO_CMM920_SET_WIRELESS_POWER_10MW
		CONEXIO_CMM920_SET_WIRELESS_POWER_20MW
	sendSenceLvl	：送信センスレベル
	recvSenceLvl	：受信センスレベル
	sendSenceTim	：送信センス時間 [ms]
	sendSenceNum	：送信センス番号
	ackRetryNum	：ackリトライ回数
	ackWaitTim	：ackウェイト時間 [ms]

conexio_cmm920_timer	タイマ設定・取得
	isWrite		:読み込みか書き込みか
		CONEXIO_CMM920_SET_READING_READ : 読み込み
		CONEXIO_CMM920_SET_READING_WRITE : 書き込み
	tim		：時間[ms]

conexio_cmm920_auto_ack_frame	自動ACKフレーム設定
	isWrite		:読み込みか書き込みか
		CONEXIO_CMM920_SET_READING_READ : 読み込み
		CONEXIO_CMM920_SET_READING_WRITE : 書き込み
	phr		：PHR部
	fc_upper	：FC部(上位8bit)　※下位8bitは設定できない

conexio_cmm920_antenna
	isWrite		:読み込みか書き込みか
		CONEXIO_CMM920_SET_READING_READ : 読み込み
		CONEXIO_CMM920_SET_READING_WRITE : 書き込み
	antennaMode	：アンテナのモード
		CONEXIO_CMM920_SET_ANTENNA_INTERNAL	内部アンテナ使用
		CONEXIO_CMM920_SET_ANTENNA_EXTERNAL	外部アンテナ使用

conexio_cmm920_version
	ver
	rev

conexio_cmm920_lsi
	lsi_addr：LSIのアドレス
	isWrite		:読み込みか書き込みか
		CONEXIO_CMM920_SET_READING_READ : 読み込み
		CONEXIO_CMM920_SET_READING_WRITE : 書き込み
	value	：値


conexio_cmm920_lsi_data_preamble_bit_len
	isWrite		:読み込みか書き込みか
		CONEXIO_CMM920_SET_READING_READ : 読み込み
		CONEXIO_CMM920_SET_READING_WRITE : 書き込み
	length		：データ長

conexio_cmm920_lsi_data_whitening
	isWrite		:読み込みか書き込みか
		CONEXIO_CMM920_SET_READING_READ : 読み込み
		CONEXIO_CMM920_SET_READING_WRITE : 書き込み
	isEnable：有効か無効か
		CONEXIO_CMM920_SET_ENABLE  有効
		CONEXIO_CMM920_SET_DISABLE   無効

conexio_cmm920_lsi_diversity_enable
	isWrite		:読み込みか書き込みか
		CONEXIO_CMM920_SET_READING_READ : 読み込み
		CONEXIO_CMM920_SET_READING_WRITE : 書き込み
	isEnable：有効か無効か
		CONEXIO_CMM920_SET_ENABLE  有効
		CONEXIO_CMM920_SET_DISABLE   無効

conexio_cmm920_data_send_single
	buf	：データバッファ
	size	：データサイズ
	send_mode:送信モード
		CONEXIO_CMM920_SENDDATA_MODE_NOACK_NORESP　	：ACKなし　レスポンスなし
		CONEXIO_CMM920_SENDDATA_MODE_ACK_NORESP		：ACKあり　レスポンスなし
		CONEXIO_CMM920_SENDDATA_MODE_NOACK_RESP		：ACKなし　レスポンスあり
		CONEXIO_CMM920_SENDDATA_MODE_ACK_RESP		：ACKあり　レスポンスあり

	r_buf	:send_modeがACKが変える場合のときの、受信データ

conexio_cmm920_data_send_multi
	buf	：データバッファ
	size	：データサイズ
	send_mode:送信モード
		CONEXIO_CMM920_SENDDATA_MODE_NOACK_NORESP　	：ACKなし　レスポンスなし
		CONEXIO_CMM920_SENDDATA_MODE_ACK_NORESP		：ACKあり　レスポンスなし
		CONEXIO_CMM920_SENDDATA_MODE_NOACK_RESP		：ACKなし　レスポンスあり
		CONEXIO_CMM920_SENDDATA_MODE_ACK_RESP		：ACKあり　レスポンスあり

	dest_id : 送信先 PAN ID
	src_id : 送信元　PAN ID
	dest_addr : 送信先　short Addr
	src_addr : 送信元 short Addr
	r_buf	:send_modeがACKが変える場合のときの、受信データ

conexio_cmm920_data_recv
	buf	：受信データバッファ（あらかじめ配列を用意する必要あり）
	size	：受信データサイズ（サイズ=0で引き渡すと受信したデータのサイズが格納される。また、
		　受信データサイズをあらかじめ入れた場合はパケットチェックでデータサイズもチェックする）
	hop	：ホップ数　（現在はシングルホップのみ）
	r_channel：受信チャネル番号
	rx_pwr	：受信強度 (-11dBm ～ -110dBm )
	crc	：CRCチェック値

// wrapper function
conexio_cmm920_set_mode
	code：モード
		CONEXIO_CMM920_SET_MODE_STOP　：停止モード
		CONEXIO_CMM920_SET_MODE_SETTING : 設定モード
		CONEXIO_CMM920_SET_MODE_RUN　： RUNモード
		CONEXIO_CMM920_SET_MODE_TEST　：　テストモード

conexio_cmm920_set_address
	panId		：自身のPAN ID
	Addr[]		：自身のロングアドレス
	shortAddr	：自身のショートアドレス
conexio_cmm920_set_wireless(int iBitrate, BYTE channel, BYTE power, char sendLv, char recvLv, unsigned short sendTim, BYTE sendNum, BYTE ackRetryNum, unsigned short ackWaitTim);
conexio_cmm920_set_timer( unsigned short tim );
conexio_cmm920_set_auto_ack_frame( unsigned short phr, unsigned char fc_upper );
conexio_cmm920_set_antenna( BYTE antennaMode );

//デフォルト値設定関数（ VCCIなどで使用した値 )
conexio_cmm920_set_wireless_default
conexio_cmm920_set_timer_default
conexio_cmm920_set_auto_ack_frame_default

