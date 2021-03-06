#ifndef CMD_SRV_H
#define CMD_SRV_H

#include <stdlib.h>
#include <mqueue.h>

#define _F_	__FILE__
#define _FU_	__FUNCTION__
#define _L_	__LINE__

#define IPC_QUEUE_NAME "/ipc_dispatch"

#define MID_P2P 1
#define MID_RMM 2
#define MID_CLOUD 4
#define MID_DISPATCH 4
#define MID_RCD 0x10

// Haven't been able to verify if these are correct
#define TF_CHECK_OK 0
#define TF_SPACE_TOO_LITTLE 4
#define TF_NOT_EXIST 5
#define TF_CARD_BAD 3
#define TF_SYSTEM_FORMAT_ERROR 2
#define TF_WRITE_SPEED_SLOW 1

// MMAP offsets
#define MMAP_PROTOCOL 0x1cc // char[4];
#define MMAP_VERSION 0x210 // char[21]
#define MMAP_P2P_ID 0x230 // char[20];
#define MMAP_KEY 0x290 // char[16];
#define MMAP_MAC 0x358 // char[17]
#define MMAP_DAY_NIGHT_MODE 0x496
#define MMAP_LDC_PERCENT 0x4c8
#define MMAP_BABY_CRY_ENABLE 0x4cc
#define MMAP_ABNORMAL_SOUND_ENABLE 0x4d0
#define MMAP_HUMAN_MOTION_ENABLE 0x4d4
#define MMAP_MIC_VOLUME 0x4d8
#define MMAP_PANORAMA_CAPTURE_STATE 0x4dc
#define MMAP_ENCODE_MODE 0x4e4
#define MMAP_HIGH_RESOLUTION 0x4e8
#define MMAP_PRE_PWD 0x4ec // char[15]
#define MMAP_PWD 0x50c // char[15]
#define MMAP_SD_SIZE 0x580
#define MMAP_SD_LEFTSIZE 0x584
// Not 100% sure of the order of the motion_rect struct 
// but it should be pretty close if not correct
#define MMAP_MOTION_RECT_MODE 0x5b8
#define MMAP_MOTION_RECT_RESOLUTION 0x5bc
#define MMAP_MOTION_RECT_LEFT 0x5c0
#define MMAP_MOTION_RECT_TOP 0x5c2
#define MMAP_MOTION_RECT_RIGHT 0x5c4
#define MMAP_MOTION_RECT_BOTTOM 0x5c6
#define MMAP_MIRROR 0x5d8
#define MMAP_LIGHT_MODE 0x5dc
#define MMAP_POWER_MODE 0x5e0
#define MMAP_IRLIGHT_MODE 0x5e8
#define MMAP_RECORD_MODE 0x5ec
#define MMAP_DEBUG_MODE 0x86c
#define MMAP_MOTION_SENSITIVITY 0x907
#define MMAP_ABNORMAL_SOUND_SENSITIVITY 0x908
#define MMAP_TF_STATUS_STATE 0x90c
// Not 100% sure of the order of the video_backup_info struct 
// but it should be pretty close if not correct
#define MMAP_VIDEO_BACKUP_INFO_ENABLE 0x960
#define MMAP_VIDEO_BACKUP_INFO_RESOLUTION 0x961
#define MMAP_VIDEO_BACKUP_INFO_BACKUP_PERIOD 0x962
#define MMAP_VIDEO_BACKUP_INFO_USER_PATH 0x963
#define MMAP_VIDEO_BACKUP_INFO_ROUTER_SD_TOTAL_SIZE 0x964
#define MMAP_VIDEO_BACKUP_INFO_ROUTER_SD_FREE_SIZE 0x968
#define MMAP_VIDEO_BACKUP_INFO_ROUTER_SD_CAM_USED_SIZE 0x96c
#define MMAP_VIDEO_BACKUP_INFO_EXTRA_SD_TOTAL_SIZE 0x970
#define MMAP_VIDEO_BACKUP_INFO_EXTRA_SD_FREE_SIZE 0x974
#define MMAP_VIDEO_BACKUP_INFO_EXTRA_SD_CAM_USED_SIZE 0x978
#define MMAP_VIEWPOINT_TRACE 0x988
#define MMAP_VOICE_CTRL 0x989
#define MMAP_LAPSE_VIDEO_ENABLE 0x9a8
#define MMAP_LAPSE_VIDEO_END_TIME 0x9b0
#define MMAP_SPEAK_MODE 0x9c0

