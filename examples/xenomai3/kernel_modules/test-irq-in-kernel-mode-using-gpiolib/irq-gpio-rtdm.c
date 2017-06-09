#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>

//#include <linux/list.h>
#include <rtdm/driver.h> // replaces #include <rtdm/rtdm_driver.h> in xenomai 2
//#include "rtdk.h" // obsolete!!

#define GPIO_IN  23
#define GPIO_OUT 22

static rtdm_irq_t irq_rtdm;

// globalCounter:
//   Global variable to count interrupts
//   Should be declared volatile to make sure the compiler doesn't cache it.
//
static volatile int globalCounter = 0 ;

static int handler_interruption(rtdm_irq_t * irq)
{
    static int value = 0;
	gpio_set_value(GPIO_OUT, value);  // realtime ???
	value = 1 - value;
    //  printk("interrupt\n"); -> not allowed in irq handler in linux, and certrainly not  realtime
	 ++globalCounter ;
    return RTDM_IRQ_HANDLED;
}


static int __init exemple_init (void)
{
	int err;

	int numero_interruption = gpio_to_irq(GPIO_IN);
	printk("numero_interruption %d\n",numero_interruption);  

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
       
    // initially turn output pin ON -> see module loaded!!
    gpio_set_value(GPIO_OUT, 1);

    printk("set irq trigger: rising\n");
        irq_set_irq_type(numero_interruption,  IRQ_TYPE_EDGE_FALLING);

    printk("irq request , IRQ_TYPE_EDGE_FALLING\n");
	if ((err = rtdm_irq_request(& irq_rtdm, 
	                 numero_interruption, handler_interruption, 
	                 IRQ_TYPE_EDGE_FALLING,
	                 THIS_MODULE->name, NULL)) != 0) {

        //printk("irq request , IRQTYPE_EDGE\n");
		printk("error %d\n",err);
		gpio_free(GPIO_OUT);
		gpio_free(GPIO_IN);
		return err;
	}
    printk("globalCounter %d\n",globalCounter);
    printk("done \n");

	return 0; 
}



static void __exit exemple_exit (void)
{
    printk("globalCounter %d\n",globalCounter);
	rtdm_irq_free(& irq_rtdm);
	gpio_free(GPIO_OUT);
	gpio_free(GPIO_IN);
}


module_init(exemple_init);
module_exit(exemple_exit);
MODULE_LICENSE("GPL");
