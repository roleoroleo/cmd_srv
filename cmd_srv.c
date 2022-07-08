#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <mqueue.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include "cmd_srv.h"

#define MMAP_SIZE 2508
#define MMAP_FILE_NAME "/tmp/mmap.info"

g_p2ptnp_info_s g_p2ptnp_info;
mqd_t ipc_mq;
unsigned char *mmap_file; // memory mapped file

void dump_string(char *source_file, const char *func, int line, char *text, ...)
{
    time_t now;
    struct tm *s_now;
    struct timeval tv;

    if (text == '\0') {
        return;
    }

    time(&now);
    s_now = localtime(&now);
    if (s_now != 0) {
        gettimeofday((struct timeval *) &tv, NULL);
        printf("\n%s(%s-%d)[%02d:%02d:%02d.%03d]:", source_file, func, line, s_now->tm_hour,
                s_now->tm_min, s_now->tm_sec, tv.tv_usec >> 10);
        va_list args;
        va_start (args, text);
        vfprintf(stdout, text, args);
        va_end (args);
        putchar(10);
    }
    return;
}

void *get_sharemem(char *filename, size_t filesize)
{
    int fd;
    void *map;
    uint8_t attempts = 10;

    do
    {
        fd = open(filename, 0);
        if (fd != -1)
        {
            map = mmap(NULL, filesize, 1, 1, fd, 0);
            if (map == MAP_FAILED)
            {
                close(fd);
                return NULL;
            }
            close(fd);
            return map;
        }
        attempts = attempts - 1;
    } while (attempts != 0);

    return NULL;
}

// Modified function from rRTSPServer
void cpy_from_mmap(unsigned char *dest, int src, size_t n)
{
    unsigned char *offset = mmap_file + src;

    if (offset + n > mmap_file + MMAP_SIZE)
    {
        memcpy(dest, offset, MMAP_SIZE - (offset - mmap_file));
        memcpy(dest + MMAP_SIZE - (offset - mmap_file), mmap_file, n - (MMAP_SIZE - (offset - mmap_file)));
    }
    else
    {
        memcpy(dest, offset, n);
    }
}

int mqueue_send(mqd_t mqfd,char *send_buf, size_t send_len)
{
    int iRet;

    if (send_len > 512 || mqfd == -1) {
        return -1;
    }
    iRet = mq_send(mqfd, send_buf, send_len, 1);

    return iRet;
}

int p2p_send_msg(mqd_t mqfd, MSG_TYPE msg_type, char *payload, int payload_len)
{
    mqueue_msg_header_t MsgHead;
    char send_buf[1024] = {0};
    int send_len = 0;
    int fsMsgRet = 0;

    memset(&MsgHead, 0, sizeof(MsgHead));
    MsgHead.srcMid = MID_P2P;
    MsgHead.mainOp = msg_type;
    MsgHead.subOp = 1;
    MsgHead.msgLength = payload_len;

    // This isn't complete and needs to be updated to match all the types
    switch (msg_type)
    {
    case DISPATCH_SET_LIGHT_ON:
    case DISPATCH_SET_LIGHT_OFF:
    case DISPATCH_SET_POWER_ON:
    case DISPATCH_SET_POWER_OFF:
    case DISPATCH_SET_MOTION_RCD:
    case DISPATCH_SET_ALWAYS_RCD:
    case DISPATCH_SET_MIRROR_ON:
    case DISPATCH_SET_MIRROR_OFF:
    case RMM_SET_DAY_NIGHT_MODE:
    case RMM_SET_MOTION_DETECT:
    case RMM_SET_LDC:
    case RMM_SET_BABY_CRY:
    case RMM_SET_MIC_VOLUME:
    case RMM_SET_ENCODE_MODE:
    case RMM_SET_HIGH_RESOLUTION:
    case RMM_SET_ABNORMAL_SOUND:
    case RMM_SET_ABNORMAL_SOUND_SENSITIVITY:
        MsgHead.dstMid = MID_RMM;
        break;
    default:
        MsgHead.dstMid = MID_DISPATCH;
        break;
    }

    memcpy(send_buf, &MsgHead, sizeof(MsgHead));

    if ((NULL != payload) && (payload_len > 0))
    {
        memcpy(send_buf + sizeof(MsgHead), payload, payload_len);
    }

    send_len = sizeof(MsgHead) + payload_len;

    fsMsgRet = mqueue_send(mqfd, send_buf, send_len);

    return fsMsgRet;
}

