#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>

//#include <linux/list.h>
#include <rtdm/driver.h>
#include <rtdm/uapi/gpio.h>
//#include <rtdm/rtdm_driver.h>

// GPIO IN 23 -> Broche 16
#define GPIO_IN  17
// GPIO OUT 22 -> Broche 15
#define GPIO_OUT 22

static rtdm_irq_t irq_rtdm;


static int handler_interruption(rtdm_irq_t * irq)
{
	static int value = 0;
    printk("interrupt\n");
	gpio_set_value(GPIO_OUT, value);
	value = 1 - value;
	return RTDM_IRQ_HANDLED;
}


static int __init exemple_init (void)
{
	int err;

	int numero_interruption = gpio_to_irq(GPIO_IN);
	
    printk("gpio request in\n");
	if ((err = gpio_request(GPIO_IN, THIS_MODULE->name)) != 0) {
		printk("error %d\n",err);
		return err;
	}
    printk("gpio direction in\n");
	if ((err = gpio_direction_input(GPIO_IN)) != 0) {
		printk("error %d\n",err);
		gpio_free(GPIO_IN);
		return err;
	}
    printk("gpio request in\n");
	if ((err = gpio_request(GPIO_OUT, THIS_MODULE->name)) != 0) {
		printk("error %d\n",err);
		gpio_free(GPIO_IN);
		return err;
	}
    printk("gpio direction out\n");
	if ((err = gpio_direction_output(GPIO_OUT, 1)) != 0) {
		printk("error %d\n",err);
		gpio_free(GPIO_OUT);
		gpio_free(GPIO_IN);
		return err;
	}

    printk("set irq trigger: rising\n");
	irq_set_irq_type(numero_interruption,  IRQF_TRIGGER_RISING);

    printk("irq request , IRQTYPE_EDGE\n");
	if ((err = rtdm_irq_request(& irq_rtdm, 
	                 numero_interruption, handler_interruption, 
	                 RTDM_IRQTYPE_EDGE,
	                 THIS_MODULE->name, NULL)) != 0) {

        //printk("irq request , IRQTYPE_EDGE\n");
		printk("error %d\n",err);
		gpio_free(GPIO_OUT);
		gpio_free(GPIO_IN);
		return err;
	}
    printk("done \n");

	return 0; 
}



static void __exit exemple_exit (void)
{
	rtdm_irq_free(& irq_rtdm);
	gpio_free(GPIO_OUT);
	gpio_free(GPIO_IN);
}


module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");
