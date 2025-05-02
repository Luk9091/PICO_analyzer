# import socket

# server_ip = "192.168.4.1"  
# server_port = 4444         

# sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# message = b"Hello Pico !!!"
# print("## PYTHON WIFI(via UDP) COMMUNICATION TEST ##")
# sock.sendto(message, (server_ip, server_port))

# response, server = sock.recvfrom(1024)
# print(f"server response: {response.decode()}")

# sock.close()


import socket
import time

local_ip = "0.0.0.0"     
local_port = 4444        
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((local_ip, local_port))
sock.settimeout(None)     

print("## UDP RECEIVE LOOP STARTED ##")
print(f"Listening on port {local_port}...\n")

last_time = None
try:
    while True:
        data, addr = sock.recvfrom(1024)  
        now = time.time()
        print(f"Received from {addr}: {data.decode(errors='replace')}")
        if last_time is not None:
            interval = now - last_time
            print(f"Time since last message: {interval:.3f} seconds\n")
        else:
            print("(First message received)\n")

        last_time = now

except KeyboardInterrupt:
    print("\n## Stopped by user ##")

finally:
    sock.close()
