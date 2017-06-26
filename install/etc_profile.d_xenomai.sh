
printf "\n"

model=`cat /proc/device-tree/model`
compatible=`cat /proc/device-tree/compatible`
printf "model: $model\n"
printf "compatible: $compatible\n"

if grep BCM2708 /proc/cpuinfo >/dev/null
then 
  echo use /usr/xenomai_pi1 as /usr/xenomai  
  ln -nsf /usr/xenomai_pi1 /usr/xenomai  
else 
  echo use /usr/xenomai_pi2 as /usr/xenomai  
  ln -nsf /usr/xenomai_pi2 /usr/xenomai  
fi
export PATH=$PATH:/usr/xenomai/bin

_IP=$(hostname -I) || true
if [ "$_IP" ]; then
  printf "My IP address is %s\n" "$_IP"
fi



