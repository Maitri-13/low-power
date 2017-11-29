#!/usr/bin/python

import mysql.connector
import socket, json
import boto3

# config variables for the RDS database
LISTEN_PORT  = 5555
RECV_LEN     = 2048
TOPIC_ARN    = "arn:aws:sns:us-west-2:159958132277:intrusionPushNotification"
RDS_HOST     = "intrusion-events.cyjeijyfmmbt.us-west-2.rds.amazonaws.com"
RDS_DATABASE = "events"

def main():

	with open("server.secret", "r") as f:
		data = f.read()

	aws_data = json.loads(data)
	ACCESS_KEY   = aws_data['access']
	SECRET_KEY   = aws_data['secret']
	RDS_USER     = aws_data['rds_user']
	RDS_PASSWORD = aws_data['rds_password']


	client = boto3.client('sns', 
		aws_access_key_id=ACCESS_KEY,
		aws_secret_access_key=SECRET_KEY,
			#	exit()
		region_name="us-west-2")

	print("\nRunning listener, press Ctrl-C to exit!\n")
	try:
		conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		conn.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		conn.bind(('0.0.0.0', LISTEN_PORT))
		conn.listen(5)
		print("Listening on port %d " % (LISTEN_PORT))

		while True:
			(curr_conn, address) = conn.accept()
			print ("received message from %s:%d" % (address[0], address[1]))

			input_data = curr_conn.recv(RECV_LEN)
		
			#try:
			json_data = json.loads(input_data)	
			message = json_data['message']
			node_id = json_data['node']
			location = json_data['location']
			timestamp = json_data['timestamp']
			print("[%d] received message from node %d: \"%s\"" % (timestamp, node_id, message))

			out_message = json.dumps(json_data)
			print("Attempting to publish %s to %s" % (out_message, TOPIC_ARN))

			response = client.publish(Message=out_message, TopicArn=TOPIC_ARN)
			format_response = response['MessageId']
			print("Publish complete: received %s" % (format_response))
			
			cnx = mysql.connector.connect(user=RDS_USER, password=RDS_PASSWORD, database=RDS_DATABASE, host=RDS_HOST)
			cursor = cnx.cursor()

			insert_str = "INSERT INTO intrusion_events (node_id, location, message, unixtime) VALUES (%s, %s, %s, %s)"

			values = (node_id, location, message, timestamp)

			cursor.execute(insert_str, values)
			cnx.commit()

			cursor.close()
			cnx.close()

	# if Ctrl-C is pressed, exit the program
	except KeyboardInterrupt:
		print("\n\tExiting program! Goodbye...\n")
		exit()


if __name__ == "__main__":
	main()
