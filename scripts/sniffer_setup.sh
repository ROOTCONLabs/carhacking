#!/bin/bash
# This script enables SocketCAN with CANtact and STM32 cansniffers (or other USB CAN-based sniffers)

sudo modprobe can
sudo modprobe vcan
sudo modprobe slcan
sudo slcand -o -c -s6 /dev/ttyACM0 can0
sudo ifconfig can0 up
sudo ifconfig can0 txqueuelen 10000
sudo ip link set dev can0 txqueuelen 10000
