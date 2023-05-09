sudo ip link set can0 type can bitrate 500000
sudo ip link set up can0
sudo ifconfig can0 txqueuelen 10000
sudo ip link set dev can0 txqueuelen 10000
