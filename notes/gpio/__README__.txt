overview
--------

 => see ../../__OVERVIEW__.txt




gpio
----
   http://www.wikiwand.com/en/General-purpose_input/output
     
   what they are:
        
     General-purpose input/output (GPIO) is a generic pin on an integrated circuit whose behavior—including whether it
     is an input or output pin—is controllable by the user at run time.
    
   why they exist: 
        
     GPIO pins have no predefined purpose, and go unused by default. The idea is that sometimes a system integrator who
     is building a full system might need a handful of additional digital control lines—and having these available from
     a chip avoids having to arrange additional circuitry to provide them.
     
   usage:
      Manufacturers use GPIOs in:

       * Devices with pin scarcity: 
             integrated circuits such as system-on-a-chip, embedded and custom hardware, and 
             programmable logic devices (for example, FPGAs)
       * Multifunction chips: 
             power managers, audio codecs, and video cards
       * Embedded applications 
           (Arduino, BeagleBone, PSoC kits, Raspberry Pi, etc.) use GPIO for reading from various environmental sensors
           (IR, video, temperature, 3-axis orientation, and acceleration), and for writing output to DC motors (via
           PWM), audio, LCD displays, or LEDs for status. GPIO capabilities may include:

  * GPIO pins can be configured to be input or output
  * GPIO pins can be enabled/disabled
  * Input values are readable (typically high=1, low=0)
  * Output values are writable/readable
  * Input values can often be used as IRQs (typically for wakeup events)
  
  
  GPIO peripherals vary widely. In some cases, they are simple—a group of pins that can switch as a group to either
  input or output. In others, each pin can be set up to accept or source different logic voltages, with configurable
  drive strengths and pull ups/downs. Input and output voltages are typically—though not always—limited to the supply
  voltage of the device with the GPIOs, and may be damaged by greater voltages.

  A GPIO pin's state may be exposed to the software developer through one of a number of different interfaces, such as
  a memory mapped peripheral, or through dedicated IO port instructions. Some GPIOs have 5 V tolerant inputs: even when
  the device has a low supply voltage (such as 2 V), the device can accept 5 V without damage.
     
  
  more info:
  
     raspberry pi :
       https://www.raspberrypi.org/documentation/usage/gpio/   
     
     pinout rpi 
       http://pinout.xyz/
      
     
     http://luketopia.net/2013/07/28/raspberry-pi-gpio-via-the-shell/
     https://projects.drogon.net/raspberry-pi/wiringpi/
     https://sites.google.com/site/semilleroadt/raspberry-pi-tutorials/gpio
   
   
  
rasperry pi   
------------
   
   libraries :
      drivers/gpio/gpiolib.c              => linux gpio library
      drivers/xenomai/gpio/gpio-core.c    => xenomai realtime gpio library (wrap in rtdm driver)  => uses gpiolib.c to setup stuff in none-realtime
     
   drivers:
             
   
   
   



short overview
=================

https://lwn.net/Articles/532714/


  GPIOs must be allocated before use, though the current implementation does not enforce this requirement. The basic
  allocation function is:

     int gpio_request(unsigned int gpio, const char *label);
 
 
  Some GPIOs are used for output, others for input. A suitably-wired GPIO can be used in either mode, though only one
  direction is active at any given time. Kernel code must inform the GPIO core of how a line is to be used; that is
  done with these functions:

      int gpio_direction_input(unsigned int gpio);
      int gpio_direction_output(unsigned int gpio, int value);
  
  In either case, gpio is the GPIO number. In the output case, the value of the GPIO (zero or one) must also be
  specified; the GPIO will be set accordingly as part of the call
 
 
 


http://stratifylabs.co/embedded%20design%20tips/2013/10/21/Tips-Understanding-Microcontroller-Pin-Input-Output-Modes/
flags  FLAG_OPEN_DRAIN FLAG_OPEN_SOURCE  are only used for  outputs!!  (not input)


General purpose input/output (GPIO) pins on microcontrollers have various modes for both input and output. 

Input modes 
   may include pull-up or pull-down resistors, hysteresis, or some combination. 

