[TOC]

2019 june 27 

Installation of xenomai master branch on mainline kernel branch v4.19.y from kernel.org  
============================================


### Versions

    export XENOMAI_BRANCH=master
    # after cloning the we get the first 7 letters of the hash as part of version
    export XENOMAI_VERSION=master-7b5c53d
    export XENOMAI_GIT_REPO=https://gitlab.denx.de/Xenomai/xenomai.git

 
    # after cloning the kernel source you will get the real kernel
    # version with 'make kernelversion'   
    export KERNEL_VERSION=4.19.56 
    export KERNEL_BRANCH=rpi-4.19.y
    export KERNEL_GIT_REPO=https://github.com/raspberrypi/linux.git
    # for which defconfig to use see: https://github.com/raspberrypi/linux/wiki/Upstreaming
    export KERNEL_DEFCONFIG=bcm2709_defconfig
    # when kernel is build it adds the kernel version as suffix
    # but you can add an extra local version
    export KERNEL_LOCALVERSION="-rpi-xeno-$XENOMAI_VERSION"
    
    export PACKAGENAME=raspberrypi-kernel-image
    export PACKAGEVERSION=${KERNEL_VERSION}${KERNEL_LOCALVERSION}
    export PACKAGEFILE=${PACKAGENAME}-${PACKAGEVERSION}_armhf.deb
    

    #export IPIPE_PATCH=ipipe-core-4.19.33-arm-2.patch 
    #export IPIPE_REPO_URL=https://www.xenomai.org/downloads/ipipe/v4.x/arm/

    export IPIPE_PATCH=ipipe-core-4.19.33-arm-2.fixed.patch 
    export IPIPE_REPO_URL=https://raw.githubusercontent.com/harcokuppens/xenomai3_rpi_gpio/master/install/__INSTALL_FAILURES__/ 
        
    export NUMCORES=4
    export ARCH=arm 
    export CROSS_COMPILE=arm-linux-gnueabihf-

### Download the ingredients
 
    # get xenomai
    git clone --depth=1 --branch=$XENOMAI_BRANCH $XENOMAI_GIT_REPO xenomai
    
    # get kernel 
    git clone -b $KERNEL_BRANCH --depth 1 $KERNEL_GIT_REPO linux
       
    # get ipipe patch for kernel
    wget $IPIPE_REPO_URL/$IPIPE_PATCH
    
Note the specific version details for future rebuilds:
    
    $ cd linux
    $ make kernelversion 
    4.19.56 
    $ git rev-parse HEAD
    7b5c53d89a53e343679759476a27bb7af736ea22
    $ cd -  
    
    cd xenomai
    $ git rev-parse HEAD
    2692220a5d55a5f36ebaee3bfde5e926f168447c
    $ cd -

Update the kernel version to the real ke

### Configuring
    
Patch the kernel with the ipipe patch
    
    xenomai/scripts/prepare-kernel.sh  --linux=linux/  --arch=$ARCH  --ipipe=$IPIPE_PATCH 

 
 Configure the kernel
 
    cd linux
    make $KERNEL_DEFCONFIG
    
    
 Within menuconfig choose the following options:
     
     $ make menuconfig
 
     General setup  --->
        Local version - append to kernel release
         -> set string in popup window to: "${KERNEL_LOCALVERSION}"
     CPU Power Management  --->
       CPU Frequency scaling  --->  
         [ ] CPU Frequency scaling (CONFIG_CPU_FREQ)
       CPU idle  --->
         [ ] CPU idle PM support (CONFIG_CPU_IDLE)
     Memory management options    
         [ ] Allow for memory compaction (CONFIG_COMPACTION)
         [ ] Contiguous Memory Allocation  (CONFIG_CMA)         
     Kernel Hacking  --->
         [ ] KGDB: kernel debugger (CONFIG_KGDB)
     Boot options  --->
     
            [*] Use appended device tree blob to zImage (EXPERIMENTAL) (CONFIG_ARM_APPENDED_DTB)                                                                        
            [*]   Supplement the appended DTB with traditional ATAG information (CONFIG_ARM_ATAG_DTB_COMPAT)                                                                    
                     Kernel command line type (Extend with bootloader kernel arguments)  --->
                       [X] Extend bootloader kernel arguments (CONFIG_ARM_ATAG_DTB_COMPAT_CMDLINE_EXTEND)

     Xenomai Cobalt --->
          Drivers  --->
                  Real-time GPIO drivers  --->
                     <*> GPIO controller (CONFIG_XENO_DRIVERS_GPIO)                                              
                     [M]   Support for BCM2835 GPIOs (CONFIG_ XENO_DRIVERS_GPIO_BCM2835)                                     
                     [ ]   Enable GPIO core debugging features

     note: in above  [ ]  means disable,  [X]  means enable in kernel, [M] means build as external module for kernel
     
     => we could also build support for BCM2835 GPIOs directly in kernel however we prefer the modular approach


