// C
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// C++
#include "hbase.h"
#include "hdir.h"
#include "hlog.h"
#include "hloop.h"
#include "hmain.h"
#include "hv.h"
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>

#include <algorithm>
#include <iostream>
#include <queue>
#include <random>
#include <string>

using namespace std;


#define FROM_SCREEN 0x0
#define FROM_SOCKET 0x1

#define UNGRAB 0x0
#define GRAB 0x1

#define UP 0x0
#define DOWN 0x1


struct input_events {
    int type;
    list<input_event> events;
};


hloop_t *loop;
hmutex_t locker;
queue<input_events> input_queue;

int screen_fd;
int screen_width;
int screen_height;
int uinput_fd;
int finger_down = 0;
int current_slot = 0;
int current_screen_slot = 0;
int finger_id[11];

void zmf(htimer_t *timer);


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
    hloge("Could not open %s, %s", dev_name, strerror(errno));
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
    if (dir == NULL)
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
            return fd;
        }
    }
    closedir(dir);
    return -1;
}

bool send_cmd(int devFd, __u16 type, __u16 code, __s32 value) {
    // return true;
    input_event event{
            .type = type, .code = code, .value = value};
    if (devFd != -1) {
        ssize_t writeSize = write(devFd, &event, sizeof(event));
        if (writeSize == sizeof(event)) {
            // printf("CMD: %d %d %d %d\n", devFd, type, code, value);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

string get_rand_str(int length) {
    char tmp;
    string buffer;
    random_device rd;
    default_random_engine random(rd());
    for (int i = 0; i < length; i++) {
        tmp = random() % 36;
        if (tmp < 10) {
            tmp += '0';
        } else {
            tmp -= 10;
            tmp += 'A';
        }
        buffer += tmp;
    }
    return buffer;
}

int create_uinput_device(int screenW, int screenH) {
    int uinput_dev_fd;
    struct uinput_user_dev uinp;
    input_event event;
    uinput_dev_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinput_dev_fd == 0) {
        hloge("Unable to open /dev/uinput\n");
        return -1;
    }
    memset(&uinp, 0, sizeof(uinp));
    strncpy(uinp.name, get_rand_str(8).c_str(), UINPUT_MAX_NAME_SIZE);
    uinp.id.version = 4;
    uinp.id.bustype = BUS_USB;
    uinp.absmin[ABS_MT_SLOT] = 0;
    uinp.absmax[ABS_MT_SLOT] = 10;
    uinp.absmin[ABS_MT_TOUCH_MAJOR] = 0;
    uinp.absmax[ABS_MT_TOUCH_MAJOR] = 15;
    uinp.absmin[ABS_MT_POSITION_X] = 0;
    uinp.absmax[ABS_MT_POSITION_X] = screenW;
    uinp.absmin[ABS_MT_POSITION_Y] = 0;
    uinp.absmax[ABS_MT_POSITION_Y] = screenH;
    uinp.absmin[ABS_MT_TRACKING_ID] = 0;
    uinp.absmax[ABS_MT_TRACKING_ID] = 65535;
    uinp.absmin[ABS_MT_PRESSURE] = 0;
    uinp.absmax[ABS_MT_PRESSURE] = 255;
    ioctl(uinput_dev_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(uinput_dev_fd, UI_SET_EVBIT, EV_REL);
    ioctl(uinput_dev_fd, UI_SET_EVBIT, EV_ABS);
    ioctl(uinput_dev_fd, UI_SET_EVBIT, EV_SYN);
    ioctl(uinput_dev_fd, UI_SET_ABSBIT, ABS_X);
    ioctl(uinput_dev_fd, UI_SET_ABSBIT, ABS_Y);
    ioctl(uinput_dev_fd, UI_SET_ABSBIT, ABS_PRESSURE);
    ioctl(uinput_dev_fd, UI_SET_ABSBIT, ABS_MT_SLOT);
    ioctl(uinput_dev_fd, UI_SET_ABSBIT, ABS_MT_TOUCH_MAJOR);
    ioctl(uinput_dev_fd, UI_SET_ABSBIT, ABS_MT_POSITION_X);
    ioctl(uinput_dev_fd, UI_SET_ABSBIT, ABS_MT_POSITION_Y);
    ioctl(uinput_dev_fd, UI_SET_ABSBIT, ABS_MT_TRACKING_ID);
    ioctl(uinput_dev_fd, UI_SET_ABSBIT, ABS_MT_PRESSURE);
    ioctl(uinput_dev_fd, UI_SET_KEYBIT, BTN_TOUCH);
    ioctl(uinput_dev_fd, UI_SET_KEYBIT, BTN_TOOL_FINGER);
    ioctl(uinput_dev_fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);

    write(uinput_dev_fd, &uinp, sizeof(uinp));
    ioctl(uinput_dev_fd, UI_DEV_CREATE);
    // printf("uinput_dev_fd: %d\n", uinput_dev_fd);
    return uinput_dev_fd;
}

list<input_event> input_event_filter(input_event *events, int size) {
    list<input_event> event;
    for (int i = 0; i < size; i++) {
        if (events[i].type == EV_ABS || events[i].type == EV_SYN) {
            event.push_back(events[i]);
        } else {
            // printf("Filter: %d %d %d\n", events[i].type, events[i].code, events[i].value);
        }
    }
    return event;
}

void on_queue_event(hevent_t *ev) {
    if (!input_queue.empty()) {
        input_events events = input_queue.front();
        input_queue.pop();
        if (events.type == FROM_SCREEN) {
            send_cmd(uinput_fd, EV_ABS, ABS_MT_SLOT, current_screen_slot);
        }
        for (input_event ev : events.events) {
            if (ev.type == EV_ABS) {
                switch (ev.code) {
                    case ABS_MT_TRACKING_ID:
                        if (events.type == FROM_SCREEN) {
                            finger_id[current_screen_slot] = ev.value;
                        } else {
                            finger_id[current_slot] = ev.value;
                        }
                        if (ev.value == -1) {
                            finger_down--;
                            if (finger_down == 0) {
                                send_cmd(uinput_fd, ev.type, ev.code, ev.value);
                                send_cmd(uinput_fd, EV_KEY, BTN_TOUCH, UP);
                                send_cmd(uinput_fd, EV_KEY, BTN_TOOL_FINGER, UP);
                                continue;
                            }
                        } else {
                            finger_down++;
                            send_cmd(uinput_fd, ev.type, ev.code, ev.value);
                            send_cmd(uinput_fd, EV_ABS, ABS_MT_PRESSURE, 80);
                            send_cmd(uinput_fd, EV_KEY, BTN_TOUCH, DOWN);
                            send_cmd(uinput_fd, EV_KEY, BTN_TOOL_FINGER, DOWN);
                            continue;
                        }
                        break;
                    case ABS_MT_SLOT:
                        if (events.type == FROM_SCREEN) {
                            current_screen_slot = ev.value;
                        } else {
                            current_slot = ev.value;
                        }
                        break;
                    default:
                        break;
                }
            }
            send_cmd(uinput_fd, ev.type, ev.code, ev.value);
        }
    }
}

hevent_t queue_event_t{
        .event_type = (hevent_type_e) (HEVENT_TYPE_CUSTOM),
        .cb = on_queue_event};

void post_queue_event() {
    hloop_post_event(loop, &queue_event_t);
}

void on_input(hio_t *io, void *buf, int readbytes) {
    input_event *event = static_cast<input_event *>(buf);
    int nums = readbytes / sizeof(input_event);
    input_events events = {
            .type = FROM_SCREEN,
            .events = input_event_filter(event, nums)};
    // printf("on_input fd=%d readbytes=%d\n", hio_fd(io), readbytes);
    // for (input_event ie : events.events) {
    //     printf("SCREEN: %d %d %d\n", ie.type, ie.code, ie.value);
    // }
    hmutex_lock(&locker);
    input_queue.push(events);
    hmutex_unlock(&locker);
    post_queue_event();
}

void sleep_ms(unsigned int secs) {
    struct timeval tval;
    tval.tv_sec = secs / 1000;
    tval.tv_usec = (secs * 1000) % 1000000;
    select(0, NULL, NULL, NULL, &tval);
}

input_event make_event(__u16 type, __u16 code, __s32 value) {
    input_event ev = {
            .type = type,
            .code = code,
            .value = value};
    return ev;
}

void touchDown(long x, long y, long finger) {
    list<input_event> event;
    event.push_back(make_event(EV_ABS, ABS_MT_SLOT, finger));
    event.push_back(make_event(EV_ABS, ABS_MT_TRACKING_ID, finger));
    event.push_back(make_event(EV_ABS, ABS_MT_POSITION_X, x));
    event.push_back(make_event(EV_ABS, ABS_MT_POSITION_Y, y));
    event.push_back(make_event(EV_SYN, 0, 0));
    input_events events = {
            .type = FROM_SOCKET,
            .events = event};
    hmutex_lock(&locker);
    input_queue.push(events);
    hmutex_unlock(&locker);
    post_queue_event();
}

void touchUp(long finger) {
    list<input_event> event;
    finger_id[finger] = -1;
    event.push_back(make_event(EV_ABS, ABS_MT_SLOT, finger));
    event.push_back(make_event(EV_ABS, ABS_MT_TRACKING_ID, -1));
    event.push_back(make_event(EV_SYN, 0, 0));
    input_events events = {
            .type = FROM_SOCKET,
            .events = event};
    hmutex_lock(&locker);
    input_queue.push(events);
    hmutex_unlock(&locker);
    post_queue_event();
}

int find_available_slot() {
    for (int i = 0; i < 10; i++) {
        if (finger_id[i] < 1) {
            return i;
        }
    }
    return -1;
}

void touchMove(long x, long y, long finger) {
    list<input_event> event;
    event.push_back(make_event(EV_ABS, ABS_MT_SLOT, finger));
    event.push_back(make_event(EV_ABS, ABS_MT_POSITION_X, x));
    event.push_back(make_event(EV_ABS, ABS_MT_POSITION_Y, y));
    event.push_back(make_event(EV_SYN, 0, 0));
    input_events events = {
            .type = FROM_SOCKET,
            .events = event};
    hmutex_lock(&locker);
    input_queue.push(events);
    hmutex_unlock(&locker);
    post_queue_event();
}


void *initTouchDevice(void *v) {
    //扫描设备触摸驱动eventX号
    screen_fd = scan_devices();

    //打开设备驱动写入
    ioctl(screen_fd, EVIOCGRAB, GRAB);

    //创建虚拟触摸设备
    uinput_fd = create_uinput_device(screen_width, screen_height);

    //初始化线程锁
    hmutex_init(&locker);

    //创建loop
    loop = hloop_new(0);
    input_event event[8];

    //创建input_event读取任务
    hread(loop, screen_fd, event, sizeof(event), on_input);

    //创建触摸定时任务
    htimer_add(loop, zmf, 20);

    //开始运行任务
    hloop_run(loop);
    //释放任务
    hloop_free(&loop);
    //释放触摸句柄
    ioctl(screen_fd, EVIOCGRAB, UNGRAB);
    close(screen_fd);
    close(uinput_fd);
    return NULL;
}

void initTouch() {
    //触摸监听
    pthread_t t1;
    pthread_create(&t1, NULL, initTouchDevice, NULL);
}
