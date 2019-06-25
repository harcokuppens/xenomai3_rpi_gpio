[TOC]

2019 june 20 

Installation of xenomai 'master' branch on mainline kernel branch v4.19.33 from kernel.org  
============================================


### Download the ingredients
 
    # get xenomai 'master' branch
    git clone --depth=50 --branch=master https://github.com/xenomai-ci/xenomai.git  xenomai-master
    
    # get kernel branch v4.14.110  from kernel.org
    git clone -b v4.19.33 --depth 1 https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git linux

    # get ipipe patch for kernel
    wget https://www.xenomai.org/downloads/ipipe/v4.x/arm/ipipe-core-4.19.33-arm-2.patch


### Configuring
    
Patch the kernel with the ipipe patch
    
    xenomai-master/scripts/prepare-kernel.sh  --linux=linux/  --arch=arm  --ipipe=ipipe-core-4.19.33-arm-2.patch 

 
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




Note I didn't disable CONFIG_MIGRATION because didn't know how:
    
	    for older kernels we had    
	         Kernel Features  --->
	             [ ] Contiguous Memory Allocator
	             [ ] Allow for memory compaction
	    changed into 
	         Memory management options    (below default values)
	             [*] Allow for memory compaction    (CONFIG_COMPACTION)
	              * Contiguous Memory Allocato  (CONFIG_CMA) => couldn't disable this!!! Unclear why not???
	            => needed to disable for  CONFIG_MIGRATION (because Page migration may increase latency)
	               
	       => so decided to leave both on and thus CONFIG_MIGRATION on and thus have bigger latecny!


Note the same configuration can be reached with the commands

    scripts/config -d CPU_FREQ -d CPU_IDLE -d CMA -d COMPACTION -d KGDB \
                   -e ARM_ATAG_DTB_COMPAT_CMDLINE_EXTEND \
                   -e XENO_DRIVERS_GPIO -m XENO_DRIVERS_GPIO_BCM2835
    
    # run 'make olddefconfig' to fix all relations within config
    # because scripts/config doesn't check the validity of the .config file.
    # This is done at next time a make command is run.
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- olddefconfig

You can verify these options with the command

    scripts/config -s CPU_FREQ -s CPU_IDLE -s CMA -s COMPACTION -s KGDB \
                   -s ARM_ATAG_DTB_COMPAT_CMDLINE_EXTEND \
                   -s XENO_DRIVERS_GPIO -s XENO_DRIVERS_GPIO_BCM2835


Note that CONFIG_CMA is enabled again.

make distribution dir where we store our build packages
    
    cd ..
    mkdir dist/
 
### Build kernel into a debian package
 
    cd linux 
    export NUMCORES=4  
    time make -j $NUMCORES ARCH=arm  CROSS_COMPILE=arm-linux-gnueabihf- KBUILD_DEBARCH=armhf deb-pkg
    # does first : make clean
    # also does : make zImage modules dtbs   to finally build debian package

    # store debian package in distribution directory
    cd ../
    mv linux-image-*.deb dist/


 
### Build xenomai user-space libraries and tools
 

For Configure details see : https://xenomai.org/installing-xenomai-3-x/#_configuring

    export DESTDIR=$PWD/xenomai-build 
    
	cd xenomai-master
	./scripts/bootstrap 
	./configure CFLAGS="-march=armv7-a  -mfloat-abi=hard -mfpu=neon -ffast-math" --host=arm-linux-gnueabihf --enable-smp --with-core=cobalt
	
	make install
		
	cd ..
	tar -C $DESTDIR -czvf dist/xenomai-master-binaries.tgz .
	# cleanup with:  rm -rf $DESTDIR

   
    

### Installing distribution files

Now in the dist/ directory we have
 
    $ ls -1 dist
    linux-image-4.19.33-dirty_4.19.33-dirty-1_armhf.deb
    xenomai-master-binaries.tgz

Copy the distribution files over to raspberry pi running raspbian. There are multiple ways to do this, but I let your figure it out yourself.


Install on the raspberry pi the distribution files:

     # become root
     sudo su -
     
     # install kernel 
     # 1) install the debian kernel package
     dpkg -i linux-image-4.19.33-dirty_4.19.33-dirty-1_armhf.deb
     # 2) move the original kernel away (as backup)
     mv /boot/kernel7.img /boot/kernel7.orig.img 
     # 3) install the newly installed kernel as the default kernel to boot
     mv /boot/vmlinuz-4.19.33-dirty /boot/kernel7.img
     
     # install xenomai 3.0.8 user-space libraries and tools
     tar -C / -xzvf xenomai-master-binaries.tgz
      

### Setting device tree

Finally you have to set the right device tree description file for your specific raspberry pi hardware board so that the linux kernel is informed of that hardware.

