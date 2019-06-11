
export LD_LIBRARY_PATH=:/usr/xenomai/lib/:/usr/xenomai/lib/
modprobe xeno_gpio
export PATH=$PATH:/usr/xenomai/bin/

gpiotest device=/dev/rtdm/pinctrl-bcm2835/gpio22 --keep-going --run=2 --verbose=2


