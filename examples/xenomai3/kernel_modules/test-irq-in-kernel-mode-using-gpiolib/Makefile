# Kbuild file
KERNELBUILDDIR ?= /var/tmp/rpi2/linux
#KERNELBUILDDIR ?= /home/rpi_xenomaiv3.0.4/linux
  
#XENO_DIR = /home/rpi_xenomaiv3.0.4/xenomai-3-3.0.4
#EXTRA_CFLAGS := -I${XENO_DIR}/include/cobalt/kernel \
#-I${XENO_DIR}/include/cobalt -I${XENO_DIR}/kernel/cobalt/include \
#-I${XENO_DIR}/kernel/cobalt/arch/arm/include -I${XENO_DIR}/include   
  
# To build modules outside of the kernel tree, we run "make"
# in the kernel source tree; the Makefile these then includes this
# Makefile once again.
# This conditional selects whether we are being included from the
# kernel Makefile or not.
ifeq ($(KERNELRELEASE),)

    # Assume the source tree is where the running kernel was built
    # You should set KERNELDIR in the environment if it's elsewhere
    #KERNELDIR ?= /lib/modules/$(shell uname -r)/build
    KERNELDIR ?= $(KERNELBUILDDIR)
    # The current directory is passed to sub-makes as argument
    PWD := $(shell pwd)

modules:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERNELDIR) M=$(PWD) modules


clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

.PHONY: unload load modules modules_install clean

else
    # called from kernel build system: just declare what our modules are
    obj-m := button_toggles_led.o
    #xeno_button_toggles_led-y :=  button_toggles_led.o 
endif
