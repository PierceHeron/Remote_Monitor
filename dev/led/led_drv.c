#include <linux/kernel.h>
#include <linux/module.h> 
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/gpio.h>

#include "cfg_type.h"
/*
D7----GPIOE13 ----- PAD_GPIO_E + 13
D8----GPIOC17  ----- PAD_GPIO_C + 17
D9----GPIOC8  ----- PAD_GPIO_C + 8
D10----GPIOC7  ----- PAD_GPIO_C + 7
*/

struct gpio_info{
	unsigned int gpio_num;
	char gpio_name[20];	
};

static struct gpio_info gec6818_led_info[4] = {
	{
		.gpio_num = PAD_GPIO_E + 13,
		.gpio_name = "gpioe13_led7",
	},
	{
		.gpio_num = PAD_GPIO_C + 17,
		.gpio_name = "gpiocc17_led8",
	},	
	{
		.gpio_num = PAD_GPIO_C + 8,
		.gpio_name = "gpioc8_led9",
	},	
	{
		.gpio_num = PAD_GPIO_C + 7,
		.gpio_name = "gpioc7_led10"
	},	
};


//1.定义一个cdev
static struct cdev gec6818_led_cdev;

static unsigned int led_major = 0; //100--静态注册
static unsigned int led_minor = 0;
static dev_t led_dev_num;

static struct class *led_class = NULL;
static struct device *led_device = NULL;
static char led_status[4]={0};

//3.定义并初始化文件操作集
static int gec6818_led_open(struct inode *inode, struct file *filp)
{
	//printk("led driver is openning\n");
	return 0;
}
//应用程序写下来的数据格式：char buf[2]
//buf[1]---哪一盏灯；buf[0]---灯的状态:1--on,0--off
static ssize_t gec6818_led_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	char kbuf[2];
	int ret;
	ret = copy_from_user(kbuf, buf, len);
	if(ret != 0){
		printk("copy from user error\n");
		return -EFAULT;	
	}

	if(kbuf[0]) {
		gpio_set_value(gec6818_led_info[(int)kbuf[1]].gpio_num, 0);
		led_status[(int)kbuf[1]] = 1;
	} else {
		gpio_set_value(gec6818_led_info[(int)kbuf[1]].gpio_num, 1);
		led_status[(int)kbuf[1]] = 0;
	}

	return len;
}

static ssize_t gec6818_led_read(struct file *filp, char __user *buf, size_t len, loff_t * off){
	int ret = 0;
	
	ret = copy_to_user(buf, led_status, 4);
	if(ret != 0)
		return -EFAULT;

	return len;
}

static int gec6818_led_release(struct inode *inode, struct file *filp)
{
	//printk("led driver is closing\n");
	return 0;
}

static const struct file_operations gec6818_led_fops = {
	.open = gec6818_led_open,
	.read = gec6818_led_read,
	.write = gec6818_led_write,
	.release = gec6818_led_release,
};

 
//module的入口函数---驱动的初始化安装函数
static int __init gec6818_led_init(void)
{
	int ret,i;
	//2.申请或注册设备号
	if(led_major == 0){
		ret = alloc_chrdev_region(&led_dev_num, led_minor, 1, "led_device");
	}else{
		led_dev_num = MKDEV(led_major,led_minor);
		ret = register_chrdev_region(led_dev_num, 1, "led_device");
	}
	if(ret < 0){
		printk("can not get device number\n");
		return ret;
	}
	
	//4.初始化cdev
	cdev_init(&gec6818_led_cdev, &gec6818_led_fops);
	
	//5、将cdev加入内核
	ret = cdev_add(&gec6818_led_cdev, led_dev_num, 1);
	if(ret < 0){
		printk("add cdev error\n");
		goto cdev_add_error;
	}
	//6.创建class
	led_class = class_create(THIS_MODULE, "gec6818_led_class");
	if(led_class == NULL){
		printk("class create error\n");
		ret = -EBUSY;
		goto class_create_error;
	}
	
	//7.创建device
	led_device = device_create(led_class, NULL,
			     led_dev_num, NULL, "led_drv");
	if(led_device == NULL){
		printk("device create error\n");
		ret = -EBUSY;
		goto device_create_error;
	}
	
	//8.申请GPIO口
	for(i=0;i<4;i++)
		gpio_free(gec6818_led_info[i].gpio_num);
	
	for(i=0;i<4;i++){
		ret = gpio_request(gec6818_led_info[i].gpio_num, 
					gec6818_led_info[i].gpio_name);
		if(ret < 0){
			printk("can not request gpio:%s\n", gec6818_led_info[i].gpio_name);
			goto gpio_request_error;			
		}

		gpio_direction_output(gec6818_led_info[i].gpio_num, 1);
	}
	
	printk("gec6818 led driver init\n");
	
	return 0;

gpio_request_error:
	while(i--)//--i
		gpio_free(gec6818_led_info[i].gpio_num);
	device_destroy(led_class, led_dev_num);	
	
device_create_error:
	class_destroy(led_class);
class_create_error:
	cdev_del(&gec6818_led_cdev);
cdev_add_error:
	unregister_chrdev_region(led_dev_num, 1);
	return ret;
}

//module的出口函数---驱动程序的卸载函数
static void __exit gec6818_led_exit(void)
{
	int i;
	unregister_chrdev_region(led_dev_num, 1);
	cdev_del(&gec6818_led_cdev);
	device_destroy(led_class, led_dev_num);
	class_destroy(led_class);
	for(i=0;i<4;i++)
		gpio_free(gec6818_led_info[i].gpio_num);
	
	printk("gec6818 led driver exit\n");
}
 
//module的入口和出口
module_init(gec6818_led_init);
module_exit(gec6818_led_exit); 
 
//module的描述
MODULE_AUTHOR("1003261954@qq.com");
MODULE_DESCRIPTION("GEC6818 LED Device Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("V1.0");
