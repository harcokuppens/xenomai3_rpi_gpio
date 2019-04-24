

prebuild downloadable xenomai 3 images:
 * sdcard image for pi1/pi-zero/p2/pi3 http://www.cs.ru.nl/lab/xenomai/raspberrypi.html
 * virtualbox image (virtual intel pc) : http://www.cs.ru.nl/lab/xenomai/virtualbox.html


build & installation
--------------------

build & installation descriptions for pi1/pi-zero and pi2/pi3  
- install_xenomai-3-3.0.5_on_rpi1__ipipe_git.txt
- install_xenomai-3-3.0.5_on_rpi2__using_official_ipipe_patch__external_bcm_patch__pinctrl-bcm2835_patchfrom_ipipe_repos.txt

note: this are the exact instructions to build the kernel.img and kernel7.img in the sdcard image 
      downloadable from http://www.cs.ru.nl/lab/xenomai/raspberrypi.html  

xenomai_startup/ folder
-----------------------
   describes changes to your pi sdcard to 
    - boot for both pi1/pi-zero and pi2/pi3
    - print ip number on boot
    - print pi information on boot
    - automatically load xeno_gpio module on boot
    - automatically set right xenomai paths to build xenomai programs

dyndns/ folder
--------------

  describes howto setup your own dynamic dns server for your own pi's on your own network
  so that you can easily retreive the dynamic assigned ip address of a pi using its mac address
  note: describes both php server and changes to your pi sdcard 




__INSTALL_FAILURES__
----------------------
  => not really interesting,
     I just describe here all installations I tried with fail, 
     just to remember I already tried this, and to see what went wrong!
     
     
      
