export LD_LIBRARY_PATH=:/usr/xenomai/lib/:/usr/xenomai/lib/
modprobe xeno_gpio
export PATH=$PATH:/usr/xenomai/bin/
gpiotest device=/dev/rtdm/pinctrl-bcm2835/gpio23 trigger=level-low --keep-going --run=0 --verbose=2

