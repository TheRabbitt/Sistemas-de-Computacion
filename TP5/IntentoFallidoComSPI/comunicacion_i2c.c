#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define I2C_ADDRESS 0x08  // Dirección de Arduino

int main() {
    int file;
    char filename[20];
    int adapter_nr = 1; // Puede variar dependiendo del bus I2C utilizado

    snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
    file = open(filename, O_RDWR);
    if (file < 0) {
        printf("No se pudo abrir el bus I2C\n");
        return 1;
    }

    if (ioctl(file, I2C_SLAVE, I2C_ADDRESS) < 0) {
        printf("Error al comunicarse con el dispositivo I2C\n");
        return 1;
    }

    while (1) {
        unsigned short value1, value2;
        unsigned char data[4];

        // Leer los valores analógicos desde Arduino
        int bytesRead = read(file, data, 4);
        if (bytesRead != 4) {
            printf("Error al leer los datos I2C. Bytes leídos: %d\n", bytesRead);
            perror("Mensaje de error");
            return 1;
        }

        value1 = (data[0] << 8) | data[1];  // Reconstruir el valor 1
        value2 = (data[2] << 8) | data[3];  // Reconstruir el valor 2

        printf("Valor analógico 1: %u\n", value1);
        printf("Valor analógico 2: %u\n", value2);

        usleep(1000000);  // Esperar 1 segundo antes de la siguiente lectura
    }

    close(file);
    return 0;
}
