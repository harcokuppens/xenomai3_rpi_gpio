

2019 juli 3

[TOC]

Installation of xenomai 3.08 on rpi kernel v4.9.y from raspberrypi github site  
============================================

**IMPORTANT**: This mainline kernel installation is not correct. The gpio pins have the wrong numbers.
We try to bring stuff from the rpi kernel to improve, however not success full so far, but we note down these trials in appendix B.

This kernel boots on rpi2/3b/3b+ but doesn't boot on newer rpi4.

### Terminology 

The raspberry pi project has its own downstream version of the official upstream kernel at kernel.org which are also known under the alias mainline or vanilla.

    mainline = upstream   = vanilla      https://git.kernel.org/
    rpi     =  downstream                https://github.com/raspberrypi/linux/

In this document we use the the term `mainline` and `rpi`.



### Specialities for this build


- the official ipipe patch needed a simple fix : repair for shifting lines and small context changes, and we remove the patches 
  for pinctrl-bcm2835.c 
- because we use a new patch for pinctrl-bcm2835.c for raspbian 4.9 kernel : pinctrl-bcm2835.c.rpi-4.9.patch
  for details how derived this patch see: 
    
        https://github.com/harcokuppens/xenomai3_rpi_gpio/blob/master/install/how_pinctrl-bcm2835_patch_for_rpi-4.9_is_derived/__README__.txt


### Versions

```bash

export XENOMAI_VERSION=3.0.8
export XENOMAI_BRANCH=v$XENOMAI_VERSION
export XENOMAI_GIT_REPO=https://gitlab.denx.de/Xenomai/xenomai.git

# after cloning the kernel source you will get the real kernel
# version with 'make kernelversion'
export KERNEL_VERSION=4.9.80  
export KERNEL_BRANCH=rpi-4.9.y
export KERNEL_GIT_REPO=https://github.com/raspberrypi/linux.git
    
# for which defconfig to use see: 
#    https://github.com/raspberrypi/linux/wiki/Upstreaming
export KERNEL_DEFCONFIG=bcm2709_defconfig

# When a kernel is build it adds the kernel version as suffix
# but you can add an extra local version.
# In this local version  we must mark rpi kernels by adding the "rpi" label
# otherwise we assume it is a mainline kernel.
export KERNEL_LOCALVERSION="-rpi-xeno-$XENOMAI_VERSION"
export KERNEL_FULLVERSION=${KERNEL_VERSION}${KERNEL_LOCALVERSION}
# note: whether KERNEL_FULLVERSION contains "rpi" is used to
#       decide which device tree files are used in the installation.
    
#note: the raspbian kernel package is called: raspberrypi-kernel 
#note: "xeno" in the version label marks it a kernel with xenomai support   
export PACKAGENAME=raspberrypi-kernel
export PACKAGEVERSION=${KERNEL_FULLVERSION}
export PACKAGEFILE=${PACKAGENAME}-${PACKAGEVERSION}_armhf.deb
    

# official ipipe patch needed a simple fix
#export IPIPE_PATCH=ipipe-core-4.9.51-arm-4.patch
#export IPIPE_REPO_URL=https://www.xenomai.org/downloads/ipipe/v4.x/arm/

# this simple fix, shifting lines and small context changes, we can download from
export IPIPE_PATCH=ipipe-core-4.9.51-arm-4.fixedforrasbian-4.9.80.patch 
export IPIPE_REPO_URL=https://raw.githubusercontent.com/harcokuppens/xenomai3_rpi_gpio/master/install/how_pinctrl-bcm2835_patch_for_rpi-4.9_is_derived/

    
export NUMCORES=4
export ARCH=arm 
export CROSS_COMPILE=arm-linux-gnueabihf-
```    

### Download the ingredients
 
 
```bash 
# get xenomai       
git clone --depth=1 --branch=$XENOMAI_BRANCH $XENOMAI_GIT_REPO xenomai
    
# get kernel 
git clone -b $KERNEL_BRANCH --depth 1 $KERNEL_GIT_REPO linux
   
# get ipipe patch for kernel
wget $IPIPE_REPO_URL/$IPIPE_PATCH

# we use a new patch for pinctrl-bcm2835.c for raspbian 4.9 kernel : pinctrl-bcm2835.c.rpi-4.9.patch
wget $IPIPE_REPO_URL/pinctrl-bcm2835.c.rpi-4.9.patch
# for details how derived this patch see: https://github.com/harcokuppens/xenomai3_rpi_gpio/blob/master/install/how_pinctrl-bcm2835_patch_for_rpi-4.9_is_derived/__README__.txt
```
    
