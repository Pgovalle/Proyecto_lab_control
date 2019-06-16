'''
Con esto se puede probar el uso de sockets UDP
haciendo uso de python. Notar que se debe cambiar
la variable UDP_IP a la correspondiente IP del Photon
'''

# Tomas Herrera C
# PUC


import socket

UDP_IPs = ["192.168.100.100","192.168.1.103"]
UDP_PORT = 80
i = 0;
while True:
	UDP_IP = UDP_IPs[i]
	i = i+1
	if(i>1):
		i = 0
	MESSAGE = raw_input(" i para prender un led, otro para apagar el led: ")
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
	sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))
