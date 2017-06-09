
Gpiotest 
==========


Xenomai comes with test programs where one of them is gpiotest.
The test programs make use of the smokey test library. Using this library
gives the programs special command line options mentioned below.




smokey test library for xenomai
--------------------------------

smokey is a test library for xenomai

All test xenomai test programs use this library, and all these programs can use
the smokey options documented below!
Note to really run a test you NEED the --run=X option!!
See the examples below.


http://www.xenomai.org/documentation/xenomai-3/html/xeno3prm/group__smokey.html
  
  It is adviced to run the program with the options :

       –verbose=1 –keep-going


   src: http://www.xenomai.org/documentation/xenomai-3/html/xeno3prm/group__smokey.html

        Pre-defined Smokey options

        Any program linked against the Smokey API implicitly recognizes the following options:

            –list 
            
                 dumps the list of tests implemented in the program to stdout. The information given includes the
                 description strings provided in the plugin declarators (smokey_test_plugin()). The position and
                 symbolic name of each test is also issued, which may be used in id specifications with the –run option
                 (see below).

            Note
                Test positions may vary depending on changes to the host program like adding or removing other tests, the symbolic name however is stable and identifies each test uniquely.

            –run[=<id[,id...]>] 
            
                  selects the tests to be run, determining the active test list among the overall set of tests detected
                  in the host program. The test driver code (e.g. implementing a test harness program on top of Smokey)
                  may then iterate over the smokey_test_list for accessing each active test individually, in the
                  enumeration order specified by the user (Use for_each_smokey_test() for that).

                If no argument is passed to –run, Smokey assumes that all tests detected in the current program should be
                picked, filling smokey_test_list with tests by increasing position order.

                Otherwise, id may be a test position, a symbolic name, or a range thereof delimited by a dash character. A
                symbolic name may be matched using a glob(3) type regular expression.

                id specification may be:
                    0-9, picks tests #0 to #9
                    -3, picks tests #0 to #3
                    5-, picks tests #5 to the highest possible test position
                    2-0, picks tests #2 to #0, in decreasing order
                    foo, picks test foo only
                    0,1,foo- picks tests #0, #1, and any test from foo up to the last test defined
                    fo* picks any test with a name starting by "fo"
                
            –exclude=<id[,id...]> excludes the given tests from the test list. The format of the argument is identical to the one accepted by the –run option.
            
            –keep-going 
                
                sets the boolean flag smokey_keep_going to a non-zero value, indicating to the test driver that
                receiving a failure code from a smoke test should not abort the test loop. This flag is not otherwise
                interpreted by the Smokey API.
            
            –verbose[=level] 
            
                sets the integer smokey_verbose_mode to a non-zero value, which should be interpreted by all parties as
                the desired verbosity level (defaults to 1).
            
            –vm 
            
                gives a hint to the test code, about running in a virtual environment, such as KVM. When passed, the
                boolean smokey_on_vm is set. Each test may act upon this setting, such as skipping time-dependent
                checks that may fail due to any slowdown induced by the virtualization.
  


gpiotest : smokey test programm for rtdm drivers for gpio on rpi for xenomai
-------------------------------------------------------------------

  There is a test program in xenomai : gpiotest 
  
  You have to run this program with just 1 argument and that is the   string "<devfile_gpio_pin_y>".
  However you can also give it the smokey library options mentioned above.

  Note: the write test  writes value=1. That value is fixed in the source code and you can't change it. 

  PS.
      You can also look at its source to understand it, the file is  : gpiotest.c
      In the subfolder mygpiotest/ we put a copy of this sourcefile called
      mygpiotest.c ("my" to distinguish the program from the original).
     



Example run gpiotest
====================

help info about which tests are available (smokey help options): 
   $ sudo /usr/xenomai/bin/arm-linux-gnueabihf-gpiotest  device=/dev/rtdm/pinctrl-bcm2835/gpio2  --verbose=2 --list
   #0   interrupt
           Wait for interrupts from a GPIO pin.
           device=<device-path>
           rigger={edge[-rising/falling/both], level[-low/high]}
           select, wait on select(2).
   #1   read_value
           Read GPIO value.
           device=<device-path>.
   #2   write_value
           Write GPIO value.
           device=<device-path>.


wait interrupt :
   sudo /usr/xenomai/bin/arm-linux-gnueabihf-gpiotest device=/dev/rtdm/pinctrl-bcm2835/gpio2 –-verbose=1 –-keep-going --run=0

write value:
    sudo /usr/xenomai/bin/arm-linux-gnueabihf-gpiotest device=/dev/rtdm/pinctrl-bcm2835/gpio2 –-verbose=1 –-keep-going --run=2

read value: 
    sudo /usr/xenomai/bin/arm-linux-gnueabihf-gpiotest  device=/dev/rtdm/pinctrl-bcm2835/gpio17  –-keep-going --run=1 --verbose=2
    value=1
    read_value OK



run example 
-------------

you need to add xenomai library to dynamic link path to run the test program :

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/xenomai/lib/

then you can run a test :

    gpiotest device=/dev/rtdm/pinctrl-bcm2835/gpio23 trigger=edge-both –-verbose=1 –-keep-going --run=0

or 

    gpiotest device=/dev/rtdm/pinctrl-bcm2835/gpio23 trigger=edge-both level=high –-verbose=1 –-keep-going --run=0

note: you can see interrupt at gpio pin 23 is enable by looking at :

    root@raspberrypi:~/test# cat /proc/xenomai/irq
      IRQ         CPU0        CPU1        CPU2        CPU3
       97:        6015        4436        1297        2886         [timer/0]
      503:           0           0           0           0         gpio23
     1033:           0           0           0           0         [sync]
     1034:           1           1           1           0         [timer-ipi]
     1035:           0           0           1           0         [reschedule]
     1036:           0           0           0           0         [virtual]
     1040:           1           0          17           0         [virtual]