int p2p_set_power(MSG_TYPE msg_type)
{
    // Set silent mode
    int count_down = 0;
    char value = -1;

    if (msg_type != DISPATCH_SET_POWER_ON && msg_type != DISPATCH_SET_POWER_OFF)
    {
        return -1;
    }

    if (p2p_send_msg(ipc_mq, msg_type, NULL, 0) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_power send_msg fail!\n");
        return -1;
    }
    else
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_power send_msg ok!\n");

        count_down = 10;
        while (count_down)
        {
            count_down--;
            if (g_p2ptnp_info.mmap_info->power_mode == value)
            {
                return 0;
            }
            usleep(100 * 1000);
        }

        return 0;
    }
}

int p2p_set_light(MSG_TYPE msg_type)
{
    int count_down = 0;
    char value = -1;

    if (msg_type != DISPATCH_SET_LIGHT_ON && msg_type != DISPATCH_SET_LIGHT_OFF)
    {
        return -1;
    }

    if (p2p_send_msg(ipc_mq, msg_type, NULL, 0) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_light send_msg fail!\n");
        return -1;
    }
    else
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_light send_msg ok!\n");

        count_down = 10;
        while (count_down)
        {
            count_down--;
            if (g_p2ptnp_info.mmap_info->light_mode == value)
            {
                return 0;
            }
            usleep(100 * 1000);
        }

        return 0;
    }
}

int p2p_set_motion_record(MSG_TYPE msg_type)
{
    int count_down = 0;
    char value = -1;

    if (msg_type != DISPATCH_SET_MOTION_RCD && msg_type != DISPATCH_SET_ALWAYS_RCD)
    {
        return -1;
    }

    if (p2p_send_msg(ipc_mq, msg_type, NULL, 0) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_motion_record send_msg fail!\n");
        return -1;
    }
    else
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_motion_record send_msg ok!\n");

        count_down = 10;
        while (count_down)
        {
            count_down--;
            if (g_p2ptnp_info.mmap_info->record_mode == value)
            {
                return 0;
            }
            usleep(100 * 1000);
        }

        return 0;
    }
}

int p2p_set_mirror_flip(MSG_TYPE msg_type)
{
    int count_down = 0;
    char value = -1;

    if (msg_type != DISPATCH_SET_MIRROR_ON && msg_type != DISPATCH_SET_MIRROR_OFF)
    {
        return -1;
    }

    if (p2p_send_msg(ipc_mq, msg_type, NULL, 0) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_mirror_flip send_msg fail!\n");
        return -1;
    }
    else
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_mirror_flip send_msg ok!\n");

        count_down = 10;
        while (count_down)
        {
            count_down--;
            if (g_p2ptnp_info.mmap_info->mirror == value)
            {
                return 0;
            }
            usleep(100 * 1000);
        }

        return 0;
    }
}

int p2p_set_motion_detect(int index, motion_rect_t motion_rect, int nIOCtrlCmdNum)
{
    int cnt_down = 0;
//    motion_rect_t motion_rect;
    int ret = 0;

    // motion_rect.mode = ntohl(p_motion_cfg->mode);
    // motion_rect.resolution = ntohl(p_motion_cfg->resolution);

    // motion_rect.left = ntohl(p_motion_cfg->rect.top_left_x);
    // motion_rect.top = 0 - ntohl(p_motion_cfg->rect.top_left_y);
    // motion_rect.right = ntohl(p_motion_cfg->rect.bottom_right_x);
    // motion_rect.bottom = 0 - ntohl(p_motion_cfg->rect.bottom_right_y);

    if (p2p_send_msg(ipc_mq, RMM_SET_MOTION_DETECT, (char *)&motion_rect, sizeof(motion_rect_t)) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_motion_detect send_msg fail!\n");
        return -1;
    }
    else
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_motion_detect send_msg ok!\n");

        cnt_down = 10;
        while (cnt_down)
        {
            if (g_p2ptnp_info.mmap_info->motion_rect.mode == motion_rect.mode &&
                g_p2ptnp_info.mmap_info->motion_rect.resolution == motion_rect.resolution &&
                g_p2ptnp_info.mmap_info->motion_rect.left == motion_rect.left &&
                g_p2ptnp_info.mmap_info->motion_rect.top == motion_rect.top &&
                g_p2ptnp_info.mmap_info->motion_rect.right == motion_rect.right &&
                g_p2ptnp_info.mmap_info->motion_rect.bottom == motion_rect.bottom)
            {
                break;
            }

            cnt_down--;
            usleep(100 * 1000);
        }
    }

    return 0;
}

