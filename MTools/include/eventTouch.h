#include <string>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <hdef.h>


int finger_count = 0;
int fd_touch = 0;
int finger_id = 520;
int down = 0;

struct input_event t;
int M_BTN_TOOL_FINGER = 0;
int M_BTN_TOUCH = 0;

int screen_width;
int screen_height;

static int abs_parse(int fd) {
    uint8_t *bits = NULL;
    ssize_t bits_size = 0;
    int i, j;
    int res;
    struct label *bit_labels;
    const char *bit_label;
    int count = 0;
    while (1) {
        res = ioctl(fd, EVIOCGBIT(EV_ABS, bits_size), bits);
        if (res < bits_size)
            break;
        bits_size = res + 16;
        bits = (uint8_t *) realloc(bits, bits_size * 2);
        if (bits == NULL)
            printf("failed to allocate buffer of size %d\n", (int) bits_size);
    }
    for (i = 0; i < res; i++) {
        for (j = 0; j < 8; j++)
            if (bits[i] & 1 << j) {
                char down = ' ';
                struct input_absinfo abs;
                if (ioctl(fd, EVIOCGABS(i * 8 + j), &abs) == 0) {
                    switch (i * 8 + j) {
                        case ABS_MT_SLOT:
                            // printf("ABS_MT_SLOT");
                            break;
                        case ABS_MT_TOUCH_MAJOR:
                            //   printf("ABS_MT_TOUCH_MAJOR");
                            break;
                        case ABS_MT_TOUCH_MINOR:
                            //   printf("ABS_MT_TOUCH_MINOR");
                            break;
                        case ABS_MT_POSITION_X:
                            // printf("ABS_MT_POSITION_X");
                            break;
                        case ABS_MT_POSITION_Y:
                            //printf("ABS_MT_POSITION_Y");
                            break;
                        case ABS_MT_TRACKING_ID:
                            //  printf("ABS_MT_TRACKING_ID");
                            break;
                        case ABS_MT_DISTANCE:
                            //  printf("ABS_MT_DISTANCE");
                            break;
                    }
                    /* printf(" : value %d, min %d, max %d, fuzz %d, flat %d, resolution %d\n",
                            abs.value, abs.minimum, abs.maximum, abs.fuzz, abs.flat,
                            abs.resolution);*/
                    if (i * 8 + j == ABS_MT_POSITION_X) {
                        screen_width = abs.maximum + 1;
                    } else if (i * 8 + j == ABS_MT_POSITION_Y) {
                        screen_height = abs.maximum + 1;
                    }
                }
                count++;
            }
    }
    free(bits);
    return count;
}

int open_device(const char *dev_name) {
    int fd = open(dev_name, O_RDWR);
    if (fd > 0) {
        if (abs_parse(fd) > 0) {
            return fd;
        } else {
            close(fd);
            return -1;
        }
    }
//    hloge("Could not open %s, %s", dev_name, strerror(errno));
    return -1;
}


int scan_devices() {
    const char *dirname = "/dev/input";
    char devname[4096];
    char *filename;
    DIR *dir;
    struct dirent *de;
    int fd;
    dir = opendir(dirname);
    if (dir == nullptr)
        return -1;
    strcpy(devname, dirname);
    filename = devname + strlen(devname);
    *filename++ = '/';
    while ((de = readdir(dir))) {
        if (de->d_name[0] == '.' &&
            (de->d_name[1] == '\0' ||
             (de->d_name[1] == '.' && de->d_name[2] == '\0')))
            continue;
        strcpy(filename, de->d_name);
        fd = open_device(devname);
        if (fd > 0) {
            printf("path: %s\n", devname);
            return fd;
        }
    }
    closedir(dir);
    return -1;
}


/* 毫秒级 延时 */
void t_Sleep(int ms) {
    struct timeval delay;
    delay.tv_sec = 0;
    delay.tv_usec = ms * 1000;    // 20 ms
    select(0, NULL, NULL, NULL, &delay);
}

/* void sendEvent(__u16 type, __u16 code, int value) { struct input_event
   event; // event.time.tv_sec = 1000; event.type = type; event.code = code;
   event.value = value; write(fd_touch, &event, sizeof(event)); } */

void sendEvent(__u16 type, __u16 code, int value) {
    struct input_event event
            {
            };
    memset(&event, 0, sizeof(event));

    event.type = type;
    event.code = code;
    event.value = value;

    if (fd_touch > 0) {
        write(fd_touch, &event, sizeof(event));
    }
}

void touchMove(int x, int y, int finger_index) {
    if (M_BTN_TOUCH == 0 && M_BTN_TOOL_FINGER == 0) {
        sendEvent(EV_KEY, BTN_TOUCH, 1);
        sendEvent(EV_KEY, BTN_TOOL_FINGER, 1);
    }
    sendEvent(EV_ABS, ABS_MT_SLOT, finger_index);
    sendEvent(EV_ABS, ABS_MT_POSITION_X, x);
    sendEvent(EV_ABS, ABS_MT_POSITION_Y, y);
    sendEvent(EV_SYN, SYN_REPORT, 0);
    // sendEvent(EV_SYN, SYN_MT_REPORT, 1);
}


void touchDown(int x, int y, int finger_index) {
    finger_count++;
    sendEvent(EV_ABS, ABS_MT_SLOT, finger_index);
    sendEvent(EV_ABS, ABS_MT_TRACKING_ID, finger_id++);
    sendEvent(EV_ABS, ABS_MT_POSITION_X, x);
    sendEvent(EV_ABS, ABS_MT_POSITION_Y, y);

    /* sendEvent(EV_ABS, ABS_X, x); sendEvent(EV_ABS, ABS_Y, y); */
    if (finger_count == 1) {
        sendEvent(EV_KEY, BTN_TOUCH, 1);
    }
    sendEvent(EV_SYN, SYN_REPORT, 0);
    // sendEvent(EV_SYN, SYN_MT_REPORT, 1);
}


void touchUp(int finger_index) {
    finger_count--;
    sendEvent(EV_ABS, ABS_MT_SLOT, finger_index);
    sendEvent(EV_ABS, ABS_MT_TRACKING_ID, -1);
    // if (finger_count == 0) {
    // sendEvent(EV_KEY, BTN_TOUCH, 0);
    // }
    sendEvent(EV_SYN, SYN_REPORT, 0);
}


void initTouch() {
    fd_touch = scan_devices();
}


void closeTouch() {
    if (fd_touch > 0) {
        close(fd_touch);
    }
}


