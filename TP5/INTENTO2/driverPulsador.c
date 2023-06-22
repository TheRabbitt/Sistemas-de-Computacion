#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>

#define BUTTON1_PIN 22
#define BUTTON2_PIN 27

static dev_t first;        // Variable global para el primer número de dispositivo
static struct cdev c_dev;  // Variable global para la estructura del dispositivo de caracteres
static struct class *cl;   // Variable global para la clase del dispositivo

static int boton_select = 0;

static int my_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}

static int my_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
    char button_value;
    int bytes_read = 0;

    if (*off > 0)
        return 0;
        
    printk(KERN_INFO "boton_select: %d", boton_select);

    if (boton_select == 0) {
        button_value = '0' + gpio_get_value(BUTTON1_PIN);
    } else if (boton_select == 1) {
        button_value = '0' + gpio_get_value(BUTTON2_PIN);
    } else {
        return 0;
    }

    if (put_user(button_value, buf) != 0)
        return -EFAULT;

    *off += 1;
    bytes_read = 1;

    return bytes_read;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
    char button_selection;
    int bytes_written = 0;

    if (len != 1)
        return -EINVAL;

    if (copy_from_user(&button_selection, buf, 1) != 0)
        return -EFAULT;

    if (button_selection == '1') {
        boton_select = 0;
        printk(KERN_INFO "boton_select: %d", boton_select);
    } else if (button_selection == '2') {
        boton_select = 1;
        printk(KERN_INFO "boton_select: %d", boton_select);
    } else {
        return -EINVAL;
    }

    bytes_written = 1;

    return bytes_written;
}

static struct file_operations pugs_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write,
};

static int __init ofcd_init(void)
{
    

    printk(KERN_INFO "Initializing ofcd module\n");

    if (alloc_chrdev_region(&first, 0, 1, "button_driver") < 0) {
        return -1;
    }

    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL) {
        unregister_chrdev_region(first, 1);
        return -1;
    }

    if (device_create(cl, NULL, first, NULL, "button_device") == NULL) {
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }

    cdev_init(&c_dev, &pugs_fops);

    if (cdev_add(&c_dev, first, 1) == -1) {
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }

    // Inicializar los pines GPIO para los botones
    if (gpio_request(BUTTON1_PIN, "button1") < 0) {
        printk(KERN_ALERT "Error requesting GPIO pin for button 1\n");
        return -1;
    }

    if (gpio_request(BUTTON2_PIN, "button2") < 0) {
        printk(KERN_ALERT "Error requesting GPIO pin for button 2\n");
        return -1;
    }

    gpio_direction_input(BUTTON1_PIN);
    gpio_direction_input(BUTTON2_PIN);

    return 0;
}

static void __exit ofcd_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);

    gpio_free(BUTTON1_PIN);
    gpio_free(BUTTON2_PIN);

    printk(KERN_INFO "Exiting ofcd module\n");
}

module_init(ofcd_init);
module_exit(ofcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tu Nombre");
MODULE_DESCRIPTION("Controlador de dispositivo de botones");
