

example build and execution
-----------------------------

$ make

$ scp  button_toggles_led.ko pi:/tmp

$ ssh root@pi

root@raspberrypi:~ # echo insmod; insmod /tmp/button_toggles_led.ko ; tail -11 /var/log/messages; echo sleep 5; sleep 5; echo rmmod; rmmod button_toggles_led ; tail -2 /var/log/messages;
insmod
Jun 28 13:14:12 raspberrypi kernel: [105543.239058] irq number: 503
Jun 28 13:14:12 raspberrypi kernel: [105543.239079] cpu: 2                 => cpu number on which program runs  => in code cpu affinity set to cpu2
Jun 28 13:14:12 raspberrypi kernel: [105543.239089] gpio request in
Jun 28 13:14:12 raspberrypi kernel: [105543.239114] gpio direction in
Jun 28 13:14:12 raspberrypi kernel: [105543.239130] gpio request in
Jun 28 13:14:12 raspberrypi kernel: [105543.239150] gpio direction out
Jun 28 13:14:12 raspberrypi kernel: [105543.239167] set irq trigger: rising
Jun 28 13:14:12 raspberrypi kernel: [105543.239185] irq request , IRQ_TYPE_EDGE_FALLING
Jun 28 13:14:12 raspberrypi kernel: [105543.239222] globalCounter 0
Jun 28 13:14:12 raspberrypi kernel: [105543.239233] handlerCPU: -1
Jun 28 13:14:12 raspberrypi kernel: [105543.239243] done
sleep 5                                                                     => user can generate interrups
rmmod
Jun 28 13:14:17 raspberrypi kernel: [105548.277209] globalCounter 175       => interrupts counted
Jun 28 13:14:17 raspberrypi kernel: [105548.277230] handlerCPU: 0           => cpu number on which interrupt handler runs  => handler runs by default on cpu0









Note: I also tried to change the cpu affinity for irq to another core then cpu0, however that failed.
      This fails probably because for all the pi's (pi1,pi2,pi3) there is no vectored interrupt controller implemented.
      For the pi2 and p3 there is  a simple register-based interface to set the global top-level interrupt to a specific core.
      Probably the  xnintr_affinity method works only for (vectored) interrupt controllers  and not for
      a specialized register-based interface on the arm processor.
       
      see: rpi2_irq_affinity.txt