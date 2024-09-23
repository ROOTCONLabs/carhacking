from pyvit import can
from pyvit.hw import socketcan

dev = socketcan.SocketCanDev("can0")

dev.start()

frame = can.Frame(0x340)

frame.data = [40, 207, 118, 208, 115, 11, 90, 225]

dev.send(frame)
