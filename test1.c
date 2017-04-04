#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define MB2B(mb) (1024*1024*mb)

int main()
{
    char data[MB2B(5)];
    int fd = open("/dev/fourmb", O_RDWR);

    for (int i = 0; i < MB2B(4) - 8; i++) {
        data[i] = '1';
    }
    for (int i = MB2B(4) - 8; i < MB2B(5); i++) {
        data[i] = '2';
    }

    int bytes_written = write(fd, data, MB2B(5));
    printf("Number of bytes written: %d\n", bytes_written);

    close(fd);
    return 0;
}