int p2p_get_motion_detect(int index, int nIOCtrlCmdNum)
{
    SMsAVIoctrlMotionDetectCfg Rsp;
    int ret = 0;

    memset(&Rsp, 0, sizeof(SMsAVIoctrlMotionDetectCfg));
    Rsp.mode = htonl(g_p2ptnp_info.mmap_info->motion_rect.mode);
    Rsp.resolution = htonl(g_p2ptnp_info.mmap_info->motion_rect.resolution);
    Rsp.rect.top_left_x = htonl(g_p2ptnp_info.mmap_info->motion_rect.left);
    Rsp.rect.top_left_y = htonl(0 - g_p2ptnp_info.mmap_info->motion_rect.top);
    Rsp.rect.bottom_right_x = htonl(g_p2ptnp_info.mmap_info->motion_rect.right);
    Rsp.rect.bottom_right_y = htonl(0 - g_p2ptnp_info.mmap_info->motion_rect.bottom);

    return ret;
}

int p2p_set_alarm_mode(int alarm_mode)
{
    int count_down = 0;

    if (p2p_send_msg(ipc_mq, RMM_SET_HUMAN_MOTION, (char *)&alarm_mode, sizeof(alarm_mode)) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_alarm_mode %d send_msg fail!\n", alarm_mode);
        return -1;
    }
    else
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_alarm_mode %d send_msg ok!\n", alarm_mode);

        count_down = 10;
        while (count_down)
        {
            count_down--;
            if (g_p2ptnp_info.mmap_info->human_motion_enable == alarm_mode)
            {
                break;
            }
            usleep(100 * 1000);
        }

        return 0;
    }
}

int p2p_set_day_night_mode(MSG_TYPE msg_type, int value)
{
    int count_down = 0;

    if (msg_type != RMM_SET_DAY_NIGHT_MODE)
    {
        return -1;
    }

    if (p2p_send_msg(ipc_mq, msg_type, (char *)&value, sizeof(value)) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_day_night_mode %d send_msg fail!\n", value);
        return -1;
    }
    else
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_day_night_mode %d send_msg ok!\n", value);

        count_down = 10;
        while (count_down)
        {
            count_down--;
            if (g_p2ptnp_info.mmap_info->irlight_mode == value)
            {
                return 0;
            }
            usleep(100 * 1000);
        }

        return 0;
    }
}

int p2p_set_alarm_sensitivity(int sensitivity)
{
    int count_down = 0;

    if (p2p_send_msg(ipc_mq, RMM_SET_MOTION_SENSITIVITY, (char *)&sensitivity, sizeof(sensitivity)) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_alarm_sensitivity %d send_msg fail!\n", sensitivity);
        return -1;
    }
    else
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_alarm_sensitivity %d send_msg ok!\n", sensitivity);

        count_down = 10;
        while (count_down)
        {
            count_down--;
            if (g_p2ptnp_info.mmap_info->motion_sensitivity == sensitivity)
            {
                break;
            }
            usleep(100 * 1000);
        }

        return 0;
    }
}

