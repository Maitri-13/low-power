import boto3
import socket, json
from time import time

BACKEND_IP   = '52.25.91.54'
BACKEND_PORT = 5555

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (BACKEND_IP, BACKEND_PORT)
sock.connect(server_address)

print("Connected to %s:%d" % (server_address[0], server_address[1]))
test_message = {}
test_message['timestamp'] = time()
test_message['notification'] = True
test_message['message'] = 'alert detected from node 1'
test_message['node'] = 1
test_message['location'] = 'East shipping and receiving dock'
message = json.dumps(test_message)
print message
sock.sendall(message.encode())
