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
test_message['data'] = {}
test_message['notification'] = {}


test_message['data']['body'] = {}
test_message['data']['body']['timestamp'] = time()
test_message['data']['body']['message'] = 'alert detected from node 3'
test_message['data']['body']['node'] = 3
test_message['data']['body']['location'] = 'West shipping and receiving dock'

test_message['notification']['body']  = 'Wow holy shit an alert'
test_message['notification']['title'] = 'Mama mia'
test_message['notification']['icon']  = 'icon'

message = json.dumps(test_message)
print message
sock.sendall(message.encode())