#define MAX_SESSION_NUM 8
#define MAX_VIEWING_NUM 6

#define VC_RECORD_DURATION 15

typedef struct
{
    int preset_enable;
    unsigned char unknown[0x8];
} ptz_info_t;

// This struct is unknown
typedef struct
{
    int stat;
//    ...
} tf_status_t;

typedef struct {
    char enable; /* Created by retype action */
    char resolution; /* Created by retype action */
    char backup_period; /* Created by retype action */
    char user_path; /* Created by retype action */
    unsigned int router_sd_total_size; /* Created by retype action */
    unsigned int router_sd_free_size; /* Created by retype action */
    unsigned int router_sd_cam_used_size; /* Created by retype action */
    unsigned int extra_sd_total_size; /* Created by retype action */
    unsigned int extra_sd_free_size; /* Created by retype action */
    unsigned int extra_sd_cam_used_size; /* Created by retype action */
} video_backup_info_t;

typedef struct {
    unsigned int mode;
    unsigned int resolution;
    short left; // top left x
    short top; // top left y
    short right; // bottom right x
    short bottom; // bottom right y
} motion_rect_t;


typedef struct
{
    unsigned char unknown_00[0x4];		// 0x0
    int region_id;				// 0x4   int (4 bytes)
    int language;				// 0x8   int (4 bytes)
    char api_server[128];			// 0xc   char[128]
    unsigned char unknown_01[0x80];		// 0x8c
    char log_server[128];			// 0x10c char[128]
    char sname[64];				// 0x18c char[64]
    char dlproto[4];				// 0x1cc char[4]
    unsigned char unknown_02[0x3c];		// 0x1d0
    unsigned int systick;			// 0x20c unsigned int (4 bytes)
    char version[32];				// 0x210 char[21]
    char p2p_id[32];				// 0x230 char[20]
    unsigned char unknown_03[0x20];		// 0x250 char[32]
    unsigned char unknown_04[0x20];		// 0x270 char[32]
    char key[16];				// 0x290 char[16]
    unsigned char unknown_05[0xb8];		// 0x2a0
    char mac[32];				// 0x358 char[17]
    unsigned char unknown_06[0x11e];		// 0x378
    int irlight_mode_no;			// 0x496 int (4 bytes) TODO: check this field against offset 0x5e8
    unsigned char unknown_07[0xa];		// 0x49a
    int tz_offset;				// 0x4a4 int (4 bytes)
    unsigned char unknown_08[0x20];		// 0x4a8
    int ldc_percent;				// 0x4c8 int (4 bytes)
    int baby_cry_enable;			// 0x4cc int (4 bytes)
    int abnormal_sound_enable;			// 0x4d0 int (4 bytes)
    int human_motion_enable;			// 0x4d4 int (4 bytes)
    int mic_volume;				// 0x4d8 int (4 bytes)
    int panorama_capture_state;			// 0x4dc int (4 bytes)
    unsigned char unknown_09[0x4];		// 0x4e0
    int encode_mode;				// 0x4e4 int (4 bytes)
    int high_resolution;			// 0x4e8 int (4 bytes)
    char pre_pwd[16];				// 0x4ec char[15]
    unsigned char unknown_10[0x10];		// 0x4fc
    char pwd[16];				// 0x50c char[15]
    unsigned char unknown_11[0x10];		// 0x51c
    unsigned char unknown_12[0x10];		// 0x52c
    unsigned char unknown_13[0x10];		// 0x53c
    unsigned char unknown_14[0x24];		// 0x54c
    int in_packet_loss;				// 0x570 int (4 bytes)
    int out_packet_loss;			// 0x574 int (4 bytes)
    unsigned char unknown_15[0x8];		// 0x578
    int sd_size;				// 0x580 int (4 bytes)
    int sd_leftsize;				// 0x584 int (4 bytes)
    unsigned char unknown_16[0x20];		// 0x588
    int motion_stat;				// 0x5a8 int (4 bytes)
    int motion_type;				// 0x5ac int (4 bytes)
    int motion_time;				// 0x5b0 int (4 bytes)
    unsigned char unknown_17[0x4];		// 0x5b4
    motion_rect_t motion_rect;			// 0x5b8 motion_rect (16 bytes)
    unsigned char unknown_18[0x10];		// 0x5c8
    int mirror;					// 0x5d8 int (4 bytes)
    int light_mode;				// 0x5dc int (4 bytes)
    int power_mode;				// 0x5e0 int (4 bytes)
    unsigned char unknown_19[0x4];		// 0x5e4
    int irlight_mode;				// 0x5e8 int (4 bytes)
    int record_mode;				// 0x5ec int (4 bytes)
    unsigned char unknown_20[0x27c];		// 0x5f0
    int debug_mode;				// 0x86c int (4 bytes)
    int video_occlusion;			// 0x870 int (4 bytes)
    unsigned char unknown_21[0x8];		// 0x874
    int ptz_sleep;				// 0x87c int (4 bytes)
    int ptz_cruise_flag;			// 0x880 int (4 bytes)
    unsigned char unknown_22[0x8];		// 0x884
    ptz_info_t ptz_info[8];			// 0x88c ptz_info_t[8] (96 bytes)
    int ptz_panoramic_sleep;			// 0x8ec int (4 bytes)
    int ptz_cruise_mode;			// 0x8f0 int (4 bytes)
    int ptz_cruise_start_time;			// 0x8f4 int (4 bytes)
    int ptz_cruise_end_time;			// 0x8f8 int (4 bytes)
    int ptz_motion_track_switch;		// 0x8fc int (4 bytes)
    unsigned char unknown_23[0x6];		// 0x900
    unsigned char is_xiaomirouter;		// 0x906 unsigned char (1 byte)
    unsigned char motion_sensitivity;		// 0x907 unsigned char (1 byte)
    unsigned char abnormal_sound_sensitivity;	// 0x908 unsigned char (1 byte)
    unsigned char unknown_24[0x3];		// 0x909
    tf_status_t tf_status;			// 0x90c tf_status_t (? bytes)
    unsigned char unknown_25[0x50];		// 0x910
    video_backup_info_t video_backup_info;	// 0x960 video_backup_info (28 bytes)
    unsigned char unknown_26[0xc];		// 0x97c
    unsigned char viewpoint_trace;		// 0x988 unsigned char (1 byte)
    unsigned char voice_ctrl;			// 0x989 unsigned char (1 byte)
    unsigned char unknown_27[0x1e];		// 0x98a
    int lapse_video_enable;			// 0x9a8 int (4 bytes)
    unsigned char unknown_28[0x4];		// 0x9ac
    int lapse_video_end_time;			// 0x9b0 int (4 bytes)
    unsigned char unknown_29[0xc];		// 0x9b4
    unsigned char speak_mode;			// 0x9c0 unsigned char (1 byte)
    char aec_key[11];				// 0x9c1 char[11]
} __attribute__((packed)) mmap_info_s;