Output modes can be 
   push-pull, 
   high-drive 
   or open-drain.
 

 
 
Official docs from linux kernel (gpiolib)
================================ 

https://www.kernel.org/doc/Documentation/gpio/

    https://www.kernel.org/doc/Documentation/gpio/gpio.txt
     => legacy: https://www.kernel.org/doc/Documentation/gpio/gpio-legacy.txt
    
    
    The exact capabilities of GPIOs vary between systems. Common options:

      - Output values are writable (high=1, low=0). Some chips also have
        options about how that value is driven, so that for example only one
        value might be driven, supporting "wire-OR" and similar schemes for the
        other value (notably, "open drain" signaling).

      - Input values are likewise readable (1, 0). Some chips support readback
        of pins configured as "output", which is very useful in such "wire-OR"
        cases (to support bidirectional signaling). GPIO controllers may have
        input de-glitch/debounce logic, sometimes with software controls.

      - Inputs can often be used as IRQ signals, often edge triggered but
        sometimes level triggered. Such IRQs may be configurable as system
        wakeup events, to wake the system from a low power state.

      - Usually a GPIO will be configurable as either input or output, as needed
        by different product boards; single direction ones exist too.

      - Most GPIOs can be accessed while holding spinlocks, but those accessed
        through a serial bus normally can't. Some systems support both types.
    
    
  https://www.kernel.org/doc/Documentation/gpio/gpio-legacy.txt
       
     note: this document is more extensive then gpio.txt  
        

    Claiming and Releasing GPIOs
    ----------------------------     
       
       Some platforms allow some or all GPIO signals to be routed to different pins.
       Similarly, other aspects of the GPIO or pin may need to be configured, such as
       pullup/pulldown. Platform software should arrange that any such details are
       configured prior to gpio_request() being called for those GPIOs, e.g. using
       the pinctrl subsystem's mapping table, so that GPIO users need not be aware
       of these details.
       
       => so pullup/pulldown should be set using  pinctrl library instead!!
           => see: https://www.kernel.org/doc/Documentation/pinctrl.txt

          this is said also in section later in document :
     
            What do these conventions omit?
            ===============================
              One of the biggest things these conventions omit is pin multiplexing, since
              this is highly chip-specific and nonportable
    
              Related to multiplexing is configuration and enabling of the pullups or
              pulldowns integrated on some platforms.  Not all platforms support them,
              or support them in the same way;
      
    
      => so cannot set pullup nor pulldown with gpiolib
         but can with lower lever pinctrl
         however much easier none-realtime with wiringpi library :
         
          see : gpio/raspberry_pi__gpio_userspace_library_wiringpi.txt
       
    
    https://www.kernel.org/doc/Documentation/gpio/consumer.txt
      describes usage gpiolib
      
      
    https://www.kernel.org/doc/Documentation/gpio/board.txt
      GPIOs can easily be mapped to devices and functions in the device tree. 
      
    https://www.kernel.org/doc/Documentation/gpio/sysfs.txt
       GPIO Sysfs Interface for Userspace
       
         
        

    https://www.kernel.org/doc/Documentation/gpio/driver.txt

     => for input 
           => doesn't say anythig about pull up/down input
              because that not supported by gpiolib  => use wiringpi lib instead to setup
     => for output describes : software-controlled open drain/source works.

    https://www.kernel.org/doc/Documentation/gpio/drivers-on-gpio.txt
     Subsystem drivers using GPIO
 
 
 
 

 https://www.kernel.org/doc/Documentation/pinctrl.txt

   Configuration of pins, pads, fingers (etc), such as software-controlled
     biasing and driving mode specific pins, such as pull-up/down, open drain,
     load capacitance etc.
    
    
     You may be able to connect an input pin to VDD or GND
     using a certain resistor value - pull up and pull down - so that the pin has a
     stable value when nothing is driving the rail it is connected to, or when it's
     unconnected.


     Interaction with the GPIO subsystem
     ===================================

     The GPIO drivers may want to perform operations of various types on the same
     physical pins that are also registered as pin controller pins.


 




 
 
 

    

