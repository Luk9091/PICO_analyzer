import os, pty, os
from threading import Thread
from time import sleep

last = 0

master, slave = pty.openpty()
slave_name = os.ttyname(slave)

print(f"Serial port: {slave_name}")


try:
    while True:
        data = os.read(master, 256)
        if not data:
            break

        print(data)
        out = b"\0\0\0\0"
        os.write(master, out)
except KeyboardInterrupt:
    pass

os.close(master)
os.close(slave)