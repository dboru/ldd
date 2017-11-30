#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define DEFAULT_DEVICE   "/dev/skel0"
#define MAX_BYTES 8

int main(int argc, char *argv[])
{
    char c;
    int fd;
    char *dev = DEFAULT_DEVICE;
    int    p = 0;
    uint8_t command[MAX_BYTES];
    while (1)
    {
        memset(command, 0x00, sizeof(command));
        int i;
        for (i=0;i<4;i++)
            command[i]=(p >> (3-i)) & 1;
        // command = [(p >> 3) & 1, (p >> 2) & 1, (p >> 1) & 1, (p >> 0) & 1];
        fd = open(dev, O_RDWR);
        if (fd == -1)
        {
            printf("No such file %s\n",dev);
            perror("open");
            exit(1);
        }

        int retval = 0;

        retval = write(fd, &command, sizeof(command));
        if (retval < 0)
            fprintf(stderr, "an error occured: %d\n", retval);

        char *data=(char *)calloc(MAX_BYTES, sizeof(char));
        retval=read(fd, data, MAX_BYTES);
        printf("called read(% d, c, 10).  returned that" 
            " %d bytes  were read.\n", fd, retval);
        data[retval] = '\0';
        printf("Those bytes are as follows: %s \n", data);

        p = (p + 1) % 16;

    }

    return 0;
}