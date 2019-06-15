import socket

TCP_IP = '0.0.0.0'
TCP_PORT = 5

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.bind((TCP_IP, TCP_PORT))
s.listen(1)
conn, addr = s.accept()
print("connection addr: {}".format(addr))
while True:
	data = conn.recv(1)
	print("recieved data: {}".format(data))
	conn.send(data)
conn.close()