#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/module.h>

//#include <linux/list.h>
#include <rtdm/driver.h> // replaces #include <rtdm/rtdm_driver.h> in xenomai 2
//#include "rtdk.h" // obsolete!!

#include <cobalt/kernel/ancillaries.h>
//#include <boilerplate/ancillaries.h>

#define GPIO_IN  23
#define GPIO_OUT 22

static rtdm_irq_t irq_rtdm;

// globalCounter:
//   Global variable to count interrupts
//   Should be declared volatile to make sure the compiler doesn't cache it.
//
static volatile int globalCounter = 0 ;
static volatile int handlerCPU = -1;

static int handler_interruption(rtdm_irq_t * irq)
{
    static int value = 0;
	gpio_set_value(GPIO_OUT, value);  // realtime ???
	value = 1 - value;
    //  printk("interrupt\n"); -> not allowed in irq handler in linux, and certrainly not  realtime
	 ++globalCounter ;
     
    handlerCPU =smp_processor_id();
    return RTDM_IRQ_HANDLED;
}


static int __init example_init (void)
{
	int err,cpu,irq_number;
        //cpumask_t cpumask;

    cpu=5;
    
	irq_number = gpio_to_irq(GPIO_IN);
	printk("irq number: %d\n",irq_number);  
    
    cpu =  smp_processor_id();// get_current_cpu();   
    printk("cpu: %d\n",cpu);
    
    

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
        irq_set_irq_type(irq_number,  IRQ_TYPE_EDGE_FALLING);

 

    printk("irq request , IRQ_TYPE_EDGE_FALLING\n");
	if ((err = rtdm_irq_request(& irq_rtdm, 
	                 irq_number, handler_interruption, 
	                 IRQ_TYPE_EDGE_FALLING,
	                 THIS_MODULE->name, NULL)) != 0) {

        //printk("irq request , IRQTYPE_EDGE\n");
		printk("error %d\n",err);
		gpio_free(GPIO_OUT);
		gpio_free(GPIO_IN);
		return err;
	}
    
    /*  try to change irq affinity to cpu3 (or any other)  => failed, stays assigned to cpu0
    cpumask_clear(&cpumask);
    cpumask_set_cpu(3,&cpumask);
    xnintr_affinity(& irq_rtdm,cpumask);
    
    // note:    
    //  This fails probably because for all the pi's (pi1,pi2,pi3) there is no vectored interrupt controller implemented.
    //  For the pi2 and p3 there is  a simple register-based interface to set the global top-level interrupt to a specific core.    
    // Probably the  xnintr_affinity method works only for (vectored) interrupt controllers  and not for
    //  a specialized register-based interface on the arm processor.    
    
    */   
    
    printk("globalCounter %d\n",globalCounter);
    printk("handlerCPU: %d\n",handlerCPU);
    printk("done \n");

	return 0; 
}




static void __exit example_exit (void)
{
    printk("globalCounter %d\n",globalCounter);
    printk("handlerCPU: %d\n",handlerCPU);
	rtdm_irq_free(& irq_rtdm);
	gpio_free(GPIO_OUT);
	gpio_free(GPIO_IN);
}


module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL");
