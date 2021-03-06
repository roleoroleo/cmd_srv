#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "cmd_srv.h"

extern g_p2ptnp_info_s g_p2ptnp_info;

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

int p2p_get_video_backup_state()
{

    // if (g_p2ptnp_info.mmap_info->tf_status.stat == TF_CHECK_OK)
    // {
    //     Rsp.has_sd = 1;
    // }
    // else
    // {
    //     Rsp.has_sd = 0;
    // }

    video_backup_info_t video_backup_info;

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
    printf("p2p_get_video_backup_state() return\nenable %d\nresolution %d\nbackup_period %d\nuser_path %d\n"
            "router_sd_total_size %d\nrouter_sd_free_size %d\nrouter_sd_cam_used_size %d\n"
            "extra_sd_total_size %d\nextra_sd_free_size %d\nextra_sd_cam_used_size %d\n",
            video_backup_info.enable, video_backup_info.resolution,
            video_backup_info.backup_period, video_backup_info.user_path,
            video_backup_info.router_sd_total_size, video_backup_info.router_sd_free_size,
            video_backup_info.router_sd_cam_used_size, video_backup_info.extra_sd_total_size,
            video_backup_info.extra_sd_free_size, video_backup_info.extra_sd_cam_used_size);

    return 0;
}

int p2p_get_encode_mode()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->encode_mode;
    printf("p2p_get_encode_mode() return (%d) \n", ret);
}

int p2p_get_high_resolution()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->high_resolution;
    printf("p2p_get_high_resolution() return (%d) \n", ret);
}

int p2p_get_ldc()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->ldc_percent;
    printf("p2p_get_ldc() return (%d) \n", ret);
}

int p2p_get_baby_cry()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->baby_cry_enable;
    printf("p2p_get_baby_cry() return (%d) \n", ret);
}

int p2p_get_abnormal_sound()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->abnormal_sound_enable;
    printf("p2p_get_abnormale_sound() return (%d) \n", ret);
}

int p2p_get_abnormal_sound_sensitivity()
{
    int ret;
    ret = (int) g_p2ptnp_info.mmap_info->abnormal_sound_sensitivity;
    printf("p2p_get_abnornal_sound_sensitivity() return (%d) \n", ret);
}

int p2p_get_mic_volume()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->mic_volume;
    printf("p2p_get_mic_volume() return (%d) \n", ret);
}

int p2p_get_viewpoint_trace()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->viewpoint_trace;
    printf("p2p_get_viewpoint_trace() return (%d) \n", ret);
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

int p2p_get_voice_ctrl()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->voice_ctrl;
    printf("p2p_get_voice_ctrl() return (%d) \n", ret);
}

int p2p_get_lapse_video()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->lapse_video_enable;
    printf("p2p_get_lapse_video() return (%d) \n", ret);
}

int cloud_get_tz_offset()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->tz_offset;
    printf("cloud_get_tz_offset() return (%d) \n", ret);
}

int cloud_get_motion_state()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->motion_stat;
    printf("cloud_get_motion_state() return (%d) \n", ret);
}

int cloud_get_motion_type()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->motion_type;
    printf("cloud_get_motion_type() return (%d) \n", ret);
}

int cloud_get_motion_time()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->motion_time;
    printf("cloud_get_motion_time() return (%d) \n", ret);
}

int cloud_get_debug_mode()
{
    int ret;
    ret = g_p2ptnp_info.mmap_info->debug_mode;
    printf("cloud_get_debug_mode() return (%d) \n", ret);
}

int cloud_get_region()
{
    set_region_msg msg;

    msg.region_id = g_p2ptnp_info.mmap_info->region_id;
    msg.language = g_p2ptnp_info.mmap_info->language;
    snprintf(msg.api_server, sizeof(msg.api_server), "%s", g_p2ptnp_info.mmap_info->api_server);
    snprintf(msg.sname, sizeof(msg.sname), "%s", g_p2ptnp_info.mmap_info->sname);
    snprintf(msg.dlproto, sizeof(msg.dlproto), "%s", g_p2ptnp_info.mmap_info->dlproto);

    printf("p2p_get_region() return\nregion_id %d\nlanguage %d\n"
            "api_server %s\nsname %s\ndlproto %s\n",
            msg.region_id, msg.language,
            msg.api_server, msg.sname, msg.dlproto);

    return 0;
}
