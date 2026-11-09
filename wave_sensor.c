#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define DEVICE_NAME "wave_sensor"
#define CLASS_NAME "wave_class"

static int major_number;
static struct class* sensor_class = NULL;
static struct device* sensor_device = NULL;

static char sensor_data[] = "Vibration data: 42 Hz\n";

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	size_t data_size = sizeof(sensor_data) - 1;
	size_t to_copy;

	if (*offset >= data_size){
		return 0;
	}
	to_copy = min(len, (size_t)(data_size - *offset));

	if (copy_to_user(buffer, sensor_data + *offset, to_copy)){
		pr_err("wave_sensor: Failed to send %zu characters\n", to_copy);
		return -EFAULT;
	}
	
	*offset += to_copy;
	pr_info("wave_sensor: Sent %zu characters to the user\n", to_copy);

	return to_copy;
}

static struct file_operations fops = {
	.read = dev_read,
};

static int __init sensor_init(void){
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_number < 0){
		return major_number;
	}

	sensor_class = class_create(CLASS_NAME);
	if (IS_ERR(sensor_class)){
		unregister_chrdev(major_number, DEVICE_NAME);
		return PTR_ERR(sensor_class);
	}

	sensor_device = device_create(sensor_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
	if (IS_ERR(sensor_device)){
		class_destroy(sensor_class);
		unregister_chrdev(major_number, DEVICE_NAME);
		return PTR_ERR(sensor_device);
	}

	printk(KERN_INFO "wave_sensor: Device created correctly\n");
	return 0;
}

static void __exit sensor_exit(void){
	device_destroy(sensor_class, MKDEV(major_number, 0));
	class_destroy(sensor_class);
	unregister_chrdev(major_number, DEVICE_NAME);
	printk(KERN_INFO "wave_sensor: Module removed\n");
}

module_init(sensor_init);
module_exit(sensor_exit);

MODULE_LICENSE("GPL");