#### Note down the specific version details for future rebuilds

```bash    
cd linux
make kernelversion 
# output: 4.9.80 
git rev-parse HEAD
# output: 7f9c648dad6473469b4133898fa6bb8d818ecff9
cd -  
    
cd xenomai
git rev-parse HEAD
# output: fbc3271096c63b21fe895c66ba20b1d10d72ff48
cd -
```

### Patch kernel with the ipipe patch and pinctrl-bcm2835.c.rpi-4.9.patch

Patch the kernel with the ipipe patch
    
```bash
xenomai/scripts/prepare-kernel.sh  --linux=linux/  --arch=$ARCH  --ipipe=$IPIPE_PATCH 
```

Patch linux/drivers/pinctrl/bcm/pinctrl-bcm2835.c

```bash
cd linux/drivers/pinctrl/bcm/
patch   -p1  <  ../../../../pinctrl-bcm2835.c.rpi-4.9.patch
cd -
```

### Configuring
    
 
We describe here two ways to configure the kernel. Manually via a menu with some explanation. Or configuration via commands which you can easily execute,

#### Configure the kernel config via a menu 

First set a default config to start from:

```bash    
cd linux
make $KERNEL_DEFCONFIG
```
    
Then start menuconfig 
 
```bash    
make menuconfig
```
 
The default configuration was made to fit specifically for the 
Broadcom BCM2835 chip. (config options `CONFIG_ARCH_BCM=y` and 
`CONFIG_ARCH_BCM2835=y`).

We can configure for realtime behavior by choosing the following options: 

     General setup  --->
        Local version - append to kernel release
         -> set string in popup window to: "${KERNEL_LOCALVERSION}"
        [ ] Initial RAM filesystem and RAM disk (initramfs/initrd) support (CONFIG_BLK_DEV_INITRD) 
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
 
     Xenomai Cobalt --->
          Drivers  --->
                  Real-time GPIO drivers  --->
                     <*> GPIO controller (CONFIG_XENO_DRIVERS_GPIO)                                              
                     [M]   Support for BCM2835 GPIOs (CONFIG_ XENO_DRIVERS_GPIO_BCM2835)                                     
                     [ ]   Enable GPIO core debugging features

     note: in above  [ ]  means disable,  [X]  means enable in kernel, [M] means build as external module for kernel
     
     => we could also build support for BCM2835 GPIOs directly in kernel however we prefer the modular approach


##### initrd  

We disable `initrd` by disabling the option "Initial RAM filesystem and RAM disk (initramfs/initrd) support (`CONFIG_BLK_DEV_INITRD`)".  We disable `initrd`  because the raspberry pi doesn't use it on boot. If you would leave it enabled then when installing the kernel debian package it would create an `initrd` file in `/boot` which is never used.  


##### kernel arguments  

Finally in older installation descriptions often in "Boot options" we had to enable an option "Extend bootloader kernel arguments". However with the new bootloader in raspbian the kernel arguments can be configured in `/boot/cmdline.txt` which is read by the bootloader. 

For specific kernel arguments see:  

    https://www.raspberrypi.org/documentation/configuration/cmdline-txt.md 

For detailed explanation how the bootloader reads the kernel arguments  from `/boot/cmdline.txt`  and how it sets them in the device tree see:

[`kernel_config__extend_bootloader_kernel_arguments.txt`](kernel_config__extend_bootloader_kernel_arguments.txt). 


#### Configure the kernel config using commands
    
The same configuration using the menu can also be reached with the commands below.

First set a default config to start from:

```bash    
cd linux
make $KERNEL_DEFCONFIG
```


Then configure kernel for realtime support:

```bash
scripts/config --set-str LOCALVERSION "${KERNEL_LOCALVERSION}" -d BLK_DEV_INITRD\
   -d CPU_FREQ -d CPU_IDLE -d CMA -d COMPACTION -d KGDB\
   -e XENO_DRIVERS_GPIO -m XENO_DRIVERS_GPIO_BCM2835
```
    
Then run 'make olddefconfig' to fix all relations within config
because scripts/config doesn't check the validity of the .config file.

```bash
make olddefconfig
```


