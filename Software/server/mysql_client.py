import boto3
import socket, json
from time import time

BACKEND_IP   = '52.25.91.54'
BACKEND_PORT = 5566

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (BACKEND_IP, BACKEND_PORT)
sock.connect(server_address)

print("Connected to %s:%d" % (server_address[0], server_address[1]))
sock.sendall("DATA REQUEST".encode())

sock.close()

