import socket, time

TCP_IP = '192.168.1.103'
TCP_PORT = 80

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print("connecting")
s.connect((TCP_IP, TCP_PORT))
print("connected")
# while True:
#   s.send(bytes('i', 'utf-8'))
#   data = s.recv(1)
#   print("llego: {}".format(data))
  #time.sleep(3)