/*
int p2p_set_video_backup_state(int index, video_backup_state_set *backup_state, int nIOCtrlCmdNum)
{
    video_backup_state_set_resp Rsp;
    int count_down = 0;

    memset(&Rsp, 0, sizeof(Rsp));

    if (backup_state->enable < 0 || backup_state->enable > 1 ||
        backup_state->backup_period < 0 || backup_state->backup_period > 3 ||
        backup_state->user_path < 0 || backup_state->user_path > 1)
    {
        Rsp.result = 1;
    }
    else
    {
        if (p2p_send_msg(ipc_mq, DISPATCH_SET_VIDEO_BACKUP_STATE, (char *)backup_state, sizeof(video_backup_state_set_resp)) < 0)
        {
            dump_string(_F_, _FU_, _L_, "p2p_set_video_backup_state fail!\n");
            return -1;
        }

        count_down = 10;
        while (count_down)
        {
            count_down--;
            if (g_p2ptnp_info.mmap_info->video_backup_info.enable == backup_state->enable &&
                g_p2ptnp_info.mmap_info->video_backup_info.resolution == backup_state->resolution &&
                g_p2ptnp_info.mmap_info->video_backup_info.backup_period == backup_state->backup_period &&
                g_p2ptnp_info.mmap_info->video_backup_info.user_path == backup_state->user_path)
            {
                break;
            }
            usleep(100 * 1000);
        }

        Rsp.result = 0;
    }

    return 0;
}
*/

int p2p_set_encode_mode(int value)
{
    int count_down = 0;

    if (p2p_send_msg(ipc_mq, RMM_SET_ENCODE_MODE, (char *)&value, sizeof(value)) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_encode_mode %d send_msg fail!\n", value);
        return -1;
    }
    else
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_encode_mode %d send_msg ok!\n", value);

        count_down = 10;
        while (count_down)
        {
            count_down--;
            if (g_p2ptnp_info.mmap_info->encode_mode == value)
            {
                return 0;
            }
            usleep(100 * 1000);
        }

        return 0;
    }
}

int p2p_set_high_resolution(int value)
{
    int count_down = 0;

    if (p2p_send_msg(ipc_mq, RMM_SET_HIGH_RESOLUTION, (char *)&value, sizeof(value)) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_high_resolution %d send_msg fail!\n", value);
        return -1;
    }
    else
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_high_resolution %d send_msg ok!\n", value);

        count_down = 10;
        while (count_down)
        {
            count_down--;
            if (g_p2ptnp_info.mmap_info->high_resolution == value)
            {
                return 0;
            }
            usleep(100 * 1000);
        }

        return 0;
    }
}

int p2p_get_video_backup_state(int index, video_backup_info_t video_backup_info)
{

    // if (g_p2ptnp_info.mmap_info->tf_status.stat == TF_CHECK_OK)
    // {
    //     Rsp.has_sd = 1;
    // }
    // else
    // {
    //     Rsp.has_sd = 0;
    // }

    video_backup_info.enable = g_p2ptnp_info.mmap_info->video_backup_info.enable;
    video_backup_info.resolution = g_p2ptnp_info.mmap_info->video_backup_info.resolution;
    video_backup_info.backup_period = g_p2ptnp_info.mmap_info->video_backup_info.backup_period;
    video_backup_info.user_path = g_p2ptnp_info.mmap_info->video_backup_info.user_path;
    video_backup_info.router_sd_total_size = htonl(g_p2ptnp_info.mmap_info->video_backup_info.router_sd_total_size);
    video_backup_info.router_sd_free_size = htonl(g_p2ptnp_info.mmap_info->video_backup_info.router_sd_free_size);
    video_backup_info.router_sd_cam_used_size = htonl(g_p2ptnp_info.mmap_info->video_backup_info.router_sd_cam_used_size);
    video_backup_info.extra_sd_total_size = htonl(g_p2ptnp_info.mmap_info->video_backup_info.extra_sd_total_size);
    video_backup_info.extra_sd_free_size = htonl(g_p2ptnp_info.mmap_info->video_backup_info.extra_sd_free_size);
    video_backup_info.extra_sd_cam_used_size = htonl(g_p2ptnp_info.mmap_info->video_backup_info.extra_sd_cam_used_size);

    return 0;
}

int p2p_get_sd_state()
{
    int ret = 0;

    if (TF_CHECK_OK == g_p2ptnp_info.mmap_info->tf_status.stat)
    {
        ret = 0;
    }
    else if (TF_SPACE_TOO_LITTLE == g_p2ptnp_info.mmap_info->tf_status.stat)
    {
        ret = 4;
    }
    else if (TF_NOT_EXIST == g_p2ptnp_info.mmap_info->tf_status.stat)
    {
        ret = 5;
    }
    else if (TF_CARD_BAD == g_p2ptnp_info.mmap_info->tf_status.stat)
    {
        ret = 3;
    }
    else if (TF_SYSTEM_FORMAT_ERROR == g_p2ptnp_info.mmap_info->tf_status.stat)
    {
        ret = 2;
    }
    else if (TF_WRITE_SPEED_SLOW == g_p2ptnp_info.mmap_info->tf_status.stat)
    {
        ret = 1;
    }

    printf("hue, p2p_get_sd_state() return (%d) \n", ret);
    return ret;
}

