

printf "\n"
# show version
/usr/xenomai/sbin/version

# fix paths for xenomai
export PATH=$PATH:/usr/xenomai/bin
export LD_LIBRARY_PATH=/usr/xenomai/lib/

# add some aliases convenient for xenomai
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
    xeno_version=$(/usr/xenomai/sbin/version); 
    if [[ $xeno_version =~ cobalt ]];then echo already running: $xeno_version; return; fi
    echo unexport all gpio pins so they are free to use in cobalt
    gpio unexportall
    if grep BCM2708 /proc/cpuinfo >/dev/null
    then 
      echo "use /usr/xenomai_pi1_cobalt as /usr/xenomai"
      ln -nsf /usr/xenomai_pi1_cobalt /usr/xenomai  
    else 
      echo "use /usr/xenomai_pi2_cobalt as /usr/xenomai"  
      ln -nsf /usr/xenomai_pi2_cobalt /usr/xenomai  
    fi
    xeno_version=$(/usr/xenomai/sbin/version)
    echo "now running: $xeno_version"
}
xeno-switch-mercury() {
    xeno_version=$(/usr/xenomai/sbin/version)
    if [[ $xeno_version =~ mercury ]];then echo already running: $xeno_version; return; fi
    if grep BCM2708 /proc/cpuinfo >/dev/null
    then 
      echo "use /usr/xenomai_pi1_mercury as /usr/xenomai"
      ln -nsf /usr/xenomai_pi1_mercury /usr/xenomai  
    else 
      echo "use /usr/xenomai_pi2_mercury as /usr/xenomai"  
      ln -nsf /usr/xenomai_pi2_mercury /usr/xenomai  
    fi
    xeno_version=$(/usr/xenomai/sbin/version)
    echo "now running: $xeno_version"
}
