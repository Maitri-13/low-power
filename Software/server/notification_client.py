import boto3
import socket, json, random
from time import time

BACKEND_IP   = '52.25.91.54'
BACKEND_PORT = 5555

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = (BACKEND_IP, BACKEND_PORT)
sock.connect(server_address)

print("Connected to %s:%d" % (server_address[0], server_address[1]))


event_time = int(time())
node_id = random.randint(0,3)
location_list = ["West shipping dock","Front Entrance","Back entrance","East Shipping and receiving"]


test_message = {}
test_message['data'] = {}
test_message['notification'] = {}


test_message['data']['body'] = {}
test_message['data']['body']['timestamp'] = event_time
test_message['data']['body']['message'] = 'alert detected from node %d' % (node_id)
test_message['data']['body']['node'] = node_id
test_message['data']['body']['location'] = location_list[node_id] 
test_message['data']['body']['aud_path'] = 'event_audio_%d.wav' % (event_time) 
test_message['data']['body']['img_path'] = 'event_image_%d.jpg' % (event_time) 

test_message['notification']['body']  = 'Intrusion event from node %d at %d' % (node_id, event_time)
test_message['notification']['title'] = 'Node %d Event' % (node_id)
test_message['notification']['icon']  = '3er_logo'

message = json.dumps(test_message)
print message
sock.sendall(message.encode())
