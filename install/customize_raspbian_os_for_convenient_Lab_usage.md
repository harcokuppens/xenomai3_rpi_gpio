Customize Raspbian OS for convenient Lab usage
==============================================


  * [Goal](#goal)
  * [Configure pi with raspi-config](#configure-pi-with-raspi-config)
  * [Install utilities](#install-utilities)
  * [Configure ssh server to allow root login](#configure-ssh-server-to-allow-root-login)
  * [Make Samba server allow mounting root's homedir without passwd](#make-samba-server-allow-mounting-roots-homedir-without-passwd)
  * [Remove authentication from virtual consoles (ctrl-alt-F1..12)](#remove-authentication-from-virtual-consoles-ctrl-alt-f112)
  * [Fixate kernel: prevent it to be updated](#fixate-kernel-prevent-it-to-be-updated)
  * [Add custom Xenomai startup and login scripts](#add-custom-xenomai-startup-and-login-scripts)
  * [Configure wlan](#configure-wlan)

Goal
----

We configure so that we can easily work with the raspberry pi. We do this by configure the rasperry pi so that we can easily
     
  - ssh login as root from another network host  => easy login and make/run programs
  - samba mount /root dir  => easy access in host to files with local mount so that we can use the editor of choice in the host to edit the files
  - and for the virtual consoles you are automatically logged in as root. Meaning when  a monitor, mouse and keyboard is connected to the raspberry pi after a boot you are automatically logged in on the monitor.
   
You can log in with the following credentials:

      user:            pi
      passwd:          raspberry 

      user:            root
      passwd:          pi
      
Note: by default in raspbian is the pi user allowed to execute the sudo command 

Configure pi with raspi-config
------------------------------

	raspi-config   
	 => goto localisation options
	     => setup timezone to europe/amsterdam
	     => setup keyboard to  us-keyboard
	
	# login as user: pi
	# become root user
	sudo su -
	# change password to 'pi'
	passwd
	# type twice 'pi'



            
Install utilities
-----------------

	apt-get update
	
	# mouse support in console:
	apt-get install gpm 
	    
	# gpio tool/libraries:
	apt-get install wiringpi
	
	# editors:
	apt-get install vim
	apt-get install neovim # because vim has no clipboard support builtin so copy pasting mouse doesn't work with vim, but does with neovim 
	apt-get install nano
	apt-get install joe  

	# for time sync: rdate -s time-srv.science.ru.nl    
	apt-get install rdate  

	# version control:
	apt-get install git 
	apt-get install subversion    
   
Configure ssh server to allow root login
----------------------------------------

	sudo systemctl enable ssh.service
	sudo systemctl start ssh.service
	
	edit  /etc/ssh/sshd_config so that :
	
	    UsePAM yes
	    PasswordAuthentication yes
	    PermitRootLogin yes
	    
	
	/etc/init.d/ssh restart

    # remove annoying warning:
	rm /etc/profile.d/sshpwd.sh
 
 

Make Samba server allow mounting root's homedir without passwd
----------------------------------------------

	apt-get install samba
	
	smbpasswd -a root    # note: use 'pi' as password
	# edit  /etc/samba/smb.conf
	# -> at homes section : set  'read only' to 'no'
	
	/etc/init.d/samba restart


Remove authentication from virtual consoles (ctrl-alt-F1..12)
-------------------------------------------

In /lib/systemd/system/getty@.service

    ...
    ExecStart=-/sbin/agetty   --noclear %I $TERM
    ...

add "--autologin root" option :

    ...
    ExecStart=-/sbin/agetty  --autologin root --noclear %I $TERM        
    ...
    

Fixate kernel: prevent it to be updated            
---------------------------------------

To fixate the kernel we must put it on a hold:

     $ apt-mark hold raspberrypi-kernel
     $ apt-mark hold raspberrypi-bootloader   
   
 verify with command :
   
     $ apt-mark showhold
     raspberrypi-bootloader
     raspberrypi-kernel               

Add custom Xenomai startup and login scripts 
---------------------------------------------
  See instructions at [`xenomai_startup/__README__xenomai_rpi_startup_config.txt`](xenomai_startup/__README__xenomai_rpi_startup_config.txt). We mainly setup here:
  
  * automatic loading the xeno_gpio kernel model, 
  * set some initial values for some gpio pins, and 
  * to run a dyndns script to register ip for mac address to dyndns server at boot. In the lab we labeled each raspberry pi with its mac address for both wifi and ethernet. We can then easily lookup the ip of a raspberry pi using only its mac address.
   


Configure wlan
---------------

We don't use eduroam because we need then user credentials to connect to the wifi network. Instead in our lab we put a  wifi access point with password protection so that devices can easily connect to wifi. We also make the SSID of the network hidden, so that it may be a little bit more secure by obscurity.

Use raspi-config to setup wlan network. However after that we need to add the line `scan_ssid=1` to /etc/wpa_supplicant/wpa_supplicant.conf after the ssid line so that the wifi connection to a hidden network works better.

Also see [`../notes/raspberrypi/networking/wifi_pi3.txt`](../notes/raspberrypi/networking/wifi_pi3.txt).

Note that country code must be set for rpi3b+. 
Why is explained by https://www.raspberrypi.org/documentation/configuration/wireless/wireless-cli.md
 
     On the Raspberry Pi 3 Model B+, you will also need to set the country code, 
     so that the 5G networking can choose the correct frequency bands.
 

 
 
