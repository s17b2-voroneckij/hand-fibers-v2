import socket
from sys import argv
from threading import Thread

num_clients = 100
num_threads = 10


def worker():
    share = num_clients // num_threads
    print("share is", share)
    sockets = [socket.socket(socket.AF_INET, socket.SOCK_STREAM) for i in range(share)]
    for s in sockets:
        s.connect(('localhost', int(argv[1])))
    print("connected")
    for i in range(100):
        for s in sockets:
            s.send(b"1\n")
            (s.recv(100))


if __name__ == "__main__":
    threads = [Thread(target=worker) for i in range(num_threads)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    print("leaving")