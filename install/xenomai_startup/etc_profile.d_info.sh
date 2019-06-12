
printf "\n"

model=$(tr -d '\0' < /proc/device-tree/model)
compatible=$(tr -d '\0' < /proc/device-tree/compatible)
printf "model: $model\n"
printf "compatible: $compatible\n"

_IP=$(hostname -I) || true
if [ "$_IP" ]; then
  printf "My IP address is %s\n" "$_IP"
fi
