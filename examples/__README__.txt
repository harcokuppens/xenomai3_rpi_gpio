
note:  for starting user space programs you have to set

          export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/xenomai/lib/
   
       so that the dynamic linker finds all xenomai dynamic link libraries
       


 
overview of programs 
====================

none-realtime linux
---------------------

./wiringpi_examples/__README__.txt
    documentation of wiringpi 

./wiringpi_examples/blink.c 
     blink program which shows blinking of a led  
     note: schematic described circuit/ folder
./wiringpi_examples/isr.c
     program showing how to handle an interrupt using a switch 
     note: schematic described circuit/ folder
./wiringpi_examples/button_toggles_led.c
     program showing how to handle an interrupt using a switch 
     to control whether a led is on or off  => basicly a light switch

none-realtime linux
---------------------


./xenomai3/userspace_programs/blink/blink.c
    NEEDS loaded:  xeno_gpio.ko
    realtime blink program which shows blinking is done realtime because
    realtime dummy tasks with lower priority are interrupted!
./xenomai3/userspace_programs/isr/
     program showing how to handle an interrupt using a switch 
     note: schematic described circuit/ folder
./xenomai3/userspace_programs//button_toggles_led/
     program showing how to handle an interrupt using a switch 
     to control whether a led is on or off  => basicly a light switch


./xenomai3/userspace_programs/gpiotest/
   contains bash scripts to conveniently run several tests using xenomai's
   gpiotest program
  
./xenomai3/userspace_programs/gpiotest/mygpiotest/mygpiotest.c
    NEEDS loaded:  xeno_gpio.ko
    source of xenomai's  gpiotest programm



./xenomai3/kernel_modules/test-irq-in-kernel-mode-using-gpiolib /irq-gpio-rtdm.c
    DOESN'T need loaded:  xeno_gpio.ko  
    => builds interrupt programming without using xenomai's gpio-core.c lib
       but does itself by using linux's gpiolib










