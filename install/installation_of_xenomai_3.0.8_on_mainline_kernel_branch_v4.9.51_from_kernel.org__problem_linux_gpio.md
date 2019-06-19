
Installation of xenomai 3.0.8 on mainline kernel branch v4.9.51 from kernel.org  
============================================

First get the ingredients:
 
    # get xenomai
    wget https://xenomai.org/downloads/xenomai/stable/xenomai-3.0.8.tar.bz2
    tar -xjvf xenomai-3.0.8.tar.bz2
    
    # get kernel branch v4.9.51  from kernel.org
    git clone -b v4.9.51 --depth 1 https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git linux

    # get ipipe patch for kernel
    wget https://www.xenomai.org/downloads/ipipe/v4.x/arm/ipipe-core-4.9.51-arm-4.patch
    
Patch the kernel with the ipipe patch
    
    xenomai-3.0.8/scripts/prepare-kernel.sh  --linux=linux/  --arch=arm  --ipipe=ipipe-core-4.9.51-arm-4.patch
 

 
 Configure the kernel
 
    cd linux
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- multi_v7_defconfig
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
 
 Within menuconfig choose the following options:
 
     CPU Power Management  --->
       CPU Frequency scaling  --->
         [ ] CPU Frequency scaling
       CPU idle  --->
         [ ] CPU idle PM support
     Kernel Features  --->
         [ ] Contiguous Memory Allocator
         [ ] Allow for memory compaction
     Kernel Hacking  --->
         [ ] KGDB: kernel debugger
     Boot options  --->
         Kernel command line type --->
             [X] Extend bootloader kernel arguments

     Xenomai Cobalt --->
          Drivers  --->
                  Real-time GPIO drivers  --->
                     <*> GPIO controller                          => with " [M] Support for BCM2835 GPIOs   " instead it builds directly in kernel, then no need to load module anymore!
                     [M]   Support for BCM2835 GPIOs                 `-> however we stick to default and just build it as module and load it at boot in /etc/rc.local
                     [ ]   Enable GPIO core debugging features


     note: in above  [ ]  means disable,  [X]  means enable


make distribution dir where we store our build packages
    
    cd ..
    mkdir dist/
 
build kernel debian package
 
    cd linux 
    export NUMCORES=4  
    time make -j $NUMCORES ARCH=arm  CROSS_COMPILE=arm-linux-gnueabihf- KBUILD_DEBARCH=armhf deb-pkg
    # does first : make clean
    # also does : make zImage modules dtbs   to finally build debian package

    # store debian package in distribution directory
    cd ../
    mv linux-image-4.9.51-*.deb dist/

package all build device tree files to install on rpi later

    cd  linux/arch/arm/boot/dts
    tar -czvf ../../../../../dist/devicetree.tgz bcm283*.dtb    
    # note:
    #   - for rpi we have : bcm27*.dtb and overlays/*  
    #   - for vanilla(kernel.org) kernel we have no overlays, we only have bcm283*.dtb  files for rpi board
    
    cd ../../../../..
 
 
 Build xenomai 3.0.8 user-space libraries and tools
 

    # configure details see : https://xenomai.org/installing-xenomai-3-x/#_configuring

	cd xenomai-3.0.8
	./scripts/bootstrap 
	./configure CFLAGS="-march=armv7-a  -mfloat-abi=hard -mfpu=neon -ffast-math" --host=arm-linux-gnueabihf --enable-smp --with-core=cobalt 
	
	
	mkdir /tmp/xenomai
	export DESTDIR=/tmp/xenomai  	
	make  DESTDIR="$DESTDIR"  install
	
	cd ..
	tar -C /tmp/xenomai -czvf dist/xenomai-3.0.8-binaries.tgz .
	
   
    
Now in the dist/ directory we have
 
    $ ls -1 dist
    devicetree.tgz
    linux-image-4.9.51-ipipe-dirty_4.9.51-ipipe-dirty_armhf.deb
    xenomai-3.0.8-binaries.tgz

Copy the distribution files over to raspberry pi running raspbian. There are multiple ways to do this, but I let your figure it out yourself.

Install on the raspberry pi the distribution files:

     # become root
     sudo su -
     
     # install kernel 
     # 1) install the debian kernel package
     dpkg -i linux-image-4.9.51-ipipe-dirty_4.9.51-ipipe-dirty_armhf.deb
     # 2) move the original kernel away (as backup)
     mv /boot/kernel7.img /boot/kernel7.orig.img 
     # 3) install the newly installed kernel as the default kernel to boot
     mv /boot/vmlinuz-4.9.51-ipipe-dirty /boot/kernel7.img
     
     # install the device tree description files in /boot
     tar -C /boot -xzvf devicetree.tgz 
     
     # install xenomai 3.0.8 user-space libraries and tools
     tar -C / -xzvf xenomai-3.0.8-binaries.tgz
     
 Finally with a text editor set the device tree to boot with 
 by adding the following line to the end of the file in /boot/config.txt
 
     device_tree=bcm2837-rpi-3-b-cobalt.dtb    
 
 then reboot and linux with xenomai should be up and running.
 
 
Test code
=========

On the following page we have a description of how I tested the new xenomai installation: 
  
    https://github.com/harcokuppens/xenomai3_rpi_gpio/blob/master/install/test_xenomai_installation_and_test_gpio_pins_are_working_correct.txt

It describes the following test code:

 * linux gpio test example code: 

        The example code is in a github repository at:
        
           https://github.com/harcokuppens/xenomai3_rpi_gpio/tree/master/examples/linux/wiringpi_examples

        The easiest way to download this folder is with the subversion command:
        
          svn checkout https://github.com/harcokuppens/xenomai3_rpi_gpio.git/trunk/examples/linux/wiringpi_examples
          

        With the default raspbian kernel these examples work. 
        However with a xenomai patched kernel the linux based examples should also still work!

          cd wiringpi_examples/
          make
          ./blink                # test gpio output
          ./button_toggles_led   # press button to toggle led  , test both interrupt from button and  output
          ./output_driven_irq    # test both  gpio output and gpio interrupt (latter driven by feeding output line with voltage divider to an input port -> no switch needed, just blinking of led toggles also interrupt)

* xenomai gpio test example code:  
       
        The example code is in a github repository at:
        
            https://github.com/harcokuppens/xenomai3_rpi_gpio/tree/master/examples/xenomai3/userspace_programs

        The easiest way to download this folder is with the subversion command:
        
            svn checkout https://github.com/harcokuppens/xenomai3_rpi_gpio.git/trunk/examples/xenomai3/userspace_programs
          
        These examples should only work with a xenomai3 patched linux kernel.    
        You also must have the rtdm driver for realtime driver support for gpio pins loaded:
       
            modprobe xeno_gpio_bcm2835
     
        We have the same examples as for linux but now in realtime mode:
       
           ./blink/                # test gpio output
           ./button_toggles_led/   # press button to toggle led  , test both interrupt from button and  output
           ./output_driven_irq/    # test both  gpio output and gpio interrupt (latter driven by feeding output line with voltage divider to an input port -> no switch needed, just blinking of led toggles also interrupt)
 
     
Problems
========
 
 
  * no network module loaded on the raspberry pi 3b+ boardHowever if we use the on raspberry 3b board the network works! So probably because  device tree description not right for 3b+ (yet).          
  * gpio pins not working (same for 3b+ as 3b boards)
 
    - in /dev/rtdm  the gpio pins are numbered from  970 to 1023
    
    - also wiringpi gpio examples don't work 
         * blink program doesn't work
         * isr program with interrupts doesn't work. Gives as error that it cannot open /sys/class/gpio/gpio23/value   => no such file.
         
    - though command "gpio readall" seems to work 
 
 
 
Retry with  pinctrl-bcm2835.c file from the raspbian with the ipipe patch applied.
================================================================================
 
The ipiped patched version  of pinctrl-bcm2835.c for kernel.org's kernel 4.9 doesn't work correctly, so instead we try to pinctrl-bcm2835.c file from the raspbian with the ipipe patch applied.

Note the ipipe patch on pinctrl-bcm2835.c  in raspbian's kernel didn't work right, and I had to fix it.
How I did this is described in 

     https://github.com/harcokuppens/xenomai3_rpi_gpio/blob/master/install/how_pinctrl-bcm2835_patch_for_rpi-4.9_is_derived/__README__.txt

Anyway finally I derived an working ipipe patch on pinctrl-bcm2835.c from raspbian's kernel  which we can download from:
 
  
       https://raw.githubusercontent.com/harcokuppens/xenomai3_rpi_gpio/master/install/how_pinctrl-bcm2835_patch_for_rpi-4.9_is_derived/rpi-4.9.y.ipipe-patched/pinctrl-bcm2835.c
 
 
Then use this file in the linux source :
    
       rm ./drivers/pinctrl/bcm/pinctrl-bcm2835.c
       wget https://raw.githubusercontent.com/harcokuppens/xenomai3_rpi_gpio/master/install/how_pinctrl-bcm2835_patch_for_rpi-4.9_is_derived/rpi-4.9.y.ipipe-patched/pinctrl-bcm2835.c
       cp pinctrl-bcm2835.c ./drivers/pinctrl/bcm/pinctrl-bcm2835.c
       
 We then rebuild and install kernel as described above.
    
 Then we find:  
 
  - in /dev/rtdm we have the right gpio files:  /dev/rtdm/pinctrl-bcm2835/gpio[0-53] 
   - with xeno_gpio_bcm2835 module loaded, the realtime example scripts do work correctly!
   - but without xeno_gpio_bcm2835 module loaded then still  wiringpi gpio examples don't work : writing to gpio pins and reading interrupt don't work  
   - however we have now  /sys/class/gpio/gpio23 after we run the isr example, so something went better. 
   - command "gpio readall" seems to work 
     
     
So gpio ports seem to work on xenomai but still something wrong with gpio ports within linux when using wiringpi library. 
