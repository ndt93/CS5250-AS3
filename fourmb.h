#ifndef FOURMB_H
#define FOURMB_H

#include <linux/ioctl.h>

#define MAJOR_NUMBER 61

#define MSG_MAX_LEN 42

#define IOCTL_SET_MSG _IOW(MAJOR_NUMBER, 0, char *)

#define IOCTL_GET_MSG _IOR(MAJOR_NUMBER, 1, char *)

#define IOCTL_SWP_MSG _IOWR(MAJOR_NUMBER, 2, char *)

#endif
