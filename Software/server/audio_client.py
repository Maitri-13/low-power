import socket, signal, wave, sys, os, struct
from time import time, sleep

from binascii import hexlify

# config variables for the RDS database
SERVER_IP    = '52.25.91.54'
LISTEN_PORT  = 5558
LISTEN_TIME  = 10 #seconds

AUDIO_BUFFER = []

def main():
	print("\nRunning audio listener, press Ctrl-C to exit!\n")

	conn = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	conn.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	print("Audio connecting to %s:%d " % (SERVER_IP, LISTEN_PORT))

	mywave =  wave.open("BYE_SHORT.wav", "rb")
	print(mywave.getparams())
	wavelen = mywave.getnframes()
	print("File has %d lines" % (wavelen))
	for i in range(wavelen):
		structdata = mywave.readframes(1)
		#try:
		data = struct.unpack("!l", structdata)
		conn.sendto(structdata, (SERVER_IP, LISTEN_PORT))
		if i == 12654 or i == 12653:
			print i
			print hexlify(structdata)
			print(data)
		#except:
		#	pass
		sleep(.00002)		


	conn.sendto("END END END", (SERVER_IP, LISTEN_PORT))
	print("Transfer complete!")

if __name__ == "__main__":
	main()
