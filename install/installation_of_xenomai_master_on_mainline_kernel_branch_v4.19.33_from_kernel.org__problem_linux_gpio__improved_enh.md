[TOC]

2019 juli 2

Installation of xenomai master branch on mainline kernel branch v4.19.y from kernel.org  
============================================



### Versions

```bash
export XENOMAI_BRANCH=master
# after cloning use the first 7 letters of the hash as part of version
export XENOMAI_VERSION=master-4b3a3ab
export XENOMAI_GIT_REPO=https://gitlab.denx.de/Xenomai/xenomai.git

# after cloning the kernel source you will get the real kernel
# version with 'make kernelversion'
export KERNEL_VERSION=4.19.33  
export KERNEL_BRANCH=v${KERNEL_VERSION}
export KERNEL_GIT_REPO=https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
    
# for which defconfig to use see: 
#    https://github.com/raspberrypi/linux/wiki/Upstreaming
export KERNEL_DEFCONFIG=multi_v7_defconfig
# when kernel is build it adds the kernel version as suffix
# but you can add an extra local version
export KERNEL_LOCALVERSION="-xeno-$XENOMAI_VERSION"
# note: absence of rpi label in kernel name means its is build
#       from the kernel.org source (mainline)
export KERNEL_FULLVERSION=${KERNEL_VERSION}${KERNEL_LOCALVERSION}
    
export PACKAGENAME=raspberrypi-kernel-image
export PACKAGEVERSION=${KERNEL_FULLVERSION}
export PACKAGEFILE=${PACKAGENAME}-${PACKAGEVERSION}_armhf.deb
    
export IPIPE_PATCH=ipipe-core-4.19.33-arm-2.patch 
export IPIPE_REPO_URL=https://www.xenomai.org/downloads/ipipe/v4.x/arm/
    
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
```
    
#### Note down the specific version details for future rebuilds

```bash    
cd linux
make kernelversion 
# output: 4.19.33 

git rev-parse HEAD
# output: 4b3a3ab00fa7a951eb1d7568c71855e75fd5af85
cd -  
    
cd xenomai
git rev-parse HEAD
# output: 2692220a5d55a5f36ebaee3bfde5e926f168447c
cd -
```

Update the kernel version to the real ke

### Configuring
    
Patch the kernel with the ipipe patch
    
```bash
xenomai/scripts/prepare-kernel.sh  --linux=linux/  --arch=$ARCH  --ipipe=$IPIPE_PATCH 
```
 
 Configure the kernel
 
```bash    
cd linux
make $KERNEL_DEFCONFIG
```
    
 below doesn't work, why??
   -> dts  specific for kernel
   ->   
    
```bash    
# get enhanced device tree file from rpi kernel
mv linux/arch/arm/boot/dts .
svn export https://github.com/raspberrypi/linux/branches/rpi-4.19.y/arch/arm/boot/dts linux/arch/arm/boot/dts    
   
mv linux/include/dt-bindings .
svn export https://github.com/raspberrypi/linux/branches/rpi-4.19.y/include/dt-bindings  linux/include/dt-bindings

    
#get enhanced defconfig bcm2709_defconfig from rpi kernel
wget https://raw.githubusercontent.com/raspberrypi/linux/rpi-4.19.y/arch/arm/configs/bcm2709_defconfig
cp bcm2709_defconfig linux/arch/arm/configs/
# or
#svn export https://github.com/raspberrypi/linux/branches/rpi-4.19.y/arch/arm/configs/bcm2709_defconfig linux/arch/arm/configs/

    
# problem with compiling overlays
#https://www.raspberrypi.org/documentation/configuration/device-tree.md
# says  these optional elements are called "overlays"
# optional so we can skip it!!
# in in arch/arm/boot/dts/Makefile
# uncomment : 
# subdir-y    := overlays
# => no building overlays which are somehow problematic
    
cd linux
export KERNEL_DEFCONFIG=bcm2709_defconfig
make $KERNEL_DEFCONFIG
```
    
 also do with    
 
