/*
 * conexio_cmm920.h
 *
 *  Created on: 2016/01/05
 *      Author: contec
 */

#ifndef CONEXIO_CMM920_H_
#define CONEXIO_CMM920_H_

#define WirelessSettingRequest	conexio_cmm920_set_wireless_default
#define ResetRequest	conexio_cmm920_reset
#define SettingRequest()	conexio_cmm920_set_mode(CONEXIO_CMM920_SET_MODE_SETTING)
#define RunRequest()	conexio_cmm920_set_mode(CONEXIO_CMM920_SET_MODE_RUN)
#define StopRequest()	conexio_cmm920_set_mode(CONEXIO_CMM920_SET_MODE_STOP)
#define InternalAntennaRequest() conexio_cmm920_set_antenna( CONEXIO_CMM920_SET_ANTENNA_INTERNAL )
#define ExternalAntennaRequest()	conexio_cmm920_set_antenna( CONEXIO_CMM920_SET_ANTENNA_EXTERNAL )
#define AutoAckFrameSettingRequest	conexio_cmm920_set_auto_ack_frame_default
#define TimerRequest	conexio_cmm920_set_timer_default
#define LsiSettingRequest()	conexio_cmm920_lsi_data_preamble_bit_len(CONEXIO_CMM920_SET_READING_WRITE, 0x01)
#define DataWhiteningEnableRequest() conexio_cmm920_lsi_data_whitening(CONEXIO_CMM920_SET_READING_WRITE, 0x01)
#define DataWhiteningInvalidRequest() conexio_cmm920_lsi_data_whitening(CONEXIO_CMM920_SET_READING_WRITE, 0x00)
#define DiversityEnableRequest()	conexio_cmm920_lsi_diversity_enable(CONEXIO_CMM920_SET_READING_WRITE, 0x01)
#define DiversityInvalidRequest()	conexio_cmm920_lsi_diversity_enable(CONEXIO_CMM920_SET_READING_WRITE, 0x00)
#define DiversitySettingRequest(val)	conexio_cmm920_lsi_diversity_enable(CONEXIO_CMM920_SET_READING_READ, val)

#endif /* CONEXIO_CMM920_H_ */
