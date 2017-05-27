
note:  for starting user space programs you have to set

          export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/xenomai/lib/
   
       so that the dynamic linker finds all xenomai dynamic link libraries
       
       
       


in order of history of projects 
---------------------------------


./xenomai3/userspace_programs/blink/blink.c
    NEEDS loaded:  xeno_gpio.ko
    realtime blink program which shows blinking is done realtime because
    realtime dummy tasks with lower priority are interrupted!

    -> works, but some strange error code on write, however light is blinking!!
       strange error:  

         Failed to write  value 0 to pin 17  ret -1 errno 22

       background info: 


        * http://www.virtsync.com/c-error-codes-include-errno

             #define EINVAL      22  /* Invalid argument */
        
        * man 2 write:

              RETURN VALUE
                     On success, the number of bytes written is returned (zero indicates nothing was written).
                     On error, -1 is returned, and errno is set appropriately.

              EINVAL fd  is
                       attached to an object which is unsuitable for writing;
                       or the file was opened with the O_DIRECT flag,
                       and either
                          the address specified in buf,
                           the value specified in count,
                           or the current file offset is not suitably aligned.


  
./xenomai3/userspace_programs/mygpiotest/mygpiotest.c
    NEEDS loaded:  xeno_gpio.ko
    modified version of xenomai's  gpiotest programm


./xenomai3/kernel_modules/test-irq-in-kernel-mode-using-gpiolib /irq-gpio-rtdm.c
    DOESN'T need loaded:  xeno_gpio.ko  
    => builds interrupt programming without using xenomai's gpio-core.c lib
       but does itself by using linux's gpiolib

    ====> rpi hangs!!    => problem rpi2-xenomai3 image   (also for pi3)





Because of problems with interrupts,(see below) , I decided to try out a none
realtime none-xenomai irq program which does work on normal raspbian image
(meaning not patched with xenomai). I verified that this program does correctly
handles interrupts via the gpio pin. However if I run this same program
on our  xenomai-patched  rpi2-xenomai3 image  the program hangs when you
trigger an interrupt via the gpio.



./wiringpi_examples/__README__.txt
./wiringpi_examples/blink.c  => OK
./wiringpi_examples/isr.c
    ====> rpi hangs!!    => problem rpi2-xenomai3 image   (also for pi3)


