
dyndns
======

The scripts in this folder allow you to easily to retreive an ip address of a raspberry pi running. 

It works as follow:
1. at every login the pi updates its ip address at a php website. It registers its mac-address as key for its current  ip-address.
2. one can then go the php website and supply the pi's mac address after which the website gives back its current ip address.

Using this setup you can have multiple raspberry pi's with exactly the same image but different mac-addresses for which you then
easily can retreive for each pi its ip address by just knowing its mac-address. So you don't need to connect the pi to a monitor anymore,
you can just connect the pi's to the network and start using them.

To limit the usage of the dyndns php-script you can limit its usage within some network domain by setting :

             $network_prefix="131.174";

You can set the limit in both client as server scripts.

installation
-------------

 1. on some webserver install at some webdir the php script:  
 
           index.php

       => at beginning of script  edit
             $network_prefix="131.174";
          to you own local domain. Or change it into ""; then it is accesible from the whole world.
          note: disables ip updates for ip addresses not within the network.


 2. copy etc_profile.d_dyndns.sh to  /etc/profile.d/dyndsn.sh on the raspberry pi  => so that runs at each login
    and also add to the file /etc/rc.local the line:                                => so that runs at each boot (without having to log in) 
    
      /bin/bash /etc/profile.d/dyndsn.sh &
    

       => at beginning of script  edit
             network_prefix="131.174"
          to you own local domain. Or change it into ""; then it is accesible from the whole world.
          note: disables ip updates for ip addresses not within the network.
      
       => and at beginning of script edit 
             dyndns_url="http://www.cs.ru.nl/lab/dyndns/"
          to the url location of the webserver script
