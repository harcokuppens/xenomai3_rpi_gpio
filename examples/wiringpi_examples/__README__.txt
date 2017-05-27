pin layout

  http://www.jameco.com/Jameco/workshop/circuitnotes/raspberry_pi_circuit_note_fig2a.jpg  


   https://pinout.xyz/#


   https://www.raspberrypi.org/documentation/hardware/raspberrypi/gpio/README.md


     
    GPIO   
       There are 3 GPIO banks on BCM2835.

       Each of the 3 banks has its own VDD input pin. On Raspberry Pi, all GPIO banks are supplied from 3.3V. 
       Connection of a GPIO to a voltage higher than 3.3V will likely destroy the GPIO block within the SoC

      => IMPORTANT: don't  3.3 V 



     Interrupts

        Each GPIO pin, when configured as a general-purpose input, can be configured as an interrupt source to the ARM. Several interrupt generation sources are configurable:

            Level-sensitive (high/low)
            Rising/falling edge
            Asynchronous rising/falling edge

        Level interrupts maintain the interrupt status until the level has been cleared by system software (e.g. by servicing the attached peripheral generating the interrupt).

        The normal rising/falling edge detection has a small amount of synchronisation built into the detection. At the system clock frequency, the pin is sampled with the criteria for generation of an interrupt being a stable transition within a 3-cycle window, i.e. a record of "1 0 0" or "0 1 1". Asynchronous detection bypasses this synchronisation to enable the detection of very narrow events.



blink
------

  https://github.com/WiringPi/WiringPi/blob/master/examples/blink.c

make blink
./blink

in other terminal see result with :

   watch -n0.2 gpio read 0

isr
---
  https://github.com/WiringPi/WiringPi/blob/master/examples/isr.c


install manpage gpio utility
----------------------------

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
