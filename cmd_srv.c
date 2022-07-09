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
#include <limits.h>
#include <errno.h>
#include "cmd_set.h"
#include "cmd_get.h"
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

int cloud_send_msg(mqd_t mqfd, MSG_TYPE msg_type, char *payload, int payload_len)
{
    mqueue_msg_header_t MsgHead;
    char send_buf[1024] = {0};
    int send_len = 0;
    int fsMsgRet = 0;

    memset(&MsgHead, 0, sizeof(MsgHead));
    MsgHead.srcMid = MID_CLOUD;
    MsgHead.mainOp = msg_type;
    MsgHead.subOp = 1;
    MsgHead.msgLength = payload_len;

    switch(msg_type)
    {
    case RCD_START_SHORT_VIDEO:
    case RCD_START_SHORT_VIDEO_10S:
    case RCD_START_VOICECMD_VIDEO:
        MsgHead.dstMid = MID_RCD;
        break;
    case RMM_START_CAPTURE:
    case RMM_SPEAK_BAN_DEVICE:
    case RMM_START_PANORAMA_CAPTURE:
    case RMM_ABORT_PANORAMA_CAPTURE:
        MsgHead.dstMid = MID_RMM;
        break;
    case CLOUD_DEBUG_ALARM:
        MsgHead.dstMid = MID_CLOUD;
        break;
    default:
        MsgHead.dstMid = MID_DISPATCH;
        break;
    }

    memcpy(send_buf, &MsgHead, sizeof(MsgHead));
    if(NULL!=payload && payload_len>0)
    {
        memcpy(send_buf + sizeof(MsgHead), payload, payload_len);
    }
    send_len = sizeof(MsgHead) + payload_len;

    fsMsgRet = mqueue_send(mqfd, send_buf, send_len);

    return fsMsgRet;
}

/* Only positive numbers */
int str2int(char *value)
{
    char *endptr;
    int errno;
    int ret;

    errno = 0;    /* To distinguish success/failure after call */

    ret = strtol(value, &endptr, 10);

    /* Check for various possible errors */
    if ((errno == ERANGE && (ret == LONG_MAX || ret == LONG_MIN)) || (errno != 0)) {
        return -1;
    }
    if (endptr == optarg) {
        return -1;
    }

    return ret;
}

/* Only positive numbers */
int str2motion_rect(motion_rect_t *mr, char *value)
{
    char *p = strtok (value, ",");
    int el;
    int array[6];
    int i;

    while (p != NULL)
    {
        el = str2int(p);
        if (el == -1) return -1;
        array[i++] = el;
        p = strtok (NULL, ".");
    }
    mr->mode = array[0];
    mr->resolution = array[1];
    mr->left = array[2];
    mr->top = array[3];
    mr->right = array[4];
    mr->bottom = array[5];

}

void print_usage(char *progname)
{
    fprintf(stderr, "\nUsage: %s OPTIONS\n\n", progname);
    fprintf(stderr, "\t-s PARAM, --set PARAM\n");
    fprintf(stderr, "\t\tset PARAM to value\n");
    fprintf(stderr, "\t-g PARAM, --get PARAM\n");
    fprintf(stderr, "\t\tget PARAM value\n");
    fprintf(stderr, "\t-v VALUE, --value VALUE\n");
    fprintf(stderr, "\t\tvalue to set\n");
    fprintf(stderr, "\t-d, --debug\n");
    fprintf(stderr, "\t\tenable debug\n");
    fprintf(stderr, "\t-l, --list\n");
    fprintf(stderr, "\t\tprint all available parameters\n");
    fprintf(stderr, "\t-h, --help\n");
    fprintf(stderr, "\t\tprint this help\n");
}