All raspberry pi device tree files are build into the linux-image-*.deb package inside the /usr/lib/linux-image-.. dir. We can list them with the following command:

    dpkg -c linux-image-*.deb |egrep 'bcm28.*.dtb'
    -rw-r--r-- root/root     12518 2019-06-20 14:43 ./usr/lib/linux-image-4.19.33-dirty/bcm2835-rpi-b-rev2.dtb
    -rw-r--r-- root/root     13445 2019-06-20 14:43 ./usr/lib/linux-image-4.19.33-dirty/bcm2837-rpi-3-b.dtb
    -rw-r--r-- root/root     12428 2019-06-20 14:43 ./usr/lib/linux-image-4.19.33-dirty/bcm2835-rpi-a-plus.dtb
    -rw-r--r-- root/root     12434 2019-06-20 14:43 ./usr/lib/linux-image-4.19.33-dirty/bcm2835-rpi-b.dtb
    -rw-r--r-- root/root     12838 2019-06-20 14:43 ./usr/lib/linux-image-4.19.33-dirty/bcm2835-rpi-zero-w.dtb
    -rw-r--r-- root/root     12312 2019-06-20 14:43 ./usr/lib/linux-image-4.19.33-dirty/bcm2835-rpi-a.dtb
    -rw-r--r-- root/root      7933 2019-06-20 14:43 ./usr/lib/linux-image-4.19.33-dirty/bcm28155-ap.dtb
    -rw-r--r-- root/root     12914 2019-06-20 14:43 ./usr/lib/linux-image-4.19.33-dirty/bcm2836-rpi-2-b.dtb
    -rw-r--r-- root/root     12348 2019-06-20 14:43 ./usr/lib/linux-image-4.19.33-dirty/bcm2835-rpi-zero.dtb
    -rw-r--r-- root/root     12642 2019-06-20 14:43 ./usr/lib/linux-image-4.19.33-dirty/bcm2835-rpi-b-plus.dtb
    -rw-r--r-- root/root     12510 2019-06-20 14:43 ./usr/lib/linux-image-4.19.33-dirty/bcm2835-rpi-cm1-io1.dtb
    -rw-r--r-- root/root     13637 2019-06-20 14:43 ./usr/lib/linux-image-4.19.33-dirty/bcm2837-rpi-3-b-plus.dtb

    

Note that the raspbian kernel and the mainline kernel use different naming conventions for the device tree files for the different raspberry pi hardware boards:
 
- for rpi we have : bcm27\*.dtb and overlays/*  
- for vanilla(kernel.org) kernel we have no overlays, we only have bcm283\*.dtb <br>

The device tree file must be located in /boot to let the bootcode load it from the FAT /boot partition to give it to kernel. Therefore we have to copy all the device tree files from the place where the kernel installed them to /boot:

     # copy the device tree files for raspberry pi into /boot
     cp /usr/lib/linux-image-4.19.33-dirty/bcm283*.dtb /boot
     
With a text editor you can then set the device tree to boot your specific raspberry pi by adding that pi's device tree file to the end of the file in /boot/config.txt. 

Eg. for the raspberry pi 3b the following line should be added:
 
     device_tree=bcm2837-rpi-3-b.dtb   
  
But the bootcode used by the raspberry can also automatically load the right device tree description file without specify it in /boot/config.txt. However it requires the files to have specific names, which follows the naming convention within raspbian. So to get the loading working automatically do the following:
 
      cd /boot
      cp bcm2836-rpi-2-b.dtb	    bcm2709-rpi-2-b.dtb
      cp bcm2837-rpi-3-b.dtb	    bcm2710-rpi-3-b.dtb
      cp bcm2837-rpi-3-b-plus.dtb   bcm2710-rpi-3-b-plus.dtb
      # important: uncomment any device_tree= option in /boot/config.txt

Then reboot and linux with xenomai should be up and running for any raspberry pi 2 or 3 board!! 
For more information see [`../notes/device_tree_files_automatic_loading_on_raspbian_image.txt`](device_tree_files_automatic_loading_on_raspbian_image.txt).
 
 
### Make a single debian package

The approach we use is to take the build kernel package and modify it so that it install also the xenomai libraries/tools and that it installs the kernel and the device tree description files for the different hardware type of raspberry pi boards into /boot. It also copies the original /boot folder to /boot_<timestamp> folder so that we always can easily recover in case the installation may fail.

    cd dist/
    # extract 
    
    package=linux-image-4.19.33-dirty_4.19.33-dirty-1_armhf.deb
    
    # extract debian package
    dpkg-deb -R $package extracted/
    
    # add xenomai user-space libraries and tools
    tar -C extracted/ -xzvf xenomai-master-binaries.tgz
    
    # add post install code to install kernel and device tree files in /boot
    cat << EOF > extracted/etc/kernel/postinst.d/xenomai
    #!/bin/bash
    # get arguments 
    kernelversion="\$1"
    kernelimagefile="\$2"
    # backup original /boot for optional recovery
    /bin/cp -a /boot /boot_backup_\$(date +%s)
    # install kernel image file
    /bin/mv \$kernelimagefile /boot/kernel7.img
    # install device tree files for all rpi 2 and 3
    cd /usr/lib/linux-image-\${kernelversion}/
    /bin/cp bcm2836-rpi-2-b.dtb   /boot/bcm2709-rpi-2-b.dtb
    /bin/cp bcm2837-rpi-3-b.dtb   /boot/bcm2710-rpi-3-b.dtb
    /bin/cp bcm2837-rpi-3-b-plus.dtb   /boot/bcm2710-rpi-3-b-plus.dtb
    cd -
    EOF
    chmod a+x extracted/etc/kernel/postinst.d/xenomai
    
    # rebuild package
    # first make root owner of all files
    sudo chown -R root:root extracted/  
    dpkg-deb -b extracted  xenomai_$package
 
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
 
    - in /dev/rtdm  the gpio pins are numbered from 1994 to 2047 but should be from 0 to 53
    
    - also wiringpi gpio examples don't work 
         * blink program doesn't work
         * isr program with interrupts doesn't work. Gives as error that it cannot open /sys/class/gpio/gpio23/value   => no such file.
         
    - though command "gpio readall" seems to work 
 
 
