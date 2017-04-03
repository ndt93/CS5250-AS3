#include "fourmb.h"

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    char user_msg[MSG_MAX_LEN + 1];
    int fd = open("/dev/fourmb", O_RDWR);

    const char *old_msg = "Old hello\n";
    strcpy(user_msg, old_msg);
    ioctl(fd, IOCTL_SET_MSG, user_msg);

    const char *new_msg = "New hello\n";
    strcpy(user_msg, new_msg);
    ioctl(fd, IOCTL_SWP_MSG, user_msg);

    printf("user_msg: %s", user_msg);

    close(fd);
    return 0;
}
