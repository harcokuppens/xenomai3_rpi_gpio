# Kbuild file
  
# To build modules outside of the kernel tree, we run "make"
# in the kernel source tree; the Makefile these then includes this
# Makefile once again.
# This conditional selects whether we are being included from the
# kernel Makefile or not.
ifeq ($(KERNELRELEASE),)

    # Assume the source tree is where the running kernel was built
    # You should set KERNELDIR in the environment if it's elsewhere
    #KERNELDIR ?= /lib/modules/$(shell uname -r)/build
    KERNELDIR ?= /root/kernelsource
    # The current directory is passed to sub-makes as argument
    PWD := $(shell pwd)

modules:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERNELDIR) M=$(PWD) modules


clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

.PHONY: unload load modules modules_install clean

else
    # called from kernel build system: just declare what our modules are
    obj-m := xeno_gpio_irq.o
    xeno_gpio_irq-y :=  irq-gpio-rtdm.o 
endif
