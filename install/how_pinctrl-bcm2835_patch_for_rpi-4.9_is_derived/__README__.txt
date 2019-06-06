how to use pinctrl-bcm2835.c.rpi-4.9.patch  on raspbian 4.9 original code 
=========================================================================

   fetch the original raspbian 4.9 version of  pinctrl-bcm2835.c :
         https://github.com/raspberrypi/linux/blob/rpi-4.9.y/drivers/pinctrl/bcm/pinctrl-bcm2835.c
           `-> https://raw.githubusercontent.com/raspberrypi/linux/rpi-4.9.y/drivers/pinctrl/bcm/pinctrl-bcm2835.c
           
          mkdir rpi-4.9.y/ 
          cd rpi-4.9.y/ 
          wget https://raw.githubusercontent.com/raspberrypi/linux/rpi-4.9.y/drivers/pinctrl/bcm/pinctrl-bcm2835.c
          cd ..
         
    then  
       $ cp -r rpi-4.9.y/ rpi-4.9.y.patched
       $ cd rpi-4.9.y.patched/
       $ patch   < ../pinctrl-bcm2835.c.rpi-4.9.patch
       patching file pinctrl-bcm2835.c
       Hunk #3 succeeded at 412 (offset -3 lines).
       Hunk #4 succeeded at 440 (offset -3 lines).
       Hunk #5 succeeded at 496 (offset -3 lines).
       Hunk #6 succeeded at 511 (offset -3 lines).
       Hunk #7 succeeded at 619 (offset -3 lines).
       Hunk #8 succeeded at 631 with fuzz 2 (offset -3 lines).
       Hunk #9 succeeded at 644 (offset -3 lines).
       Hunk #10 succeeded at 685 (offset -3 lines).
       Hunk #11 succeeded at 1097 (offset 7 lines).
       Hunk #12 succeeded at 1115 (offset 7 lines).
       Hunk #13 succeeded at 1127 with fuzz 2 (offset 7 lines).
       
       => patch successfull


how we created the  pinctrl-bcm2835.c.rpi-4.9.patch  for the raspbian 4.9 original code 
=======================================================================================


We have a good ipipe patched version for  pinctrl-bcm2835.c for the raspberry pi kernel version
from a special branch of xenomai:
 
   https://gitlab.denx.de/Xenomai/ipipe/raw/vendors/raspberry/ipipe-4.1/drivers/pinctrl/bcm/pinctrl-bcm2835.c 

however this file is for raspbian 4.1 and not for raspbian 4.9!!

So we want the derive the ipipe patched version of pinctrl-bcm2835.c for rpi-4.9.y.
This file is not available in some repository.

Maybe the ipipe patch for raspbian 4.1 will this also work for raspbian 4.9 ?

First collect all sources:

    raspbian 4.1 
    ------------

     - original
        https://github.com/raspberrypi/linux/blob/rpi-4.1.y/drivers/pinctrl/bcm/pinctrl-bcm2835.c
       ``-> https://raw.githubusercontent.com/raspberrypi/linux/rpi-4.1.y/drivers/pinctrl/bcm/pinctrl-bcm2835.c

           mkdir rpi-4.1.y/
           cd rpi-4.1.y/ 
           wget https://raw.githubusercontent.com/raspberrypi/linux/rpi-4.1.y/drivers/pinctrl/bcm/pinctrl-bcm2835.c
           cd ..
 
     - ipipe patched 
        in a special branch of the xenomai git repository I found a patched version
        specially for the rpi-4.1.y kernel:

        https://gitlab.denx.de/Xenomai/ipipe/blob/vendors/raspberry/ipipe-4.1/drivers/pinctrl/bcm/pinctrl-bcm2835.c
        `-> https://gitlab.denx.de/Xenomai/ipipe/raw/vendors/raspberry/ipipe-4.1/drivers/pinctrl/bcm/pinctrl-bcm2835.c
        
           mkdir rpi-4.1.y.ipipe-patched/ 
           cd rpi-4.1.y.ipipe-patched/
           wget https://gitlab.denx.de/Xenomai/ipipe/raw/vendors/raspberry/ipipe-4.1/drivers/pinctrl/bcm/pinctrl-bcm2835.c
           cd ..    
    
    raspbian 4.9
    ------------
  
     - original
        https://github.com/raspberrypi/linux/blob/rpi-4.9.y/drivers/pinctrl/bcm/pinctrl-bcm2835.c
       ``-> https://raw.githubusercontent.com/raspberrypi/linux/rpi-4.9.y/drivers/pinctrl/bcm/pinctrl-bcm2835.c

           mkdir rpi-4.9.y/
           cd rpi-4.9.y/ 
           wget https://raw.githubusercontent.com/raspberrypi/linux/rpi-4.9.y/drivers/pinctrl/bcm/pinctrl-bcm2835.c
           cd ..


