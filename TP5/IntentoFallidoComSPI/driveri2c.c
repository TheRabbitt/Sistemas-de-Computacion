#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/uaccess.h>
#include <linux/delay.h>

#define ARDUINO_ADDRESS 0x08

static struct i2c_client *client;
static dev_t first;
static struct cdev c_dev;
static struct class *cl;

static int my_open(struct inode *i, struct file *f)
{
    struct i2c_board_info info;
    struct i2c_adapter *adap = NULL;

    adap = i2c_get_adapter(1); // Intentar obtener el adaptador I2C con dirección 1
    if (!adap) {
        adap = i2c_get_adapter(2); // Si no se encuentra el adaptador 1, intentar con el adaptador 2
    }

    if (!adap) {
        printk(KERN_ALERT "Error: No se pudo obtener el adaptador I2C\n");
        return -ENODEV;
    }

    printk(KERN_INFO "Modulo de kernel I2C abierto correctamente\n");

    strlcpy(info.type, "driveri2c", I2C_NAME_SIZE);
    info.addr = ARDUINO_ADDRESS;
    client = i2c_new_client_device(adap, &info);
    if (!client) {
        i2c_put_adapter(adap);
        printk(KERN_ALERT "Error: No se pudo crear el dispositivo I2C\n");
        return -ENODEV;
    }

    f->private_data = client;
    return 0;
}

static int my_close(struct inode *i, struct file *f)
{
    if (client) {
        i2c_unregister_device(client);
        i2c_put_adapter(client->adapter);
        client = NULL;
    }
    printk(KERN_INFO "Modulo de kernel I2C cerrado correctamente\n");
    return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    unsigned char data[4] = {0}; // Buffer para almacenar los datos leídos inicializados en cero
    unsigned short value1, value2;
    char result[50];
    struct i2c_msg msgs[1];
    struct i2c_adapter *adap = client->adapter;
    
    if (!client) {
        printk(KERN_ALERT "Error: Dispositivo I2C no inicializado\n");
        return -ENODEV;
    }
  

    // Configurar la estructura de mensaje para recibir los datos del Arduino
    msgs[0].addr = client->addr;
    msgs[0].flags = I2C_M_RD;
    msgs[0].buf = data;
    msgs[0].len = sizeof(data);

    // Realizar la operación de lectura
    if (i2c_transfer(adap, msgs, 1) < 0) {
        printk(KERN_ALERT "Error al leer desde Arduino\n");
        return -EFAULT;
    }

    // Leer los valores analógicos individuales
    value1 = (data[0] << 8) | data[1];
    value2 = (data[2] << 8) | data[3];

    sprintf(result, "%u %u\n", value1, value2);

    if (copy_to_user(buf, result, strlen(result)) != 0) {
        printk(KERN_ALERT "Error al copiar datos al espacio de usuario\n");
        return -EFAULT;
    }

    return strlen(result);
}

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read
};

static int __init mymodule_init(void)
{
    if (alloc_chrdev_region(&first, 0, 1, "MyModule") < 0) {
        return -1;
    }

    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL) {
        unregister_chrdev_region(first, 1);
        return -1;
    }

    if (device_create(cl, NULL, first, NULL, "mydevice") == NULL) {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }

    cdev_init(&c_dev, &fops);

    if (cdev_add(&c_dev, first, 1) == -1) {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    
    printk(KERN_INFO "Modulo de kernel I2C cargado\n");

    return 0;
}

static void __exit mymodule_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    my_close(NULL, NULL); // Cerrar el dispositivo I2C antes de descargar el módulo
    printk(KERN_INFO "Modulo de kernel I2C descargado\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eze,Maca,David");
MODULE_DESCRIPTION("Simple I2C communication example");