int main(int argc, char **argv)
{
    int errno;
    char *endptr;
    int c;

    int set = 0;
    char param[1024];
    char value[1024] = {0};
    int ivalue;
    int debug = 0;

    while (1) {
        static struct option long_options[] =
        {
            {"set",  required_argument, 0, 's'},
            {"get",  required_argument, 0, 'g'},
            {"value",  required_argument, 0, 'v'},
            {"list",  no_argument, 0, 'l'},
            {"debug",  no_argument, 0, 'd'},
            {"help",  no_argument, 0, 'h'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "s:g:v:ldh",
                         long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c) {
        case 's':
        case 'g':
            if (c == 's') set = 1;
            if (strlen(optarg) < sizeof(param) - 1) {
                strcpy(param, optarg);
            } else {
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            break;

//            errno = 0;    /* To distinguish success/failure after call */
//
//            mic_volume = strtol(optarg, &endptr, 10);
//
//            /* Check for various possible errors */
//            if ((errno == ERANGE && (mic_volume == LONG_MAX || mic_volume == LONG_MIN)) || (errno != 0)) {
//                print_usage(argv[0]);
//                exit(EXIT_FAILURE);
//            }
//            if (endptr == optarg) {
//                print_usage(argv[0]);
//                exit(EXIT_FAILURE);
//            }
//            break;

        case 'v':
            if (strlen(optarg) < sizeof(value) - 1) {
                strcpy(value, optarg);
            } else {
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
            }
            break;

        case 'l':
            printf("List of parameters:\n");
            printf("\tmic_volume\n");
            printf("\tpower_mode\n");
            printf("\tlight_mode\n");
            printf("\trecord_mode\n");
            printf("\tmirror\n");
            printf("\tmotion_rect\n");
            printf("\thuman_motion_enable\n");
            printf("\tirlight_mode\n");
            printf("\tmotion_sensitivity\n");
            printf("\ttz_offset\n");
            exit(EXIT_SUCCESS);
            break;

        case 'd':
            fprintf (stderr, "debug on\n");
            debug = 1;
            break;

        case 'h':
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
            break;

        case '?':
            /* getopt_long already printed an error message. */
            break;

        default:
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
        }
    }

    if (argc == 1) {
        print_usage(argv[0]);
        exit(EXIT_SUCCESS);
    }
    if ((set == 1) && (value[0] == '\0')) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if((g_p2ptnp_info.mmap_info = (mmap_info_s *) get_sharemem(MMAP_FILE_NAME, sizeof(mmap_info_s))) == NULL)
    {
        dump_string(_F_, _FU_, _L_, "open share mem fail!\n");
        return -1;
    }

    ipc_mq = mq_open("/ipc_dispatch", O_RDWR);
    if (ipc_mq == -1) {
        dump_string(_F_, _FU_, _L_, "open queue fail!\n");
        munmap(g_p2ptnp_info.mmap_info, sizeof(mmap_info_s));
        return -2;
    }
    printf("Size of mmap.info: %d\n", sizeof(mmap_info_s));

    if (set == 1) {
        if (strcmp("mic_volume", param) == 0) {
            ivalue = str2int(value);
            if (ivalue == -1) {
                printf("Invalid value: %s\n", value);
                return -2;
            }
            p2p_set_mic_volume(ivalue);
        } else if (strcmp("power_mode", param) == 0) {
            ivalue = str2int(value);
            if (ivalue == -1) {
                printf("Invalid value: %s\n", value);
                return -2;
            }
            p2p_set_power(ivalue);
        } else if (strcmp("light_mode", param) == 0) {
            ivalue = str2int(value);
            if (ivalue == -1) {
                printf("Invalid value: %s\n", value);
                return -2;
            }
            p2p_set_light(ivalue);
        } else if (strcmp("record_mode", param) == 0) {
            ivalue = str2int(value);
            if (ivalue == -1) {
                printf("Invalid value: %s\n", value);
                return -2;
            }
            p2p_set_motion_record(ivalue);
        } else if (strcmp("mirror", param) == 0) {
            ivalue = str2int(value);
            if (ivalue == -1) {
                printf("Invalid value: %s\n", value);
                return -2;
            }
            if (ivalue == 1)
                p2p_set_mirror_flip(DISPATCH_SET_MIRROR_ON);
            else
                p2p_set_mirror_flip(DISPATCH_SET_MIRROR_OFF);

        } else if (strcmp("motion_rect", param) == 0) {
            motion_rect_t motion_rect;
            ivalue = str2motion_rect(&motion_rect, value);
            if (ivalue == -1) {
                printf("Invalid value: %s\n", value);
                return -2;
            }
            p2p_set_motion_detect(motion_rect);
        } else if (strcmp("human_motion_enable", param) == 0) {
            ivalue = str2int(value);
            if (ivalue == -1) {
                printf("Invalid value: %s\n", value);
                return -2;
            }
            p2p_set_alarm_mode(ivalue);
        } else if (strcmp("irlight_mode", param) == 0) {
            ivalue = str2int(value);
            if (ivalue == -1) {
                printf("Invalid value: %s\n", value);
                return -2;
            }
            p2p_set_day_night_mode(ivalue);
        } else if (strcmp("motion_sensitivity", param) == 0) {
            ivalue = str2int(value);
            if (ivalue == -1) {
                printf("Invalid value: %s\n", value);
                return -2;
            }
            p2p_set_alarm_sensitivity(ivalue);
        } else if (strcmp("tz_offset", param) == 0) {
            ivalue = str2int(value);
            if (ivalue == -1) {
                printf("Invalid value: %s\n", value);
                return -2;
            }
            cloud_set_tz_offset(ivalue);
        } else {
            printf("Invalid parameter: %s\n", param);
        }
    } else {
        if (strcmp("mic_volume", param) == 0) {
            p2p_get_mic_volume();
        } else if (strcmp("power_mode", param) == 0) {
            p2p_get_power();
        } else if (strcmp("light_mode", param) == 0) {
            p2p_get_light();
        } else if (strcmp("record_mode", param) == 0) {
            p2p_get_motion_record();
        } else if (strcmp("mirror", param) == 0) {
            p2p_get_mirror_flip();
        } else if (strcmp("motion_rect", param) == 0) {
            p2p_get_motion_detect();
        } else if (strcmp("human_motion_enable", param) == 0) {
            p2p_get_alarm_mode();
        } else if (strcmp("irlight_mode", param) == 0) {
            p2p_get_day_night_mode();
        } else if (strcmp("motion_sensitivity", param) == 0) {
            p2p_get_alarm_sensitivity();
        } else if (strcmp("tz_offset", param) == 0) {
            cloud_get_tz_offset();
        } else {
            printf("Invalid parameter: %s\n", param);
        }
    }

    mq_close(ipc_mq);
    munmap(g_p2ptnp_info.mmap_info, sizeof(mmap_info_s));

    return 0;
}