Note the same configuration can be reached with the commands

    scripts/config --set-str LOCALVERSION "${KERNEL_LOCALVERSION}" \
                   -d CPU_FREQ -d CPU_IDLE -d CMA -d COMPACTION -d KGDB \
                   -e ARM_APPENDED_DTB -e ARM_ATAG_DTB_COMPAT -e ARM_ATAG_DTB_COMPAT_CMDLINE_EXTEND \
                   -e XENO_DRIVERS_GPIO -m XENO_DRIVERS_GPIO_BCM2835
    
Then run 'make olddefconfig' to fix all relations within config
because scripts/config doesn't check the validity of the .config file.

    make olddefconfig

Finally you can verify the set options with the command

    for OPTION in LOCALVERSION  CPU_FREQ CPU_IDLE CMA COMPACTION KGDB \
      ARM_APPENDED_DTB ARM_ATAG_DTB_COMPAT ARM_ATAG_DTB_COMPAT_CMDLINE_EXTEND \
      XENO_DRIVERS_GPIO XENO_DRIVERS_GPIO_BCM2835 MIGRATION
    do 
       echo "${OPTION}: $(scripts/config -s ${OPTION})"   
    done   

We also check status of MIGRATION because it is needed to decrease latency. The config option MIGRATION is disabled indirectly by disabling CMA and COMPACTION. Note that if you enable either of them then 'make menuconfig' gives the warning:

    *** WARNING! Page migration (CONFIG_MIGRATION) may increase latency. ***    
 
                   
#### Fixing olddefconfig warning


In this specific case when we run 'make olddefconfig' we get a warning:

    $ make olddefconfig
    scripts/kconfig/conf  --olddefconfig Kconfig
    
    WARNING: unmet direct dependencies detected for BCM_VC_SM_CMA
      Depends on [n]: STAGING [=y] && BCM_VIDEOCORE [=y] && BCM2835_VCHIQ [=y] && DMA_CMA [=n]
      Selected by [m]:
      - BCM2835_VCHIQ_MMAL [=m] && STAGING [=y] && BCM_VIDEOCORE [=y] && (ARCH_BCM2835 [=y] || COMPILE_TEST [=n])
    #
    # configuration written to .config
    #


I couldn't disable `BCM_VC_SM_CMA`, because it was turned on automatically by other options when running 'make olddefconfig'. So instead I decided to enable  "Contiguous Memory Allocation" again to get rid of this WARNING. We did that with:

    scripts/config -e DMA_CMA -e  CMA
    make olddefconfig


So finally in our case we get:

    $for OPTION in LOCALVERSION  CPU_FREQ CPU_IDLE CMA COMPACTION KGDB \
    >  ARM_APPENDED_DTB ARM_ATAG_DTB_COMPAT ARM_ATAG_DTB_COMPAT_CMDLINE_EXTEND \
    >  XENO_DRIVERS_GPIO XENO_DRIVERS_GPIO_BCM2835 MIGRATION
    >do
    >   echo "${OPTION}: $(scripts/config -s ${OPTION})"
    >done
    LOCALVERSION: -rpi-xeno-master
    CPU_FREQ: n
    CPU_IDLE: n
    CMA: y
    COMPACTION: n
    KGDB: n
    ARM_APPENDED_DTB: y
    ARM_ATAG_DTB_COMPAT: y
    ARM_ATAG_DTB_COMPAT_CMDLINE_EXTEND: y
    XENO_DRIVERS_GPIO: y
    XENO_DRIVERS_GPIO_BCM2835: m
    MIGRATION: y


