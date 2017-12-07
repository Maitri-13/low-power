import socket, signal, wave, sys, os, struct
from time import time
from binascii import hexlify

STATE_CAPTURE  = 1
STATE_END      = 2
STATE_WAITING  = 3



# config variables for the RDS database
LISTEN_PORT  = 5558
LISTEN_TIME  = 10 #seconds



def sigalrm_handler(signum, frame):
	print("Capture timeout ")
	server_state = STATE_END

def main():
	AUDIO_BUFFER = []
	print("\nRunning audio listener, press Ctrl-C to exit!\n")

	conn = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	conn.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	conn.bind(('0.0.0.0', LISTEN_PORT))
	print("Audio listening on port %d " % (LISTEN_PORT))

	server_state = STATE_WAITING
	signal.signal(signal.SIGALRM, sigalrm_handler)
	counter = 0
	while True:

		(audio_data, address) = conn.recvfrom(1024)

		if server_state == STATE_WAITING:
			start_time = time()
			is_receiving = True 
			print("Starting capture")
			AUDIO_BUFFER.append(audio_data)
			server_state = STATE_CAPTURE
			continue;

		elif server_state == STATE_CAPTURE:
			if "END" in audio_data[:20]:
				print("Capture complete!")
				server_state = STATE_END
			AUDIO_BUFFER.append(audio_data)

		if server_state == STATE_END:
			print("Writing file...")	
			filename = "audio_capture_%d.wav" % (start_time)
			wavwrite =  wave.open(filename, 'w')
			print("2, 2, 44100, 9672600, 'NONE', 'not compressed'")
			wavwrite.setparams((2, 2, 44100, 9672600, 'NONE', 'not compressed'))
			for item in AUDIO_BUFFER:
				pack_val = struct.pack("l", item)
				wavwrite.writeframes(pack_val)
			wavwrite.close()
			AUDIO_BUFFER = []
			print("Write file complete!")	
			server_state = STATE_WAITING
			continue;
		

if __name__ == "__main__":
	main()
