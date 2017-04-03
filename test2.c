#include "fourmb.h"

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    const char *msg = "Hello there!\n";
    char user_msg[MSG_MAX_LEN + 1];
    int fd = open("/dev/fourmb", O_RDWR);
    int ret = ioctl(fd, IOCTL_SET_MSG, msg);
    printf("Set msg: %d\n", ret);
    ret = ioctl(fd, IOCTL_GET_MSG, user_msg);
    printf("Get msg: %d\n", ret);
    printf("%s", user_msg);
    close(fd);
    return 0;
}
