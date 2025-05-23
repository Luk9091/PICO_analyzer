import sys
from threading import Thread

import serial
from serial import Serial

port = "/dev/ttyACM0"
speed = 115200


def parse():
    args = sys.argv[1:]
    if (len(args) >= 1):
        global port
        port = args[0]
    elif (len(args) >= 2):
        global speed
        speed = int(args[1])


def readThread():
    while(dev.is_open):
        try:
            data = dev.readline(256)
        except TypeError:
            return
        if not data:
            continue

        # data = data.decode()
        # data = data.strip("\n")
        # data = data.strip("\r")
        print(data)


parse()
dev = Serial(port, speed, timeout= 0.1, exclusive= True)

thread = Thread(target= readThread)
thread.start()
try:
    while(True):
        data = input()
        data += '\n'
        data = data.encode()
        dev.write(data)
except KeyboardInterrupt:
    pass

print("Good bey")
dev.close()
dev.cancel_read()
thread.join()
