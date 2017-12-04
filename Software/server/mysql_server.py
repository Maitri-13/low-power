import mysql.connector
import socket, json

RDS_HOST     = "intrusion-events.cyjeijyfmmbt.us-west-2.rds.amazonaws.com"
RDS_DATABASE = "events"
LISTEN_PORT  = 5566
RECV_LEN     = 2048


def main():

	# get data 
	with open("server.secret", "r") as f:
		data = f.read()

	aws_data = json.loads(data)
	RDS_USER     = aws_data['rds_user']
	RDS_PASSWORD = aws_data['rds_password']

	# connect to MYSQL
	try:
		cnx = mysql.connector.connect(user=RDS_USER, password=RDS_PASSWORD, database=RDS_DATABASE, host=RDS_HOST)
		cursor = cnx.cursor()
	except:
		print("UNABLE TO CONNECT TO DATABASE")
		sys.exit(1)

	# create socket 
	print("\nRunning RDS listener, press Ctrl-C to exit!\n")
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

			if "DATA REQUEST" in input_data:
				print("Querying Database...")
				MYSQL_QUERY    = "SELECT node_id, location, message, unixtime  FROM intrusion_events ORDER BY unixtime DESC LIMIT 8;" 

				cursor.execute(MYSQL_QUERY)
				results_dict = {}
				counter = 0
				for (node_id, location, message, unixtime ) in cursor:
					key = "entry-%d" % (counter)
					results_dict[key] = {}
					results_dict[key]['node_id']  = str(node_id)
					results_dict[key]['location'] = str(location)
					results_dict[key]['message']  = str(message)
					results_dict[key]['unixtime'] = str(unixtime)
					if results_dict[key]['unixtime'] == None:
						results_dict[key]['unixtime'] = "Unknown" 
					counter += 1;


				print("Read results complete")
				
			else:
				print("Invalid Request! Got %s" % (input_data))
				
			print(results_dict)
			curr_conn.send(str(results_dict)+"\n")

	# if Ctrl-C is pressed, exit the program
	except KeyboardInterrupt:
		print("\n\tExiting program! Goodbye...\n")
		cursor.close()
		cnx.close()
		exit()


if __name__ == "__main__":
	main()