Make a distribution dir where we store our build packages
    
    cd ..
    mkdir dist/
 
### Build kernel into a debian package
 
    cd linux   
    
    time make -j $NUMCORES KBUILD_DEBARCH=armhf deb-pkg
    # does first : make clean
    # also does : make zImage modules dtbs to finally build debian package
    # builds debian packages: 
    #   linux-image*.deb linux-headers*.deb linux-libc-dev*.deb
    
    # store debian package in distribution directory
    cd ../
    mv linux-image-*.deb linux-headers*.deb linux-libc-dev*.deb dist/

 
### Build xenomai user-space libraries and tools
 

For Configure details see : https://xenomai.org/installing-xenomai-3-x/#_configuring

    export DESTDIR=$PWD/xenomai-build 
    
	 cd xenomai
	 ./scripts/bootstrap 
	 ./configure CFLAGS="-march=armv7-a  -mfloat-abi=hard -mfpu=neon -ffast-math" --host=arm-linux-gnueabihf --enable-smp --with-core=cobalt
	
	 make install
		
	 cd ..
	 tar -C $DESTDIR -czvf dist/xenomai-binaries.tgz .
	 # cleanup with:  rm -rf $DESTDIR

   

### Installing manually

#### Installing the distribution files

Now in the dist/ directory we have
 
    $ ls -1 dist
    linux-image-${KERNEL_VERSION}${KERNEL_LOCALVERSION}-1_armhf.deb
    xenomai-binaries.tgz

Copy the distribution files over to raspberry pi running raspbian. There are multiple ways to do this, but I let your figure it out yourself.


Install on the raspberry pi the distribution files:

     # become root
     sudo su -
     
     # install kernel 
     # 1) install the debian kernel package
     dpkg -i linux-image-${KERNEL_VERSION}${KERNEL_LOCALVERSION}-1_armhf.deb
     # 2) move the original kernel away (as backup)
     mv /boot/kernel7.img /boot/kernel7.orig.img 
     # 3) install the newly installed kernel as the default kernel to boot
     mv /boot/vmlinuz-${KERNEL_VERSION}${KERNEL_LOCALVERSION} /boot/kernel7.img
     
     # install xenomai user-space libraries and tools
     tar -C / -xzvf xenomai-binaries.tgz
      

#### Setting device tree

Finally you have to set the right device tree description file for your specific raspberry pi hardware board so that the linux kernel is informed of that hardware.

All raspberry pi device tree files are build into the linux-image-*.deb package inside the /usr/lib/linux-image-.. dir. We can list them with the following command:


    $ dpkg -c linux-image-*.deb |egrep '/bcm27.*.dtb'
    -rw-r--r-- root/root     23946 2019-06-25 18:33 ./usr/lib/linux-image-4.19.56-rpi-4.19.y-xenomai-master+/bcm2708-rpi-b.dtb
    -rw-r--r-- root/root     26463 2019-06-25 18:33 ./usr/lib/linux-image-4.19.56-rpi-4.19.y-xenomai-master+/bcm2710-rpi-3-b.dtb
    -rw-r--r-- root/root     25261 2019-06-25 18:33 ./usr/lib/linux-image-4.19.56-rpi-4.19.y-xenomai-master+/bcm2710-rpi-cm3.dtb
    -rw-r--r-- root/root     40264 2019-06-25 18:33 ./usr/lib/linux-image-4.19.56-rpi-4.19.y-xenomai-master+/bcm2711-rpi-4-b.dtb
    -rw-r--r-- root/root     23671 2019-06-25 18:33 ./usr/lib/linux-image-4.19.56-rpi-4.19.y-xenomai-master+/bcm2708-rpi-zero.dtb
    -rw-r--r-- root/root     24407 2019-06-25 18:33 ./usr/lib/linux-image-4.19.56-rpi-4.19.y-xenomai-master+/bcm2708-rpi-zero-w.dtb
    -rw-r--r-- root/root     27082 2019-06-25 18:33 ./usr/lib/linux-image-4.19.56-rpi-4.19.y-xenomai-master+/bcm2710-rpi-3-b-plus.dtb
    -rw-r--r-- root/root     25293 2019-06-25 18:33 ./usr/lib/linux-image-4.19.56-rpi-4.19.y-xenomai-master+/bcm2709-rpi-2-b.dtb
    -rw-r--r-- root/root     23723 2019-06-25 18:33 ./usr/lib/linux-image-4.19.56-rpi-4.19.y-xenomai-master+/bcm2708-rpi-cm.dtb
    -rw-r--r-- root/root     24205 2019-06-25 18:33 ./usr/lib/linux-image-4.19.56-rpi-4.19.y-xenomai-master+/bcm2708-rpi-b-plus.dtb



