
export LD_LIBRARY_PATH=:/usr/xenomai/lib/:/usr/xenomai/lib/
modprobe xeno_gpio

export PATH=$PATH:/usr/xenomai/bin/
arm-linux-gnueabihf-gpiotest  --list