int p2p_set_ldc(int percent)
{
    int cnt_down = 50;
    int ret = 0;

    if (p2p_send_msg(ipc_mq, RMM_SET_LDC, (char *)&percent, sizeof(percent)) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_ldc send_msg fail!\n");
        ret = -1;
    }

    while (cnt_down)
    {
        if (g_p2ptnp_info.mmap_info->ldc_percent == percent)
        {
            break;
        }

        cnt_down--;
        usleep(100 * 1000);
    }

    return ret;
}

int p2p_set_baby_cry(int enable)
{
    int cnt_down = 50;
    int ret = 0;

    if (p2p_send_msg(ipc_mq, RMM_SET_BABY_CRY, (char *)&enable, sizeof(enable)) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_baby_cry send_msg fail!\n");
        ret = -1;
    }

    while (cnt_down)
    {
        if (g_p2ptnp_info.mmap_info->baby_cry_enable == enable)
        {
            break;
        }

        cnt_down--;
        usleep(100 * 1000);
    }

    return ret;
}

int p2p_set_abnormal_sound(int enable)
{
    int cnt_down = 50;
    int ret = 0;

    if (p2p_send_msg(ipc_mq, RMM_SET_ABNORMAL_SOUND, (char *)&enable, sizeof(enable)) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_abnormal_sound send_msg fail!\n");
        ret = -1;
    }

    while (cnt_down)
    {
        if (g_p2ptnp_info.mmap_info->abnormal_sound_enable == enable)
        {
            break;
        }

        cnt_down--;
        usleep(100 * 1000);
    }

    return ret;
}

int p2p_set_abnormal_sound_sensitivity(int value)
{
    int cnt_down = 50;
    int ret = 0;

    if (p2p_send_msg(ipc_mq, RMM_SET_ABNORMAL_SOUND_SENSITIVITY, (char *)&value, sizeof(value)) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_abnormal_sound_sensitivity send_msg fail!\n");
        ret = -1;
    }

    while (cnt_down)
    {
        if (g_p2ptnp_info.mmap_info->abnormal_sound_sensitivity == value)
        {
            break;
        }

        cnt_down--;
        usleep(100 * 1000);
    }

    return ret;
}

int p2p_set_mic_volume(int percent)
{
    int cnt_down = 50;
    int ret = 0;

    if (p2p_send_msg(ipc_mq, RMM_SET_MIC_VOLUME, (char *)&percent, sizeof(percent)) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_mic_volume send_msg fail!\n");
        ret = -1;
    }

    while (cnt_down)
    {
        if (g_p2ptnp_info.mmap_info->mic_volume == percent)
        {
            break;
        }

        cnt_down--;
        usleep(100 * 1000);
    }

    return ret;
}

int p2p_set_viewpoint_trace(unsigned char mode)
{
    int cnt_down = 50;
    int ret = 0;

    if (p2p_send_msg(ipc_mq, DISPATCH_SET_VIEWPOINT_TRACE, (char *)&mode, sizeof(mode)) < 0)
    {
        dump_string(_F_, _FU_, _L_, "p2p_set_viewpoint_trace send_msg fail!\n");
        ret = -1;
    }

    while (cnt_down)
    {
        if (g_p2ptnp_info.mmap_info->viewpoint_trace == mode)
        {
            break;
        }

        cnt_down--;
        usleep(100 * 1000);
    }

    return ret;
}

int main(int argc, char **argv)
{
    if((g_p2ptnp_info.mmap_info = (mmap_info_s *) get_sharemem(MMAP_FILE_NAME, sizeof(mmap_info_s))) == NULL)
    {
        dump_string(_F_, _FU_, _L_, "open share mem fail!\n");
        return -1;
    }

    printf("Size of mmap.info: %d\n", sizeof(mmap_info_s));

    munmap(g_p2ptnp_info.mmap_info, sizeof(mmap_info_s));

    return 0;
}