/*
typedef struct
{
    char bUsed;
    char bVideoRequested;
    char bAudioRequested;
    char bRecordPlay;

    char bSpeakerStart;
    char encrypt;
    char record_crtl_refreshed;
    char file_switch;

    char force_i_frame;
    char use_test_auth;
    char calc_bitrate;
    char resolution_switch_counter;
    char first_i_frame_sended;
    char view_state;
    char tnp_ver;
    char reserved[1];

    short width;
    short height;

    unsigned short video_seq;
    unsigned short audio_seq;

    int SessionHandle;
    int usecount;
    int resolution;
    int pre_resolution;
    int auto_resolution;
    int record_speed;
    int pre_record_speed;
    int video_index;
    int audio_index;
    int i_frame_left_size;
    unsigned int cur_ts;
    unsigned int max_buff_size;
    unsigned int videoStartTime;
    time_t replay_time;
    unsigned int playDuration;
    struct timeval g_tv;

    mp4trackinfo mp4info;
    SMsgAVIoctrlPlayRecord record_ctrl;
    p2p_nonce_t p2p_nonce;

    char password[20];
    unsigned char *buff;
    unsigned short fshare_read_mask;
    unsigned char vps[40];
    int vps_len;
    unsigned char sps[60];
    int sps_len;
    unsigned char pps[20];
    int pps_len;
} st_User;
*/

