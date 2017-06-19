
wiringpi
=======

http://wiringpi.com/

WiringPi is a PIN based GPIO access library written in C for the BCM2835 used in
the Raspberry Pi. It’s released under the GNU LGPLv3 license and is usable from
C, C++ and RTB (BASIC) as well as many other languages with suitable wrappers.

WiringPi includes a command-line utility "gpio" which can be used to program and
setup the GPIO pins. You can use this to read and write the pins and even use it
to control them from shell scripts.

The documentation of the gpio utility is  at:
  
    http://wiringpi.com/the-gpio-utility/


credits 
-------

 example programs in this folder are based on : 

  https://github.com/WiringPi/WiringPi/blob/master/examples/blink.c
  https://github.com/WiringPi/WiringPi/blob/master/examples/isr.c


install manpage "gpio" utility on pi
----------------------------------
  instead of the manpage you can also lookup the documentation at : http://wiringpi.com/the-gpio-utility/

    wget https://raw.githubusercontent.com/ngs/wiringPi/master/gpio/gpio.1
    echo 'export LC_ALL="en_US.UTF-8"' >>  ~/.bashrc
    . ~/.bashrc
    # locate ls.1   => to find location of manpages
    #  /usr/local/share/man/man1/
    cp gpio.1 /usr/share/man/man1/
    man mandb

 done
 try out :
 
    man gpio