```bash   
wget https://raw.githubusercontent.com/raspberrypi/linux/rpi-4.19.y/arch/arm/configs/bcm2711_defconfig
cp bcm2711_defconfig linux/arch/arm/configs/
cd linux
export KERNEL_DEFCONFIG=bcm2711_defconfig
make $KERNEL_DEFCONFIG
=> for rpi 
=> install as kernel7l.img  
```    

    

    
    
Start menuconfig 
 
```bash    
make menuconfig
```
 
and choose the following options: 

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

```bash
scripts/config --set-str LOCALVERSION "${KERNEL_LOCALVERSION}"  \
   -d CPU_FREQ -d CPU_IDLE -d CMA -d COMPACTION -d KGDB -d CONFIG_BLK_DEV_INITRD\
   -e ARM_APPENDED_DTB -e ARM_ATAG_DTB_COMPAT -e ARM_ATAG_DTB_COMPAT_CMDLINE_EXTEND \
   -e XENO_DRIVERS_GPIO -m XENO_DRIVERS_GPIO_BCM2835
```
    
Then run 'make olddefconfig' to fix all relations within config
because scripts/config doesn't check the validity of the .config file.

```bash
make olddefconfig
```

Finally you can verify the set options with the command

```bash
for OPTION in LOCALVERSION  CPU_FREQ CPU_IDLE CMA COMPACTION KGDB \
  ARM_APPENDED_DTB ARM_ATAG_DTB_COMPAT ARM_ATAG_DTB_COMPAT_CMDLINE_EXTEND \
  XENO_DRIVERS_GPIO XENO_DRIVERS_GPIO_BCM2835 MIGRATION
do 
   echo "${OPTION}: $(scripts/config -s ${OPTION})"   
done   
```

We also check status of MIGRATION because it is needed to decrease latency. The config option MIGRATION is disabled indirectly by disabling CMA and COMPACTION. Note that if you enable either of them then 'make menuconfig' gives the warning:

    *** WARNING! Page migration (CONFIG_MIGRATION) may increase latency. ***    
 

We finally get:

```bash
$ for OPTION in LOCALVERSION  CPU_FREQ CPU_IDLE CMA COMPACTION KGDB \
>   ARM_APPENDED_DTB ARM_ATAG_DTB_COMPAT ARM_ATAG_DTB_COMPAT_CMDLINE_EXTEND \
>   XENO_DRIVERS_GPIO XENO_DRIVERS_GPIO_BCM2835 MIGRATION
> do
>   echo "${OPTION}: $(scripts/config -s ${OPTION})"
> done
LOCALVERSION: -xeno-master-4b3a3ab
CPU_FREQ: n
CPU_IDLE: n
CMA: n
COMPACTION: n
KGDB: n
ARM_APPENDED_DTB: y
ARM_ATAG_DTB_COMPAT: y
ARM_ATAG_DTB_COMPAT_CMDLINE_EXTEND: y
XENO_DRIVERS_GPIO: y
XENO_DRIVERS_GPIO_BCM2835: m
MIGRATION: undef
```

Make a distribution dir where we store our build packages
 
```bash    
cd ..
mkdir dist/
```

### Build kernel into a debian package
 
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

 
### Build xenomai user-space libraries and tools
 

For Configure details see : https://xenomai.org/installing-xenomai-3-x/#_configuring

```bash
export DESTDIR=$PWD/xenomai-build 
    
 cd xenomai
 ./scripts/bootstrap 
 ./configure CFLAGS="-march=armv7-a  -mfloat-abi=hard -mfpu=neon -ffast-math" --host=arm-linux-gnueabihf --enable-smp --with-core=cobalt
	
 make install
	
 cd ..
 tar -C $DESTDIR -czvf dist/xenomai-binaries.tgz .
 # cleanup with:  
 rm -rf $DESTDIR
```
   
### Installing manually

Now in the dist/ directory we have
 
```bash 
$ ls -1 dist
linux-image-${KERNEL_FULLVERSION}*_armhf.deb
xenomai-binaries.tgz
``` 

