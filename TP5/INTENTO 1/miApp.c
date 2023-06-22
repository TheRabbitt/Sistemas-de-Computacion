#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 50

int main()
{
    int fd;
    char buffer[BUFFER_SIZE];

    fd = open("/dev/mydevice", O_RDONLY);
    if (fd < 0) {
        perror("Error al abrir el dispositivo");
        return errno;
    }

    ssize_t numRead = read(fd, buffer, BUFFER_SIZE - 1);
    if (numRead < 0) {
        perror("Error al leer desde el dispositivo");
        close(fd);
        return errno;
    }

    buffer[numRead] = '\0';
    printf("Valores leídos: %s", buffer);

    close(fd);

    return 0;
}
