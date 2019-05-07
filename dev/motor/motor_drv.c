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
#include <linux/delay.h>

#include "cfg_type.h"
/*
UARTTXD1----GPIOD19 ----- PAD_GPIO_D + 19
UARTTXD2----GPIOD20 ----- PAD_GPIO_D + 20
*/

struct gpio_info{
	unsigned int gpio_num;
	char gpio_name[20];	
};

static struct gpio_info gec6818_motor_info[2] = {
	{
		.gpio_num = PAD_GPIO_D + 19,
		.gpio_name = "motor1_sig",
	},
	{
		.gpio_num = PAD_GPIO_D + 20,
		.gpio_name = "motor2_sig"
	},	
};


//1.定义一个cdev
static struct cdev gec6818_motor_cdev;

static unsigned int motor_major = 0; //100--静态注册
static unsigned int motor_minor = 0;
static dev_t motor_dev_num;

static struct class *motor_class = NULL;
static struct device *motor_device = NULL;

static char motor_status[2] = {0};

//3.定义并初始化文件操作集
static int gec6818_motor_open(struct inode *inode, struct file *filp)
{
	//printk("motor driver is openning\n");
	return 0;
}
//应用程序写下来的数据格式：char buf[2]
static ssize_t gec6818_motor_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	char kbuf[2];
	int ret;
	unsigned int duty = 0;

	ret = copy_from_user(kbuf, buf, len);
	if(ret != 0){
		printk("copy from user error\n");
		return -EFAULT;	
	}

	if(kbuf[1] == 1) {
		if(motor_status[(unsigned int)kbuf[0]] != 180)
			duty = (motor_status[(unsigned int)kbuf[0]] + 10) * 11.11;
			motor_status[(unsigned int)kbuf[0]] += 10;
	} else if (kbuf[1] == 2) {
		if(motor_status[(unsigned int)kbuf[0]] != 0)
			duty = (motor_status[(unsigned int)kbuf[0]] - 10) * 11.11;
			motor_status[(unsigned int)kbuf[0]] -= 10;
	} else {
		duty = 1000;
	}	
	gpio_direction_output(gec6818_motor_info[(unsigned int)kbuf[0]].gpio_num, 0);
	if(duty > 1000) {
		mdelay(1);
		udelay(duty - 500);
	} else {
		udelay(500 + duty);
	}
	gpio_direction_output(gec6818_motor_info[(unsigned int)kbuf[0]].gpio_num, 1);

	return len;
}

static int gec6818_motor_release(struct inode *inode, struct file *filp)
{
	//printk("motor driver is closing\n");
	return 0;
}

static const struct file_operations gec6818_motor_fops = {
	.open = gec6818_motor_open,
	.write = gec6818_motor_write,
	.release = gec6818_motor_release,
};

 
//module的入口函数---驱动的初始化安装函数
static int __init gec6818_motor_init(void)
{
	int ret,i;
	//2.申请或注册设备号
	if(motor_major == 0){
		ret = alloc_chrdev_region(&motor_dev_num, motor_minor, 1, "motor_device");
	}else{
		motor_dev_num = MKDEV(motor_major,motor_minor);
		ret = register_chrdev_region(motor_dev_num, 1, "motor_device");
	}
	if(ret < 0){
		printk("can not get device number\n");
		return ret;
	}
	
	//4.初始化cdev
	cdev_init(&gec6818_motor_cdev, &gec6818_motor_fops);
	
	//5、将cdev加入内核
	ret = cdev_add(&gec6818_motor_cdev, motor_dev_num, 1);
	if(ret < 0){
		printk("add cdev error\n");
		goto cdev_add_error;
	}
	//6.创建class
	motor_class = class_create(THIS_MODULE, "gec6818_motor_class");
	if(motor_class == NULL){
		printk("class create error\n");
		ret = -EBUSY;
		goto class_create_error;
	}
	
	//7.创建device
	motor_device = device_create(motor_class, NULL,
			     motor_dev_num, NULL, "motor_drv");
	if(motor_device == NULL){
		printk("device create error\n");
		ret = -EBUSY;
		goto device_create_error;
	}
	
	//8.申请GPIO口
	for(i=0;i<2;i++)
		gpio_free(gec6818_motor_info[i].gpio_num);
	
	for(i=0;i<2;i++){
		ret = gpio_request(gec6818_motor_info[i].gpio_num, 
					gec6818_motor_info[i].gpio_name);
		if(ret < 0){
			printk("can not request gpio:%s\n", gec6818_motor_info[i].gpio_name);
			goto gpio_request_error;			
		}	
		gpio_direction_output(gec6818_motor_info[i].gpio_num, 0);
	}
	mdelay(2000);

	/*gpio_direction_output(gec6818_motor_info[0].gpio_num, 1);
	gpio_direction_output(gec6818_motor_info[1].gpio_num, 1);

	mdelay(2000);
	
	gpio_direction_output(gec6818_motor_info[0].gpio_num, 0);
	gpio_direction_output(gec6818_motor_info[1].gpio_num, 0);
	
	udelay(1000);
	mdelay(18);
	
	motor_status[0] = 90;
	motor_status[1] = 90;*/

	printk("gec6818 motor driver init\n");
	
	return 0;

gpio_request_error:
	while(i--)//--i
		gpio_free(gec6818_motor_info[i].gpio_num);
	device_destroy(motor_class, motor_dev_num);	
	
device_create_error:
	class_destroy(motor_class);
class_create_error:
	cdev_del(&gec6818_motor_cdev);
cdev_add_error:
	unregister_chrdev_region(motor_dev_num, 1);
	return ret;
}

//module的出口函数---驱动程序的卸载函数
static void __exit gec6818_motor_exit(void)
{
	int i;
	unregister_chrdev_region(motor_dev_num, 1);
	cdev_del(&gec6818_motor_cdev);
	device_destroy(motor_class, motor_dev_num);
	class_destroy(motor_class);
	for(i=0;i<2;i++)
		gpio_free(gec6818_motor_info[i].gpio_num);
	
	printk("gec6818 motor driver exit\n");
}
 
//module的入口和出口
module_init(gec6818_motor_init);
module_exit(gec6818_motor_exit); 
 
//module的描述
MODULE_AUTHOR("1003261954@qq.com");
MODULE_DESCRIPTION("GEC6818 MOTOR Device Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("V1.0");