Finally you can verify the set options with the command

```bash
for OPTION in LOCALVERSION BLK_DEV_INITRD\
  CPU_FREQ CPU_IDLE CMA COMPACTION KGDB \
  XENO_DRIVERS_GPIO XENO_DRIVERS_GPIO_BCM2835 MIGRATION 
do 
   echo "${OPTION}: $(scripts/config -s ${OPTION})"   
done   
```

We also check status of MIGRATION because it is needed to decrease latency. The config option MIGRATION is disabled indirectly by disabling CMA and COMPACTION. Note that if you enable either of them then 'make menuconfig' gives the warning:

    *** WARNING! Page migration (CONFIG_MIGRATION) may increase latency. ***    
 

We finally get:

```bash
$ for OPTION in LOCALVERSION BLK_DEV_INITRD\
>   CPU_FREQ CPU_IDLE CMA COMPACTION KGDB \
>   XENO_DRIVERS_GPIO XENO_DRIVERS_GPIO_BCM2835 MIGRATION
> do
>    echo "${OPTION}: $(scripts/config -s ${OPTION})"
> done
LOCALVERSION: -rpi-xeno-3.0.8
BLK_DEV_INITRD: n
CPU_FREQ: n
CPU_IDLE: n
CMA: n
COMPACTION: n
KGDB: n
XENO_DRIVERS_GPIO: y
XENO_DRIVERS_GPIO_BCM2835: m
MIGRATION: undef
```


### Building

First make a distribution dir where we store our build packages
 
```bash    
cd ..
mkdir dist/
```

#### Build kernel into a debian package



Then build the kernel packages:
 
```bash 
cd linux   
    
time make -j $NUMCORES KBUILD_DEBARCH=armhf deb-pkg
# does first : make clean
# also does : make zImage modules dtbs to finally build debian package
# builds debian packages: 
#   linux-image*.deb linux-headers*.deb linux-libc-dev*.deb
    
# store debian package in distribution directory
cd ../
mv linux-image-*.deb linux-headers*.deb linux-libc-dev*.deb dist/
```

 
#### Build xenomai user-space libraries and tools
 

For Configure details see : https://xenomai.org/installing-xenomai-3-x/#_configuring   
    
```bash
export DESTDIR=$PWD/xenomai-build 
    
 cd xenomai
 ./scripts/bootstrap 
 ./configure CFLAGS="-march=armv7-a  -mfloat-abi=hard -mfpu=neon -ffast-math" --host=arm-linux-gnueabihf --enable-smp --with-core=cobalt
	
sudo make DESTDIR=$DESTDIR install	
   		
cd ..
tar -C $DESTDIR -czvf dist/xenomai-binaries.tgz .
# cleanup with:  
#sudo rm -rf $DESTDIR
```


### Installing
   
#### Installing manually

Now in the dist/ directory we have
 
```bash 
$ ls -1 dist
linux-image-${KERNEL_FULLVERSION}*_armhf.deb
xenomai-binaries.tgz
``` 

##### Installing the distribution files on the mounted sdcard of the Raspberry pi 
    
```bash    
# become root with preserving the environment
sudo -E su -p
    
# mount sdcard
export MOUNTPOINT=/media/sdcard 
export SDCARD=/dev/sdb
mkdir ${MOUNTPOINT}
mount ${SDCARD}2 ${MOUNTPOINT}
mount ${SDCARD}1 ${MOUNTPOINT}/boot
 
# backup original /boot for optional recovery
/bin/cp -a ${MOUNTPOINT}/boot ${MOUNTPOINT}/boot_backup
 
# install kernel, modules and device tree files
# 1) extract the debian kernel package on the sdcard
dpkg-deb -xv linux-image-*.deb ${MOUNTPOINT}
# 2) install the newly installed kernel as the default kernel to boot
/bin/mv ${MOUNTPOINT}/boot/vmlinuz-${KERNEL_FULLVERSION}* ${MOUNTPOINT}/boot/kernel7.img
# 3) install device tree files
cd ${MOUNTPOINT}/usr/lib/linux-image-${KERNEL_FULLVERSION}/
if [[ "$KERNEL_FULLVERSION" == *rpi* ]]
then 
    # for rpi kernel use the raspbian specific bcm27 device tree files
    /bin/cp bcm27*.dtb ${MOUNTPOINT}/boot
    # for rpi also copy the overlay files into /boot/overlays
    /bin/cp -a overlays ${MOUNTPOINT}/boot
else
    # for mainline kernel use the bcm28 files, however give
    # them the names the raspbian bootloader expect (that is
    # the  raspbian specific bcm27 names)
    /bin/cp bcm2836-rpi-2-b.dtb   ${MOUNTPOINT}/boot/bcm2709-rpi-2-b.dtb
    /bin/cp bcm2837-rpi-3-b.dtb   ${MOUNTPOINT}/boot/bcm2710-rpi-3-b.dtb
    /bin/cp bcm2837-rpi-3-b-plus.dtb   ${MOUNTPOINT}/boot/bcm2710-rpi-3-b-plus.dtb
    # note: kernel.org doesn't yet have dts file for rpi-4; just released
fi  
cd -  
  
# install xenomai user-space libraries and tools
tar -C ${MOUNTPOINT} -xzvf xenomai-binaries.tgz
 
# umount
umount ${MOUNTPOINT}/boot ${MOUNTPOINT}/
```

