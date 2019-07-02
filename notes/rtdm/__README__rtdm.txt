


Question:

    I want access the Raspberry Pi 3 peripherals via Xenomai RTDM drivers.

    In this link http://www.cs.ru.nl/lab/xenomai/ is said that that image available
    has the GPIO drivers implemented. I wanted to know if in this image there is
    the drivers for the another peripherals (PWM, I2C, UART, etc.).


Answer:

    When building the kernel we only included support for a rtdm driver for gpio.  However  when configuring the kernel you could activate more rtdm drivers for xenomai.

    You have to rebuild the kernel in the image, but that can be easily done by following the build descriptions I put on:
       http://www.cs.ru.nl/lab/xenomai/raspberrypi.html
    note: I just update the description using markdown to make it easier to read

    When you look in the kernel config a lot seems supported :

        $ make menuconfig

        Xenomai/cobalt  --->
               Drivers  --->
                 [*] Enable legacy pathnames for named RTDM devices 

                    Serial drivers  --->                         
                    Testing drivers  --->                        
                    CAN drivers  --->                            
                    RTnet  --->                                  
                    ANALOGY drivers  --->                        
                    Real-time IPC drivers  --->                  
                    UDD support  --->                            
                    Real-time GPIO drivers  --->                 
                    GPIOPWM support  --->                        
                    Real-time SPI master drivers  --->           

                                                                                                                                                                       
    Curious about what is documented for rtdm drivers I found the following:
    https://gitlab.denx.de/Xenomai/xenomai/wikis/Programming
       https://gitlab.denx.de/Xenomai/xenomai/wikis/Driver_Serial_16550A
       https://gitlab.denx.de/Xenomai/xenomai/wikis/RTnet
       https://gitlab.denx.de/Xenomai/xenomai/wikis/Analogy_General_Presentation

    However not all drivers are documented, so you probably just have to try and google.

    good luck,

    best regards,
    Harco Kuppens




RTDM documentation
--------------------

original paper: http://www.cs.ru.nl/lab/xenomai/RTDM-and-Applications.pdf

https://gitlab.denx.de/Xenomai/xenomai/wikis/Migrating_From_Xenomai_2_To_3
  RTDM interface changes  in xenomai 3
  => contains good documentation


https://gitlab.denx.de/Xenomai/xenomai/wikis/Introducing_Xenomai_3
  
RTDM improvements

  New device description model

      The major changes are aimed at narrowing the gap between the regular
      Linux device driver model and RTDM even more, based on new driver and
      device description models.


      As a result of this, not only the RTDM API is common between the
      single and dual kernel configurations, but both expose devices the
      same way in the regular Linux device namespace.


      This means that RTDM device properties can be inspected via sysfs,
      even in a dual kernel (i.e. Cobalt) configuration.



  API extension


      A wait queue construct very similar to the Linux kernel wait queues
      is available from the driver API. This allows for fine-grained
      synchronization on arbitrary events and conditions.


      The RTDM specification was extended with providing mmap() support
      from drivers, which Xenomai 3 implements.


https://gitlab.denx.de/Xenomai/xenomai/wikis/Start_Here

 the stock kernel drivers can be reused by real-time applications
 with no change (provided they don’t create latency issues due to
 an unfortunate implementation). On the other hand, drivers need
 to be specifically implemented for running over a dual kernel
 configuration, i.e. over the in-kernel RTDM interface.



https://gitlab.denx.de/Xenomai/xenomai/wikis/Porting_To_Xenomai_POSIX
  
  Access to drivers

  In this case the secondary mode switches are due to calls to open, read,
  write, ioctl, socket, connect, sendto, recvfrom, etc…​


  The cure is to rewrite drivers using a Xenomai based driver framework.
  The common drivers skin is RTDM, a set of Xenomai services which offer
  the equivalent of Linux services for writing drivers like character
  devices and socket protocols.


  On top of RTDM, other APIs exist such as Real-time socket CAN, an API
  for writing drivers for the CAN protocol, Comedi/RTDM, an API for
      acquisition cards, RTnet, an implementation of an UDP/IP layer for
      real-time ethernet drivers, USB4RT, an API for USB drivers, and probably
      other such APIs.


      Porting a Linux driver to RTDM is usually not as hard as it seems: the
      RTDM services resemble their Linux equivalents, so any people with Linux
      driver knowledge should be able to port drivers to RTDM. For more
      information on the RTDM framework, see:
      RTDM and Applications


      From an application point of view, using the Xenomai POSIX skin wrapped
      services allows for manipulation of file descriptors provided by the
      RTDM skin as if they were ordinary file descriptors




https://gitlab.denx.de/Xenomai/xenomai/wikis/Programming
   https://gitlab.denx.de/Xenomai/xenomai/wikis/Driver_Serial_16550A
   https://gitlab.denx.de/Xenomai/xenomai/wikis/RTnet
   https://gitlab.denx.de/Xenomai/xenomai/wikis/Analogy_General_Presentation

https://gitlab.denx.de/Xenomai/xenomai/wikis/Building_Applications_For_Xenomai_3
  Compiling a RTDM-based module

http://www.cs.ru.nl/lab/xenomai/api3/group__rtdm.html

