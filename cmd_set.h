#include "cmd_srv.h"

int p2p_set_power(MSG_TYPE msg_type);
int p2p_set_light(MSG_TYPE msg_type);
int p2p_set_motion_record(MSG_TYPE msg_type);
int p2p_set_mirror_flip(MSG_TYPE msg_type);
int p2p_set_motion_detect(int index, motion_rect_t motion_rect, int nIOCtrlCmdNum);
int p2p_set_alarm_mode(int alarm_mode);
int p2p_set_day_night_mode(MSG_TYPE msg_type, int value);
int p2p_set_alarm_sensitivity(int sensitivity);
//int p2p_set_video_backup_state(int index, video_backup_state_set *backup_state, int nIOCtrlCmdNum);
int p2p_set_encode_mode(int value);
int p2p_set_high_resolution(int value);
int p2p_set_ldc(int percent);
int p2p_set_baby_cry(int enable);
int p2p_set_abnormal_sound(int enable);
int p2p_set_abnormal_sound_sensitivity(int value);
int p2p_set_mic_volume(int percent);
int p2p_set_viewpoint_trace(unsigned char mode);
