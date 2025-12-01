sudo modprobe can
sudo modprobe can_raw
sudo modprobe can_dev
sudo modprobe slcan

sudo ip link set can0 up type can bitrate 500000
