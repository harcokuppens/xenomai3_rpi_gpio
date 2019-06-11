
note:  for starting user space programs you have to set

          export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/xenomai/lib/
   
       so that the dynamic linker finds all xenomai dynamic link libraries
       


 
overview of programs 
====================

none-realtime linux
---------------------

./linux/wiringpi_examples/__README__.txt
    documentation of wiringpi 

./linux/wiringpi_examples/blink.c 
     blink program which shows blinking of a led  
     note: schematic described circuit/ folder
./linux/wiringpi_examples/isr.c
     program showing how to handle an interrupt using a switch 
     note: schematic described circuit/ folder
./linux/wiringpi_examples/button_toggles_led.c
     program showing how to handle an interrupt using a switch 
     to control whether a led is on or off  => basicly a light switch
./linux/wiringpi_examples/output_driven_irq.c
     program showing how to generate an interrupt using an output gpio pin 22 
     to control whether a led is on or off  => basicly a light switch
     and at the same time generate an interrupt on pin 24

realtime linux using xenomai cobalt kernel
-------------------------------------------

userspace programs

IMPORTANT:
  user space programs NEED loaded xenomai's gpio kernel module "xeno_gpio.ko"
  so that the realtime device files for GPIO are loaded :

        /dev/rtdm/pinctrl-bcm2835/gpioX            :  for gpio pin X 
        
  Using these realtime device files a realtime xenomai user program can control
  the gpio pins:
       setting  a high or low voltage to output pin by writing 1 or 0   from device file for that pin
       reading  a high or low voltage from input pin by reading 1 or 0  from device file for that pin
       blocking read on a input pin, which gets unblocked when an interrupt happened on a gpio input pin

./xenomai3/userspace_programs/blink/blink.c
    realtime blink program which shows blinking is done realtime because
    realtime dummy tasks with lower priority are interrupted!
./xenomai3/userspace_programs/isr/
     program showing how to handle an interrupt using a switch 
     note: schematic described circuit/ folder
./xenomai3/userspace_programs//button_toggles_led/
     program showing how to handle an interrupt using a switch 
     to control whether a led is on or off  => basicly a light switch
./xenomai3/userspace_programs/output_driven_irq/
     program showing how to generate an interrupt using an output gpio pin 22 
     to control whether a led is on or off  => basicly a light switch
     and at the same time generate an interrupt on pin 24

./xenomai3/userspace_programs/gpiotest/
   contains bash scripts to conveniently run several tests using xenomai's
   gpiotest program
  
./xenomai3/userspace_programs/gpiotest/mygpiotest/mygpiotest.c
    NEEDS loaded:  xeno_gpio.ko
    source of xenomai's  gpiotest programm


kernelspace programs

IMPORTANT:  kernel space programs don't need the "xeno_gpio.ko" mode loaded, because
            they can directly call the kernel space gpio API to handle interrupts,
            They don't need device files to indirect interact with the driver in kernel space!

./xenomai3/kernel_modules/test-irq-in-kernel-mode-using-gpiolib/button_toggles_led.c
    DOESN'T need loaded:  xeno_gpio.ko  
    
    NOTE: in kernel mode xenomai : 
       uses linux's gpiolib in kernel space to setup gpio pin's
       however use's xenomai's onw gpio-core.c library (xenomai source) to 
       do reading/writing gpio pins in realtime!

    the button_toggles_led.c example:
     program showing how to handle an interrupt using a switch 
     to control whether a led is on or off  => basicly a light switch
        
  