typedef struct
{
    mqd_t mqfd_dispatch;
    mqd_t mq_p2ptnp;

    mmap_info_s* mmap_info;

    int cur_usr;
//    st_User gUser[MAX_SESSION_NUM];
    int viewer_table[MAX_VIEWING_NUM];
    int user_num;
    int max_user_num;
} g_p2ptnp_info_s;

typedef enum {
    DISPATCH_SET_TIME = 0x71,
    DISPATCH_SET_BIND_FAIL = 0x73,
    DISPATCH_SET_POWER_ON = 0x74,
    DISPATCH_SET_POWER_OFF = 0x75,
    DISPATCH_SET_LIGHT_ON = 0x76,
    DISPATCH_SET_LIGHT_OFF = 0x77,
    DISPATCH_SET_MOTION_RCD = 0x78,
    DISPATCH_SET_ALWAYS_RCD = 0x79,
    DISPATCH_SET_MIRROR_ON = 0x7a,
    DISPATCH_SET_MIRROR_OFF = 0x7b,
    DISPATCH_MOTION_DETECT_START = 0x7c,
    DISPATCH_MOTION_DETECT_STOP = 0x7d,
    DISPATCH_SET_TNP_INIT_STATUS = 0x7f,
    DISPATCH_SET_TNP_WORK_MODE = 0x80,
    DISPATCH_SET_SD_FORMAT = 0x85,
    DISPATCH_SET_DEBUG_MODE = 0x86,
    DISPATCH_SET_VIDEO_BACKUP_STATE = 0x87,
    DISPATCH_SET_BAN_DEVICE = 0x88,
    DISPATCH_SYNC_INFO_FROM_SERVER = 0x89,
    DISPATCH_FINISH_SYNC_INFO_FROM_SERVER = 0x8a,
    DISPATCH_SET_CLOUD_STORAGE_ON = 0x8b,
    DISPATCH_SET_CLOUD_STORAGE_OFF = 0x8c,
    DISPATCH_SET_REGION = 0x8d,
    DISPATCH_SET_TZ_OFFSET = 0x8e,
    DISPATCH_SET_PWD_USED_CNT = 0x8f,
    DISPATCH_SET_PWD_SYNC_SERVER = 0x90,
    DISPATCH_SET_OTA_STATE = 0x91,
    DISPATCH_SET_PANORAMA_CAPTURE_STATE = 0x92,
    DISPATCH_SET_PANORAMA_CAPTURE_COUNT = 0x93,
    DISPATCH_SET_CLOUD_STORAGE_MODE_MOTION_DETECT = 0x94,
    DISPATCH_SET_CLOUD_STORAGE_MODE_ALL_DAY = 0x95,
    DISPATCH_SAVE_API_SERVER = 0x96,
    DISPATCH_CLEAR_API_SERVER = 0x97,
    DISPATCH_SET_BIND_TIMEOUT = 0x98,
    DISPATCH_SPEAK_WRONG_DEVICE = 0x99,
    DISPATCH_SET_LAPSE_VIDEO = 0x9a,
    DISPATCH_SET_LAPSE_VIDEO_MODIFY = 0x9b,
    DISPATCH_P2P_CONNECTTED = 0xe1,
    DISPATCH_P2P_DISCONNECTTED = 0xe2,
    DISPATCH_P2P_VIEWING = 0xe3,
    DISPATCH_P2P_STOP_VIEWING = 0xe4,
    DISPATCH_P2P_CLR_VIEWING = 0xe5,
    DISPATCH_MOTION_TRACKING_DETECT_START = 0xe6,
    DISPATCH_BABYCRY_OCCUR = 0xea,
    DISPATCH_BABYCRY_CLEAR = 0xeb,
    DISPATCH_SET_SCHEDULE_POWER = 0xec,
    DISPATCH_HUMAN_DETECT_START = 0xed,
    DISPATCH_ABNORMAL_SOUND_OCCUR = 0xee,
    DISPATCH_ABNORMAL_SOUND_CLEAR = 0xef,
    DISPATCH_CLOUD_PIC_INDEX_OCCUR = 0xf0,
    DISPATCH_CLOUD_PIC_INDEX_CLEAR = 0xf1,
    DISPATCH_CLOUD_PIC_SET_INTERVAL = 0xf2,
    DISPATCH_SET_CLOUD_IMAGE_ON = 0xf3,
    DISPATCH_SET_CLOUD_IMAGE_OFF = 0xf4,
    DISPATCH_FACE_DETECT_START = 0xf7,
    RMM_START_CAPTURE = 0x1007,
    RMM_SPEAK_BAN_DEVICE = 0x100d,
    RMM_SET_MOTION_DETECT = 0x1023,
    RMM_SET_DAY_NIGHT_MODE = 0x1024,
    RMM_SET_MOTION_SENSITIVITY = 0x1027,
    RMM_SET_LDC = 0x1028,
    RMM_SET_BABY_CRY = 0x1029,
    RMM_SET_MIC_VOLUME = 0x102a,
    RMM_START_PANORAMA_CAPTURE = 0x102b,
    RMM_ABORT_PANORAMA_CAPTURE = 0x102c,
    RMM_SET_ENCODE_MODE = 0x1032,
    RMM_SET_HIGH_RESOLUTION = 0x1033,
    RMM_SET_LAPSE_FRAMERATE = 0x1036,
    RMM_SET_LAPSE_FORCE_IDR = 0x1037,
    RMM_APP_AUDIO_MODE_SIMPLEX = 0x1038,
    RMM_APP_AUDIO_MODE_DUPLEX = 0x1039,
    RMM_SET_ABNORMAL_SOUND = 0x103a,
    RMM_SET_ABNORMAL_SOUND_SENSITIVITY = 0x103b,
    RMM_SET_HUMAN_MOTION = 0x103c,
    RCD_START_SHORT_VIDEO = 0x2000,
    RCD_START_SHORT_VIDEO_10S = 0x2001,
    RCD_START_VOICECMD_VIDEO = 0x2002,
    RCD_START_SHORT_FACE_VIDEO = 0x2003,
    RCD_START_SHORT_HUMAN_VIDEO = 0x2004,
    DISPATCH_SET_VIEWPOINT_TRACE = 0x3002,
    DISPATCH_SET_VOICE_CTRL = 0x3003,
    DISPATCH_P2P_PTZ_PRESET_ADD = 0x4000,
    DISPATCH_P2P_PTZ_PRESET_DEL = 0x4001,
    DISPATCH_P2P_PTZ_SET_CURISE_STAY_TIME = 0x4003,
    DISPATCH_P2P_PTZ_CRUISE_START = 0x4004,
    DISPATCH_P2P_PTZ_CRUISE_STOP = 0x4005,
    DISPATCH_P2P_PTZ_SET_CURISE_PERIOD = 0x400a,
    DISPATCH_P2P_PTZ_SET_MOTION_TRACK = 0x400b,
    CLOUD_DEBUG_ALARM = 0x6006
} MSG_TYPE;

