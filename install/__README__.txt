



/etc/profile.d/xenomai.sh
-------------------------

  this login script 
   - prints the pi type and its current ip address 

   - fixes that the right /usr/xenomai folder is used
       /usr/xenomai1 for the pi 1 and pi zero
       /usr/xenomai2 for the pi 2 and pi 3

  note: we assume you setup the /boot partition such that :
       /boot/kernel    is the a kernel image for the pi1 (or pi zero) with xenomai 3
       /boot/kernel7   is the a kernel image for the pi2 (or pi3) with xenomai 3


installation
------------

  cp etc_profile.d_xenomai.sh /etc/profile.d/xenomai.sh




 
