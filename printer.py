from time import sleep
from sys import argv
import socket

if __name__ == "__main__":
    while True:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect(('localhost', 8002))
            for i in range(1000):
                s.send(argv[1].encode('utf-8') + b'\n')
                print(i)
                print(s.recv(100))
