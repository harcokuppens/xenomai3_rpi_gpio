

Xenomai GPIO support
====================

practical
---------

user space 
  user space programs NEED loaded xenomai's gpio kernel module "xeno_gpio.ko"
  so that the realtime device files for GPIO are loaded :

        /dev/rtdm/pinctrl-bcm2835/gpioX            :  for gpio pin X

  Using these realtime device files a realtime xenomai user program can control
  the gpio pins:
       setting  a high or low voltage to output pin by writing 1 or 0   from device file for that pin
       reading  a high or low voltage from input pin by reading 1 or 0  from device file for that pin
       blocking read on a input pin, which gets unblocked when an interrupt happened on a gpio input pin


kernel space
  kernel space programs don't need the "xeno_gpio.ko" mode loaded, because
  they can directly call the kernel space gpio API to handle interrupts,
  They don't need device files to indirect interact with the driver in kernel space!
  
   => update: for xenomai 3.08 the module is renamed to "xeno_gpio_bcm2835.ko"
              
              load it with :
              
                   modprobe xeno_gpio_bcm2835
                   
              check if it is really loaded with "lsmod" command
              
    note: only when this module is loaded then the /dev/rtdm/pinctrl-bcm2835/ folder with 
          the /dev/rtdm/pinctrl-bcm2835/gpioX devices appear                 

How to use program with gpio's in userspace or kernels space: 
    
      see rtdm_api.txt 

and how gpio pins map to interrupts see:

   ./notes/gpio/gpio_to_irq.txt

xenomai reuses linux's gpiolib and gpiochip
-------------------------------------------

 Realtime rtdm drivers reuse the gpiolib of linux to setup the gpiochip
 correctly none-realtime, and then  do for the realtime driver special realtime code
 to do realtime communciation with the gpiochip, mainly realtime write or read.
 
 note: it can also reuse wiringpi lib to setup the gpio pin at none-realtime!!
 
 
 xenomai's realtime gpio library wraps linux gpiochip struct of the device to implement a RTDM driver.
 However often you must adapt the driver code of the gpiochip struct if it calls linux services
 which can not be used from real-time domain. You either need to replace these calls by realtime
 versions (maybe copying patching the linux service original code), or sometimes
 you can simply replace a spinlock by an ipipe-spinlock.
 
 Below it describes this in more details :
 
 

general online background information for reusing linux's gpio chip  implementation for xenomai
------------------------------------------------------------------------------------------------

http://xenomai.org/2014/09/porting-xenomai-dual-kernel-to-a-new-arm-soc/#GPIOs

xenomai in general about gpio ports :
  
    Most SoCs have GPIOs. In the context of Xenomai, they are interesting for two reasons:

      -  they may be used by real-time drivers as input our output for communicating with peripherals 
         externals to the SoC;
      -  they may be used as interrupt sources.
    

    GPIOs in real-time drivers

        As for hardware timers and interrupt controllers, the specificities of a GPIO controller are embedded in a
        structure, this one name “struct gpio_chip”. You usually find the definition for the SoC you use in one of the
        files: drivers/gpio-X.c, arch/arm/mach-Y/gpio.c, arch/arm/plat-Z/gpio.c.  ( in kernel source )

        This handlers are then accessible using the “gpiolib” infrastructure.

        For instance, the “struct gpio_chip” contains a “get” members which get called when using the function
        “gpio_get_value”.

        You should first check that the implementation of the function members of the “struct gpio_chip” structure do
        not use Linux services which can not be used from real-time domain. If this is the case:

          * if the implementation of these handlers need to communicate with an I2C or SPI chip, the code as it is
            needs significant changes to be made available to real-time drivers, starting with rewriting the driver for
            the I2C or SPI controller as a driver running in real-time domain;
            
          * if the implementation of these handlers simply uses a spinlock, the spinlock may be turned into an I-pipe
            spinlock (pay attention, however, that there is not other Linux service called, or actions which may take
            an unbounded time when holding the spinlock).

    GPIOs as interrupt sources

        Most SoCs have so many GPIOs, that each one can not have a separate line at the interrupt controller level, so
        they are multiplexed. What happens then is that there is a single line for a whole GPIO bank, the interrupt
        handler for this irq line should read a GPIO controller register to find out which of the GPIOs interrupts are
        pending, then invoke the handler for each of them. 
        
        The mechanism used by the Linux kernel to handle this
        situation is called “chained interrupts”, you can find whether the SoC you use in this case if it calls the
        function “irq_set_chained_handler”. It is usually found in drivers/gpio/gpio-X.c, arch/arm/mach-Y/gpio.c,
        arch/arm/plat-Z/gpio.c, arch/arm/mach-X/irq.c, or arch/arm/plat-Y/irq.c.

        What will happen with the I-pipe core, is that the handler registered with “irq_set_chained_handler” will be
        called in real-time context, so should not use any Linux service which can not be used from real-time context,
        in particular, calls to “generic_handle_irq”, should be replaced with calls to “ipipe_handle_demuxed_irq”.

        When GPIOs are used as interrupt sources, a “struct irq_chip” is defined, allowing the kernel to see the GPIOs
        controller as an interrupt controller, so, most of what is said in the “Interrupt controller” section     -> see  below
        also applies to the GPIO controller. Most of the time, though, the “flow handler” for these interrupts is
        “handle_simple_irq”, and nothing needs to be done.




  Interrupt controller

      The I-pipe core needs to interact with the SoC interrupt controller, it uses a deferred interrupt model, which means
      that when an interrupt happens, it is first acknowledged and masked at the interrupt controller level, it will be
      handled then unmasked at a later time, which is slightly different from the way Linux handles interrupt, so require
      deep modifications.


      Fortunately, as for timer management, interrupt controllers specificities are embedded in the “struct irq_chip”
      structure, and interactions with them are implemented in a generic way, so almost no modifications need to be done in
      the SoC specific code. Though, there are a few things to which you should pay attention.   => see further http://xenomai.org/2014/09/porting-xenomai-dual-kernel-to-a-new-arm-soc/#Interrupt_controller
  
     
       

