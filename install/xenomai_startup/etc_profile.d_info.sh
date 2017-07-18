
printf "\n"

model=`cat /proc/device-tree/model`
compatible=`cat /proc/device-tree/compatible`
printf "model: $model\n"
printf "compatible: $compatible\n"

_IP=$(hostname -I) || true
if [ "$_IP" ]; then
  printf "My IP address is %s\n" "$_IP"
fi