So we want the derive the ipipe patched version of pinctrl-bcm2835.c for rpi-4.9.y.


     => first I made a patch for rpi-4.1.y by doing a unified diff of rpi's original version and our patched version:
      
           diff -u rpi-4.1.y/pinctrl-bcm2835.c xenomai_branch_vendors_raspberry_ipipe-4.1/pinctrl-bcm2835.c  > pinctrl-bcm2835.c.rpi-4.1.patch

        and then applied this diff on the rpi-4.9.y/pinctrl-bcm2835.c
        however the final result made the kernel crash on receiving a gpio interrupt

        I saved this failed patch result in : 

            rpi-4.9.y.ipipe-patched-with-pinctrl-bcm2835.c.rpi-4.1.patch/pinctrl-bcm2835.c
        
     => So something was not right with this patch. 
        
        To found out what I first fetched the ipipe-core-4.9.51-arm-4.patch for the kernel.org
        kernel and extracted the patch specificly for the pinctrl-bcm2835.c
        file in the file ipipe-core-4.9.51-arm-4.only-for-pinctrl-bcm2835.patch
        
        I downloaded the kernel.org.4.9.5 version of  pinctrl-bcm2835.c and
        applied the ipipe patch to it. So I got the files:

         kernel.org.4.9.51/pinctrl-bcm2835.c
         kernel.org.4.9.51.ipipe-patched/pinctrl-bcm2835.c  

      So I inspected these files: 

        rpi-4.1.y/pinctrl-bcm2835.c
        rpi-4.1.y.ipipe-patched/pinctrl-bcm2835.c
        rpi-4.9.y/pinctrl-bcm2835.c
        rpi-4.9.y.ipipe-patched/pinctrl-bcm2835.c
        kernel.org.4.9.51/pinctrl-bcm2835.c
        kernel.org.4.9.51.ipipe-patched/pinctrl-bcm2835.c
        rpi-4.9.y.ipipe-patched-with-pinctrl-bcm2835.c.rpi-4.1.patch/pinctrl-bcm2835.c

     and finally discovered the following:
     
        - the kernel.org 4.9  source version is almost the same as the rpi-4.9.y, 
          only some details of interupts handling where different.
          Raspbian just has the details better.
          
          note: the latest kernel.org has version which is even more simular as the raspbian version 
                see https://github.com/torvalds/linux/blob/v5.1/drivers/pinctrl/bcm/pinctrl-bcm2835.c
                      also has three constants BCM2835_NUM_GPIOS  BCM2835_NUM_BANKS BCM2835_NUM_IRQS (4.9 kernel.org has only 2 of them)
                
                So the kernel.org kernel catching up with the rpi kernel.
                
        - when comparing 

             kernel.org.4.9.51.ipipe-patched/pinctrl-bcm2835.c rpi-4.9.y.ipipe-patched-with-pinctrl-bcm2835.c.rpi-4.1.patch/pinctrl-bcm2835.c

          I found that the interface for the gpio_irq_cascade method was changed from 2 arguments to 1 argument
          
              fix for patch :
               - old interface  : 
                    static void gpio_irq_cascade(unsigned int irq, struct irq_desc *desc)
                    {
                    #ifdef CONFIG_IPIPE
                    	bcm2835_gpio_irq_handler(irq, irq_get_handler_data(irq));
                    #endif
                    }

              - new  interface  : (fix for patch)
                    static void gpio_irq_cascade(struct irq_desc *desc)
                    {
                    #ifdef CONFIG_IPIPE
                    	unsigned int irq = irq_desc_get_irq(desc);
                    	bcm2835_gpio_irq_handler(irq, irq_get_handler_data(irq));
                    #endif
                    }     
                         
        - we applied this fix to the file rpi-4.9.y.ipipe-patched-with-pinctrl-bcm2835.c.rpi-4.1.patch/pinctrl-bcm2835.c 
          and store the result in the file rpi-4.9.y.ipipe-patched/pinctrl-bcm2835.c 

            => after rebuilding the kernel and running it, we found the gpio pins work in xenomai!!

        - then we derived a new patch by doing a unified diff of rpi's original version and our patched version:
        
            diff -u rpi-4.9.y/pinctrl-bcm2835.c rpi-4.9.y.ipipe-patched/pinctrl-bcm2835.c  > pinctrl-bcm2835.c.rpi-4.9.patch



