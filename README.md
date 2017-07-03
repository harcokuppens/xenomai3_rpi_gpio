# xenomai3_rpi2_gpio


overview
--------

notes:   
  contains all notes I made during my investigation of getting xenomai 3 working on pi1,pi2,pi3 
  which contains lots of topics :
      - xenomai2 versus xenomai3  and legacy notes
      - apis's compiling and posix wrapping
      - rtdm and gpio(linux vs xenomai)
      - interrupts 
      - cpu and irq load balancing and scheduling (linux vs xenomai)
  I call this sections notes and not documentation, because it contains also a lots of information copied from other sources
  for which I always place an url as reference.      
  
  Disclaimer:
    I also written down notes myself as I think it works, but there can be mistakes!
      
           
circuits
   overview of drawing and simulator tools used to create the models in this folder
   general documentation about electronic circuits, eg. pullup/pulldown, and there safety
   several circuit drawings from which some can be emulated
   NOTE: using the combined model one can test all examples in this repository!

examples   
   several examples to illustrate gpio input/output and interrupts
   linux and xenomai examples  ( for xenomai both userspace and kernel space examples)
      

install
   descriptions how to install xenomai3 on the pi1 or the pi2/3
   and some usefull startup tips and scripts to make a convenient image for the pi
