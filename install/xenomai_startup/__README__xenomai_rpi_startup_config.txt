Xenomai startup and login scripts 
=================================

/etc/rc.local
-------------
  
 this startup script gets executed once when the pi boots
   - prints the pi's current  ip address 
   - prints the pi's model 
   - fixes that the right /usr/xenomai folder is used
       /usr/xenomai1 for the pi 1 and pi zero
       /usr/xenomai2 for the pi 2 and pi 3

     note: we assume you setup the /boot partition such that :
       /boot/kernel    is the a kernel image for the pi1 (or pi zero) with xenomai 3
       /boot/kernel7   is the a kernel image for the pi2 (or pi3) with xenomai 3

   - loads the xeno_gpio kernel model
   
   - set some initial values for some gpio pins
   
   - run dyndns script to register ip for mac address to dyndns server at boot
     see: https://github.com/harcokuppens/dyndns 

/etc/profile.d/info.sh
-------------------------

  this login script 
   - prints the pi type and its current ip address 

/etc/profile.d/xenomai.sh
-------------------------

  this login script 
   - prints the current xenomai version
   - fixes paths for xenomai (PATH and LD_LIBRARY_PATH)
   - adds several convenient  aliases and functions to the bash environment :

           alias xeno-stat="cat /proc/xenomai/sched/stat"
           alias xeno-threads="cat /proc/xenomai/sched/threads"
           alias xeno-rt-threads="cat /proc/xenomai/sched/rt/threads"
           alias xeno-interrupts="cat /proc/xenomai/irq"
           alias xeno-version=/usr/xenomai/sbin/version

           # convenience methods to switch between cobalt and mercury
           # note: mercury is running on xenomai-cobalt patched kernel but without rt_preempt patch
           #        => running on mercury on this image is thus just running on linux but then as high prio (fifo) thread within linux (independent cobalt)
           #        => just to show portability xenomai programs from cobalt to mercury
           #        => important: most rtdm drivers are only available for cobalt and are not available yet for mercury  (eg. gpio rtdm driver)
           xeno-switch-cobalt() {
             ...
           }
           xeno-switch-mercury() {
             ...
           }     


installation
------------

  cp etc_profile.d_xenomai.sh /etc/profile.d/xenomai.sh
  cp etc_profile.d_info.sh /etc/profile.d/info.sh
  cp etc_rc.local /etc/rc.local



dynamic update ip at boot to server and look it up at a webserver
-----------------------------------------------------------------

at https://github.com/harcokuppens/dyndns you can download a dynamic dns
client script to run at boot of the raspberry pi, and a php server script
where you can lookup the ip of a raspberry pi.

For details see: https://github.com/harcokuppens/dyndns

  wget -O /usr/local/bin/update_ip_at_dyndns_server https://raw.githubusercontent.com/harcokuppens/dyndns/master/update_ip_at_dyndns_server  
  chmod a+x /usr/local/bin/update_ip_at_dyndns_server

 
