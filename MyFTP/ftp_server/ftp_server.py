# -*- coding:utf-8 -*-
__author__ = 'webber'
import SocketServer
import os,sys,_socket
import verryfy_table
import json


class MyTCPHandler(SocketServer.BaseRequestHandler):

    def authentication(self):
        while True:
            msg = self.request.recv(1024).strip()
            if msg.startswith("ftp_authentication"):
                msg = msg.split("|")
                username = msg[1]
                pwd = msg[2]
                if verryfy_table.verify.has_key(username):
                    if verryfy_table.verify[username] == pwd:
                        auth_ack = "legal user"
                        self.request.send(auth_ack)
                        break
                    else:
                        auth_ack = "wrong password"
                        self.request.send(auth_ack)
                else:
                    auth_ack = "no this user!!!"
                    self.request.send(auth_ack)

    def handle(self):
        self.authentication()
        print "{}  has connected...".format(self.client_address)
        while True:
            try:
                msg = self.request.recv(1024).strip()
                print "{}  send message:".format(self.client_address),
                print msg
                msg = msg.split("|")
                if hasattr(self, msg[0]):
                    func = getattr(self, msg[0])
                    func(msg[1:])

            except Exception:
                print 'A client has left!!!',self.client_address
                break

    def file_transfer(self, msg):
        # 这里，我们把所有的上传文件都放在recv文件夹下
        filename = "recv/"+msg[1]
        if msg[0] == "get":
            if os.path.isfile(filename):
                file_size = os.path.getsize(filename)
                ack_msg = "The file is ready, filesize is %s ,Are you sure to download?(yse/no)" % file_size
                self.request.send(ack_msg)
                client_ack = self.request.recv(1024)     #等待接收客户端的确认信息（yes），避免“粘包”
                #print "---> client ack: ", client_ack
                if client_ack == "yes":
                    f = file(filename, "rb")
                    for line in f:
                        self.request.send(line)
                    else:
                        print "-------file %s send done--------" % filename
                        f.close()
                else:
                    self.request.send("wrong cmd!")
            else:
                ack_msg = "\033[32;1mSorry, No such file!!!\033[0m"
                self.request.send(ack_msg)

        elif msg[0] == "put":
            file_size = msg[2]
            if not os.path.isfile(filename):
                ack = "I will receive!!! ,filesize: %s" % file_size
                self.request.send(ack)

                f = file("%s" % filename, 'wb')  # 写文件
                recv_size = 0
                file_size = int(file_size)
                progress_percent = 0
                while not file_size == recv_size:
                    if file_size - recv_size > 1024:
                        data = self.request.recv(1024)
                        recv_size += len(data)
                    else:
                        data = self.request.recv(file_size - recv_size)
                        recv_size += len(data)
                    f.write(data)
                    #print "----->", file_size, recv_size
                    cur_percent = int(float(recv_size) / file_size * 100)
                    if cur_percent > progress_percent:
                        progress_percent = cur_percent
                        self.show_progress(file_size, recv_size, progress_percent)
                else:
                    f.close()
                    ack_recv_done = "------receive file %s done!!!--------" % filename
                    print ack_recv_done
                    self.request.send(ack_recv_done)
            else:
                ack = "The file name is existing!!!"
                self.request.send(ack)

    def file_query(self,msg):
        file_list = os.listdir('recv/')
        file_set = json.dumps(file_list)
        self.request.send(file_set)

    def file_quit(self,msg):
        self.request.send(msg)
        self.request.close()


    def show_progress(self, total, finished, percent):
        progress_mark = "=" * (percent/2)
        sys.stdout.write("[%s/%s]%s>%s%%\r" % (total, finished, progress_mark, percent))
        sys.stdout.flush()
        if percent == 100:
            print '\n'

if __name__ == "__main__":
    _socket.TCP_NODELAY = 0     # 关闭Nagle算法，防止“粘包”问题出现的另一种办法
    HOST, PORT = "10.10.24.15", 5000
    server = SocketServer.ThreadingTCPServer((HOST, PORT), MyTCPHandler)
    print 'waiting for connection........ '
    server.serve_forever()
