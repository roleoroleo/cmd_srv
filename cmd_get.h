#include "cmd_srv.h"

int p2p_get_sd_state();

int p2p_get_power();
int p2p_get_light();
int p2p_get_motion_record();
int p2p_get_mirror_flip();
int p2p_get_motion_detect();
int p2p_get_alarm_mode();
int p2p_get_day_night_mode();
int p2p_get_alarm_sensitivity();

int p2p_get_video_backup_state(int index, video_backup_info_t video_backup_info);
int p2p_get_encode_mode(int value);
int p2p_get_high_resolution(int value);
int p2p_get_ldc(int percent);
int p2p_get_baby_cry(int enable);
int p2p_get_abnormal_sound(int enable);
int p2p_get_abnormal_sound_sensitivity(int value);
int p2p_get_mic_volume();
int p2p_get_viewpoint_trace(unsigned char mode);
