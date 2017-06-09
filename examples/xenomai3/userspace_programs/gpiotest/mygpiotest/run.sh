#insmod ../xeno_gpio.ko  # loads rtdm driver to construct /dev/rtdm/gpio_X device files for gpio ports in xenomai user space!
modprobe xeno_gpio


#  make circuit which pulls it high!!

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/xenomai/lib/
#./mygpiotest device=/dev/rtdm/pinctrl-bcm2835/gpio23 trigger=level-high --verbose=1 --keep-going --run=0
./mygpiotest device=/dev/rtdm/pinctrl-bcm2835/gpio23 trigger=level-low --verbose=1 --keep-going --run=0
#./mygpiotest device=/dev/rtdm/pinctrl-bcm2835/gpio23 trigger=edge-both,level-high --verbose=1 --keep-going --run=0