typedef struct
{
    int dstMid;
    int srcMid;
    short mainOp;
    short subOp;
    int msgLength;
} mqueue_msg_header_t;

typedef struct
{
    int top_left_x;
    int top_left_y;
    int bottom_right_x;
    int bottom_right_y;
} rect_t;

typedef struct
{
    int mode;
    int resolution;
    rect_t rect;
} SMsAVIoctrlMotionDetectCfg;

typedef struct
{
    char fileName[64];
    time_t eventTime;
} rcd_short_video_info_t;

typedef enum
{
    E_NORMAL_TYPE = 0,
    E_FACE_TYPE,
    E_HUMAN_TYPE,
    E_BUTT
} e_short_video_type;

typedef enum {
    REGION_CHINA = 0x1,
    REGION_SOUTHEAST_ASIA = 0x9,
    REGION_EUROPE = 0x10,
    REGION_AMERICA = 0x11
} REGION_ID;

typedef enum {
    LANG_DEFAULT = 0x3
//    ...
} LANG_TYPE;

typedef struct
{
    REGION_ID region_id;
    LANG_TYPE language;
    char api_server[128];
    char sname[64];
    char dlproto[64];
} set_region_msg;

typedef enum {
    MOTION_TYPE_MOTION = 0x0,
    MOTION_TYPE_MOTION_TRACKING = 0x1,
    MOTION_TYPE_HUMAN = 0x2,
    MOTION_TYPE_FACE = 0x7
} MOTION_TYPE;

void dump_string(char *source_file, const char *func, int line, char *text, ...);
int p2p_send_msg(mqd_t mqfd, MSG_TYPE msg_type, char *payload, int payload_len);
int cloud_send_msg(mqd_t mqfd, MSG_TYPE msg_type, char *payload, int payload_len);

#endif
