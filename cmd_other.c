#include <string.h>
#include "cmd_srv.h"

extern mqd_t ipc_mq;

int cloud_cap_pic(char *pic_name)
{
    if(cloud_send_msg(ipc_mq, RMM_START_CAPTURE, pic_name, strlen(pic_name)) < 0)
    {
        dump_string(_F_, _FU_, _L_,  "cloud_cap_pic send_msg fail!\n");
        return -1;
    }
    else
    {
        dump_string(_F_, _FU_, _L_,  "cloud_cap_pic send_msg ok!\n");
        return 0;
    }
}

int cloud_make_video(char *video_name, int second, e_short_video_type short_video_type, time_t eventTime)
{
    MSG_TYPE msg_type;
    rcd_short_video_info_t rcdInfo;
    if(second == 6)
    {
        if(E_NORMAL_TYPE == short_video_type)
        {
            msg_type = RCD_START_SHORT_VIDEO;
        }
        else if(E_FACE_TYPE == short_video_type)
        {
            msg_type = RCD_START_SHORT_FACE_VIDEO;
        }
        else
        {
            msg_type = RCD_START_SHORT_HUMAN_VIDEO;
        }
    }
    else if( second == VC_RECORD_DURATION )
    {
        msg_type = RCD_START_VOICECMD_VIDEO;
    }
    else
    {
        msg_type = RCD_START_SHORT_VIDEO_10S;
    }

    strncpy(rcdInfo.fileName, video_name, sizeof(rcdInfo.fileName));
    rcdInfo.fileName[63] = 0;
    rcdInfo.eventTime = eventTime;

    if(cloud_send_msg(ipc_mq, msg_type, (char *)&rcdInfo, sizeof(rcdInfo)) < 0)
    {
        dump_string(_F_, _FU_, _L_,  "cloud_make_video %d(s) send_msg fail!\n", second);
        return -1;
    }
    else
    {
        dump_string(_F_, _FU_, _L_,  "cloud_make_video %d(s)send_msg ok!\n", second);
        return 0;
    }
}