For details how we installed the device tree files see [Appendix A: Details about device tree files](#Appendix-A:-Details-about-device-tree-files).

##### Installing the distribution files on the Raspberry pi itself


Copy the distribution files over to raspberry pi running raspbian. There are multiple ways to do this, but I let your figure it out yourself.


Install on the raspberry pi the distribution files:

```bash
# become root
sudo -E su -p
 
# backup original /boot for optional recovery
/bin/cp -a /boot /boot_backup
 
# install kernel, modules and device tree files
# 1) extract the debian kernel package on the sdcard
dpkg -i linux-image-*.deb     
# 2) install the newly installed kernel as the default kernel to boot
/bin/mv /boot/vmlinuz-${KERNEL_FULLVERSION}* /boot/kernel7.img
# 3) install device tree files
cd /usr/lib/linux-image-${KERNEL_FULLVERSION}/
if [[ "$KERNEL_FULLVERSION" == *rpi* ]]
then 
    # for rpi kernel use the raspbian specific bcm27 device tree files
    /bin/cp bcm27*.dtb /boot
    # for rpi also copy the overlay files into /boot/overlays
    /bin/cp -a overlays /boot
else
    # for mainline kernel use the bcm28 files, however give
    # them the names the raspbian bootloader expect (that is
    # the  raspbian specific bcm27 names)
    /bin/cp bcm2836-rpi-2-b.dtb   /boot/bcm2709-rpi-2-b.dtb
    /bin/cp bcm2837-rpi-3-b.dtb   /boot/bcm2710-rpi-3-b.dtb
    /bin/cp bcm2837-rpi-3-b-plus.dtb   /boot/bcm2710-rpi-3-b-plus.dtb
    # note: kernel.org doesn't yet have dts file for rpi-4; just released
fi  
cd - 
  
# install xenomai user-space libraries and tools
tar -C / -xzvf xenomai-binaries.tgz
```      





#### Easy installation

##### Make a single debian package for easy installation

Instead of installing the distribution files as described in the step above and doing the device tree setup manually, it would be much easier if all of this would be done by a single debian package we just have to download and install. In this section we are going to build such a debian package.
  
The approach we use is to take the build kernel package and modify it so that it install also the xenomai libraries/tools and that it installs the kernel and the device tree description files for the different hardware type of raspberry pi boards into /boot. It also copies the original /boot folder to /boot_<timestamp> folder so that we always can easily recover in case the installation may fail.


```bash
cd dist/
    
# extract debian package
dpkg-deb -R linux-image-*.deb extracted/
    
# add xenomai user-space libraries and tools
sudo tar -C extracted/ -xzvf xenomai-binaries.tgz
    
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
# by copying the device tree files for raspberry pi into /boot
# for rpi kernel use the raspbian specific bcm27 device tree files
#/bin/cp /usr/lib/linux-image-\${kernelversion}/bcm27*.dtb /boot

# install device tree files for all rpi 2 and 3
# by copying the device tree files for raspberry pi into /boot
cd /usr/lib/linux-image-\${kernelversion}/
if [[ "\$kernelversion" == *rpi* ]]
then 
    # for rpi kernel use the raspbian specific bcm27 device tree files
    /bin/cp bcm27*.dtb /boot
    # for rpi also copy the overlay files into /boot/overlays
    /bin/cp -a overlays /boot
else
    # for mainline kernel use the bcm28 files, however give
    # them the names the raspbian bootloader expect (that is
    # the  raspbian specific bcm27 names)
    /bin/cp bcm2836-rpi-2-b.dtb   /boot/bcm2709-rpi-2-b.dtb
    /bin/cp bcm2837-rpi-3-b.dtb   /boot/bcm2710-rpi-3-b.dtb
    /bin/cp bcm2837-rpi-3-b-plus.dtb   /boot/bcm2710-rpi-3-b-plus.dtb
    # note: kernel.org doesn't yet have dts file for rpi-4; just released
fi    

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
```
 
##### Installation

Just download the debian package we build in the previous onto the raspberry pi, and then just run:

```bash 
sudo dpkg -i $PACKAGEFILE
```   
 
 

Test code
=========

On the following page we have a description of how I tested the new xenomai installation: 
  
    https://github.com/harcokuppens/xenomai3_rpi_gpio/blob/master/install/test_xenomai_installation_and_test_gpio_pins_are_working_correct.txt

It describes the following test code:

 * linux gpio test example code: 

        The example code is in a github repository at:
        
           https://github.com/harcokuppens/xenomai3_rpi_gpio/tree/master/examples/linux/wiringpi_examples

        The easiest way to download this folder is with the subversion command:
        
          svn export https://github.com/harcokuppens/xenomai3_rpi_gpio.git/trunk/examples/linux/wiringpi_examples
          

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
        
            svn export https://github.com/harcokuppens/xenomai3_rpi_gpio.git/trunk/examples/xenomai3/userspace_programs
          
        These examples should only work with a xenomai3 patched linux kernel.    
        You also must have the rtdm driver for realtime driver support for gpio pins loaded:
       
            modprobe xeno_gpio_bcm2835
     
        We have the same examples as for linux but now in realtime mode:
       
           ./blink/                # test gpio output
           ./button_toggles_led/   # press button to toggle led  , test both interrupt from button and  output
           ./output_driven_irq/    # test both  gpio output and gpio interrupt (latter driven by feeding output line with voltage divider to an input port -> no switch needed, just blinking of led toggles also interrupt)
 


Problems
========
 
 
         
gpio pins not working in linux

- wiringpi gpio examples don't work: 
    * wiringpi blink program doesn't work
    * wiringpi isr program with interrupts doesn't work. Gives as error that it cannot open /sys/class/gpio/gpio23/value   => no such file.
         
- though command "gpio readall" seems to work 
 
 


Appendices
==========
     
Appendix A:  Details about device tree files
--------------------------------------------


###  Device file naming and auto loading by raspbian bootloader
    
With a text editor you can then set the device tree to boot your specific raspberry pi by adding that pi's device tree file to the end of the file in /boot/config.txt. 

Eg. for the raspberry pi 3b the following line should be added:
 
     device_tree=bcm2837-rpi-3-b.dtb   
  
But the boot code used by the raspberry can also automatically load the right device tree description file without specify it in `/boot/config.txt` as long as the device tree files follow the naming convention of the rpi kernel.
That's why we copied for mainline kernel the device tree files to the `/boot` directory with the rpi name and didn't set any `device_tree=` option in `/boot/config.txt`.

For more information see [`../notes/device_tree_files_automatic_loading_on_raspbian_image.txt`](../notes/device_tree_files_automatic_loading_on_raspbian_image.txt). 



### Detailed explanation how to install device tree files

So you have to set the right device tree description file for your specific raspberry pi hardware board so that the linux kernel is informed of that hardware.

All raspberry pi device tree files are build into the linux-image-*.deb package inside the /usr/lib/linux-image-.. dir. We can list them with the following command:

```bash
$ dpkg -c linux-image-*.deb |egrep '/bcm.*.dtb'
```

Note that the raspbian kernel and the mainline kernel use different naming conventions for the device tree files for the different raspberry pi hardware boards:
 
- for rpi kernel: `bcm27*.dtb` and `overlays/*`  
- for mainline kernel: `bcm283*.dtb` (no overlays)<br>

The device tree file must be located in /boot to let the boot code load it from the FAT /boot partition to give it to kernel. Therefore we have to copy all the device tree files from the place where the kernel installed them to /boot:

* for the rpi kernel

    ```bash
     # copy the device tree files for raspberry pi into /boot
     /bin/cp /usr/lib/linux-image-*/bcm27*.dtb /boot
     # copy the overlay files into /boot/overlays
     /bin/cp -a /usr/lib/linux-image-*/overlays /boot
    ```
* for the mainline kernel
     
    ```bash 
     # copy the device tree files for raspberry pi into /boot
     cd /usr/lib/linux-image-*/
     /bin/cp bcm2836-rpi-2-b.dtb   /boot/bcm2709-rpi-2-b.dtb
     /bin/cp bcm2837-rpi-3-b.dtb   /boot/bcm2710-rpi-3-b.dtb
     /bin/cp bcm2837-rpi-3-b-plus.dtb   /boot/bcm2710-rpi-3-b-plus.dtb
     # note: kernel.org doesn't yet have dts file for rpi-4; the rpi-4 is just released
     cd -
     ```

Important: uncomment any `device_tree=` option in /boot/config.txt.

Then reboot and linux with xenomai should be up and running for any raspberry pi 2 or 3 board!! 




Appendix B:  Hacks
-------------------

This mainline kernel installation is not correct. The gpio pins have the wrong numbers.
We try to bring stuff from the rpi kernel to improve, however not success full so far, but we note down these trials.

Reason that these trials don't work because the defconfig and device tree files are really specificly adapted to the adaptions to the  rasbpian kernel.

So my conclusion: stick to mainline defconfig and device tree files.
Means that for the rpi4 we cannot yet build a mainline kernel yet, because the mainline does not yet have a device tree description for that hardware board because it is just released.

### Hack 1: use defconfig from rpi kernel  on mainline kernel 


Change in build: 

```bash 
export KERNEL_DEFCONFIG=bcm2709_defconfig

#get enhanced defconfig bcm2709_defconfig from rpi kernel  
export BASE_URL_DEFCONFIG=https://raw.githubusercontent.com/raspberrypi/linux/rpi-4.19.y/arch/arm/configs
wget -O linux/arch/arm/configs/$KERNEL_DEFCONFIG $BASE_URL_DEFCONFIG/$KERNEL_DEFCONFIG
# or
#export BASE_URL_DEFCONFIG=https://github.com/raspberrypi/linux/branches/rpi-4.19.y/arch/arm/configs/ 
#svn export $BASE_URL_DEFCONFIG/$KERNEL_DEFCONFIG linux/arch/arm/configs/
```

**Conclusion:** doesn't work: get waiting on root device on boot! (kernel does boot but something wrong in mounting root device)  


### Hack 2: use dt files from rpi kernel on mainline kernel 
 
Change in build:
    
```bash    
# get enhanced device tree file from rpi kernel
mv linux/arch/arm/boot/dts .
svn export https://github.com/raspberrypi/linux/branches/rpi-4.19.y/arch/arm/boot/dts linux/arch/arm/boot/dts    
   
mv linux/include/dt-bindings .
svn export https://github.com/raspberrypi/linux/branches/rpi-4.19.y/include/dt-bindings  linux/include/dt-bindings

#IMPORTANT: disable overlays     
#PROBLEM: with compiling overlays
#  $ time make -j 1 KBUILD_DEBARCH=armhf dtbs
#  ....         
#  make[2]: *** No rule to make target `arch/arm/boot/dts/overlays/adau1977-adc.dtbo', needed by `__build'.  Stop.
#  make[1]: *** [arch/arm/boot/dts/overlays] Error 2
#  make: *** [dtbs] Error 2
##  => think .dtbo is  specific for overlays in rpi kernel
#FIX: disable overlays in makefile
#https://www.raspberrypi.org/documentation/configuration/device-tree.md
# says  these optional elements are called "overlays"
# optional so we can skip it!!
# in in arch/arm/boot/dts/Makefile
# uncomment : 
# subdir-y    := overlays
# => no building overlays which are somehow problematic
```
  
  
**Conclusion:** doesn't work: get rainbow screen on boot! (kernel doesn't boot)  
 
 Reason maybe that: 
 
    The current implementation is not a pure Device Tree system – there is still
    board support code that creates some platform devices – but the external 
    interfaces (I2C, I2S, SPI), and the audio devices that use them, must now be 
    instantiated using a Device Tree Blob (DTB) passed to the kernel by the 
    loader (start.elf).
    
 src: https://www.raspberrypi.org/documentation/configuration/device-tree.md