#### Installing the distribution files on the mounted sdcard of the Raspberry pi 
    
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
 /bin/cp -a ${MOUNTPOINT}/usr/lib/linux-image-*/bcm* /boot
 export OVERLAYSDIR=$(printf ${MOUNTPOINT}/usr/lib/linux-image-*/overlays)
 if [[ -d ${OVERLAYSDIR} ]]; then /bin/cp -a ${OVERLAYSDIR}  /boot/ ; fi   
 
 # install xenomai user-space libraries and tools
 tar -C ${MOUNTPOINT} -xzvf xenomai-binaries.tgz
 
 # umount
 umount ${MOUNTPOINT}/boot ${MOUNTPOINT}/
```

#### Installing the distribution files on the Raspberry pi itself


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
 /bin/cp -a /usr/lib/linux-image-*/bcm* /boot
 export OVERLAYSDIR=$(printf /usr/lib/linux-image-*/overlays)
 if [[ -d ${OVERLAYSDIR} ]]; then /bin/cp -a ${OVERLAYSDIR}  /boot/ ; fi
  
 # install xenomai user-space libraries and tools
 tar -C / -xzvf xenomai-binaries.tgz
```      

#### Setting device tree files

Finally you have to set the right device tree description file for your specific raspberry pi hardware board so that the linux kernel is informed of that hardware.


All raspberry pi device tree files are build into the linux-image-*.deb package inside the /usr/lib/linux-image-.. dir. We can list them with the following command:

```bash
$ dpkg -c linux-image-*.deb |egrep '/bcm.*.dtb'
```

Note that the raspbian kernel and the mainline kernel use different naming conventions for the device tree files for the different raspberry pi hardware boards:
 
- for rpi kernel: `bcm27*.dtb` and `overlays/*`  
- for vanilla(kernel.org) kernel: `bcm283*.dtb` (no overlays)<br>

The device tree file must be located in /boot to let the bootcode load it from the FAT /boot partition to give it to kernel. Therefore we have to copy all the device tree files from the place where the kernel installed them to /boot:

* for the rpi kernel

    ```bash
     # copy the device tree files for raspberry pi into /boot
     /bin/cp /usr/lib/linux-image-*/bcm27*.dtb /boot
     # copy the overlay files into /boot/overlays
     /bin/cp -a /usr/lib/linux-image-*/overlays /boot
    ```
* for the vanilla(kernel.org) kernel
     
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



##### Remark about device file naming and auto loading
    
With a text editor you can then set the device tree to boot your specific raspberry pi by adding that pi's device tree file to the end of the file in /boot/config.txt. 

Eg. for the raspberry pi 3b the following line should be added:
 
     device_tree=bcm2837-rpi-3-b.dtb   
  
But the bootcode used by the raspberry can also automatically load the right device tree description file without specify it in /boot/config.txt as long as the device tree files follow the naming convention of the rpi kernel.
That's why we copied for mainline kernel the device tree files to the /boot directory with the rpi name and didn't set any `device_tree=` option in /boot/config.txt.

For more information see [`../notes/device_tree_files_automatic_loading_on_raspbian_image.txt`](../notes/device_tree_files_automatic_loading_on_raspbian_image.txt). 


### Automatic installation

#### Make a single debian package for easy installation

Instead of installing the distribution files as described in the step above and doing the device tree setup manually, it would be much easier if all of this would be done by a single debian package we just have to download and install. In this section we are going to build such a debian package.
  
The approach we use is to take the build kernel package and modify it so that it install also the xenomai libraries/tools and that it installs the kernel and the device tree description files for the different hardware type of raspberry pi boards into /boot. It also copies the original /boot folder to /boot_<timestamp> folder so that we always can easily recover in case the installation may fail.


```bash
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
# by copying the device tree files for raspberry pi into /boot
# for rpi kernel use the raspbian specific bcm27 device tree files
/bin/cp /usr/lib/linux-image-\${kernelversion}/bcm27*.dtb /boot
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
 
#### Automatic installation

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
 
     

