export LD_LIBRARY_PATH=:/usr/xenomai/lib/:/usr/xenomai/lib/
modprobe xeno_gpio
./button_toggles_led
