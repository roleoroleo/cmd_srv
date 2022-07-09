#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "cmd_srv.h"

extern g_p2ptnp_info_s g_p2ptnp_info;

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

    printf("p2p_get_sd_state() return (%d) \n", ret);
    return ret;
}

int p2p_get_power()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->power_mode;
    printf("p2p_get_power() return (%d) \n", ret);
}

int p2p_get_light()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->light_mode;
    printf("p2p_get_light() return (%d) \n", ret);
}

int p2p_get_motion_record()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->record_mode;
    printf("p2p_get_motion_record() return (%d) \n", ret);
}

int p2p_get_mirror_flip()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->mirror;
    printf("p2p_get_mirror_flip() return (%d) \n", ret);
}

int p2p_get_mic_volume()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->mic_volume;
    printf("p2p_get_mic_volume() return (%d) \n", ret);
}

int p2p_get_motion_detect()
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
    printf("p2p_get_motion_detect() return (mode %d resolution %d: %d-%d %d-%d) \n",
            Rsp.mode, Rsp.resolution, Rsp.rect.top_left_x, Rsp.rect.top_left_y,
            Rsp.rect.bottom_right_x, Rsp.rect.bottom_right_y);

    return ret;
}

int p2p_get_alarm_mode()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->human_motion_enable;
    printf("p2p_get_alarm_mode() return (%d) \n", ret);
}

int p2p_get_day_night_mode()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->irlight_mode;
    printf("p2p_get_day_night_mode() return (%d) \n", ret);
}

int p2p_get_alarm_sensitivity()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->motion_sensitivity;
    printf("p2p_get_alarm_sensitivity() return (%d) \n", ret);
}