Note that the raspbian kernel and the mainline kernel use different naming conventions for the device tree files for the different raspberry pi hardware boards:
 
- for rpi we have : bcm27\*.dtb and overlays/*  
- for vanilla(kernel.org) kernel we have no overlays, we only have bcm283\*.dtb <br>

The device tree file must be located in /boot to let the bootcode load it from the FAT /boot partition to give it to kernel. Therefore we have to copy all the device tree files from the place where the kernel installed them to /boot:

     # copy the device tree files for raspberry pi into /boot
     /bin/cp /usr/lib/linux-image-*/bcm27*.dtb /boot
     # copy the overlay files into /boot/overlays
     /bin/cp -a /usr/lib/linux-image-*/overlays /boot
     
With a text editor you can then set the device tree to boot your specific raspberry pi by adding that pi's device tree file to the end of the file in /boot/config.txt. 

Eg. for the raspberry pi 3b the following line should be added:
 
     device_tree=bcm2710-rpi-3-b-plus.dtb   
  
However the bootcode used by the raspbian can automatically load the right device tree description file without specifying it in /boot/config.txt. So you can omit setting the `device_tree` in  /boot/config.txt.
Then reboot and linux with xenomai should be up and running for any raspberry pi 2 or 3 board!! 
For more information see [`../notes/device_tree_files_automatic_loading_on_raspbian_image.txt`](../notes/device_tree_files_automatic_loading_on_raspbian_image.txt).
 
 
### Automatic installation

#### Make a single debian package for easy installation

Instead of installing the distribution files as described in the step above and doing the device tree setup manually, it would be much easier if all of this would be done by a single debian package we just have to download and install. In this section we are going to build such a debian package.
  
The approach we use is to take the build kernel package and modify it so that it install also the xenomai libraries/tools and that it installs the kernel and the device tree description files for the different hardware type of raspberry pi boards into /boot. It also copies the original /boot folder to /boot_<timestamp> folder so that we always can easily recover in case the installation may fail.

    cd dist/
    
    # extract debian package
    dpkg-deb -R linux-image-*.deb extracted/
    
    # add xenomai user-space libraries and tools
    tar -C extracted/ -xzvf xenomai-binaries.tgz
    
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
    # copy the device tree files for raspberry pi into /boot
    /bin/cp bcm27*.dtb /boot
    # copy the overlay files into /boot/overlays
    /bin/cp -a overlays /boot
    cd -
    EOF
    
    # make executable
    chmod a+x extracted/etc/kernel/postinst.d/xenomai
    
    # change package name and version in debian control file
    sed -i "s/^Package:.*/Package: $PACKAGENAME/" extracted/DEBIAN/control
    sed -i "s/^Version:.*/Version: $PACKAGEVERSION/" extracted/DEBIAN/control   
    
    # rebuild package
    # first make root owner of all files
    sudo chown -R root:root extracted/  
    dpkg-deb -b extracted  $PACKAGEFILE
 
#### Automatic installation

Just download the debian package we build in the previous onto the raspberry pi, and then just run:

    dpkg -i $package
   
 
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
 
     

