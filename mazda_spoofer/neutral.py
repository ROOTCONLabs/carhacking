from pyvit import can
from pyvit.hw import socketcan

dev = socketcan.SocketCanDev("can0")

dev.start()

frame = can.Frame(0x228)

frame.data = [3,3,3,3,3,3]

dev.send(frame)
