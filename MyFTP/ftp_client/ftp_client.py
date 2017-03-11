# -*- coding:utf-8 -*-
__author__ = 'webber'
import socket
import getpass
import os
import sys, time
import json

HOST = '10.10.24.15'
PORT = 5000


class FTP(object):

    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def get(self, remote_file):  #get remote file from ftp_server
        filename = remote_file[0]
        cmd = "file_transfer|get|%s" % filename
        self.sock.send(cmd)
        feedback = self.sock.recv(1024)
        print "The message from ftp_server: ", feedback
        if feedback.startswith('The file is ready'): #用于与服务器交互
            while True:
                ack = raw_input(">>>:").strip()
                if len(ack) == 0: continue
                if ack == "yes":
                    self.sock.send(ack)
                    break
                elif ack == "no":
                    print "All right , you son of beach!!!"
                    return 0
                else:
                    print "wrong cmd, download failed!"
                    return 0
            for i in feedback.split():      #取出file_size
                if i.isdigit():
                    file_size = int(i)
            f = file("%s" % "recv/"+filename, 'wb')  #写文件到指定目录recv下
            recv_size = 0
            progress_percent = 0
            while not file_size == recv_size:
                if file_size - recv_size > 1024:
                    data = self.sock.recv(1024)
                    recv_size += len(data)      #!!注意，这里不一定会正好收到1024的消息，所以不要recv(1024)
                                                #所以这里改为实际recv到的data长度
                else:
                    data = self.sock.recv(file_size - recv_size)
                    recv_size += len(data)
                    '''
                    if file_size-recv_size > 0:                     #这里主要是针对接收1k左右的文件可能会出现的问题，如果不做这个逻辑，
                        data = self.sock.recv(file_size-recv_size)  # 那么只接收一次，而源文件发过来很多行，则只会接收一行数据
                        recv_size += len(data)
                    else:
                        recv_size = file_size
                    '''
                f.write(data)
                #print "----->", file_size, recv_size
                cur_percent = int(float(recv_size) / file_size * 100)
                if cur_percent > progress_percent:
                    progress_percent = cur_percent
                    self.show_progress(file_size, recv_size, progress_percent)
            else:
                f.close()
                print "------receive file %s done!!!--------" % filename
        else:
            print "file transfer failed!!!"

    def put(self, local_file):  #put local file to ftp_server
        filename = local_file[0]
        if os.path.isfile(filename):
            file_size = os.path.getsize(filename)
            cmd = "file_transfer|put|%s|%s" % (filename, file_size)
            self.sock.send(cmd)
            put_ack = self.sock.recv(1024)
            print "The message from ftp_server: ", put_ack
            if put_ack.startswith("I will receive"):
                f = file(filename, "rb")
                for line in f:
                    self.sock.send(line)
                else:
                    print "-------file %s send done--------" % filename
                    f.close()
                    print self.sock.recv(1024)
        else:
            print "-----No File in Local-----"

    def list(self):
        # 列出本地文件夹recv下的文件
        print "local files:"
        file_list = os.listdir('recv/')
        if len(file_list) == 0:
            print "None"
        elif len(file_list) > 0:
            for _file in file_list:
                print _file

    def server_file(self):
        # 列出Server端的文件
        print "Server files:"
        cmd = "file_query|list"
        self.sock.send(cmd)
        file_list = json.loads(self.sock.recv(8192))
        if len(file_list) == 0:
            print "None"
        elif len(file_list) > 0:
            for _file in file_list:
                print _file

    def quit(self):
        cmd = "file_quit|quit"
        self.sock.send(cmd)
        msg = self.sock.recv(1024)
        print "%s ========ByeBye==========" % msg
        self.sock.close()
        sys.exit(0)


    def help(self):
        print "\033[31;1m get|put|list|server_file \033[0m"
        print "\033[33;1m get [file path] \033[0m"
        print "\033[33;1m put [file path] \033[0m"
        print "\033[33;1m list (display local file) \033[0m"
        print "\033[33;1m server_file (display server file) \033[0m"
        print "\033[33;1m quit (quit the FTP client) \033[0m"

    def interaction(self):
        print "------use 'help' for command------"
        try:
            while True:
                user_input = raw_input("ftp_client>>>:").strip()
                if len(user_input) == 0: continue
                cmd = user_input.split()
                if hasattr(self, cmd[0]):
                    func = getattr(self, cmd[0])
                    if len(cmd) > 1:
                        func(cmd[1:])
                    elif cmd[0] == "help" or "list" or "server_file" or 'quit':
                        func()
                    else:
                        print "you must input a file path!!!"
                else:
                    print "--->> \033[31;1m %s \033[0m is a wrong cmd!!" % cmd[0]

        except Exception, e:
            print "server disconnected!!!", e

    def connect(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((self.host, self.port))
        self.verify()

    def verify(self):
        while True:
            try_count = 0
            while try_count < 3:
                user_name = raw_input("please input your username:").strip()
                if len(user_name) == 0: continue
                pwd = raw_input("input your password:").strip()
                #pwd = getpass.getpass("input your password:")   getpass()方法在win上面运行有问题
                auth_info = "ftp_authentication|%s|%s" % (user_name, pwd)

                self.sock.send(auth_info)
                auth_ack = self.sock.recv(1024)

                if auth_ack == "legal user":
                    self.interaction()
                else:
                    print auth_ack
                    try_count += 1
            else:
                print "you try more times!!!"

    def show_progress(self, total, finished, percent):
        progress_mark = "=" * (percent/2)
        sys.stdout.write("[%s/%s]%s>%s%%\r" % (total, finished, progress_mark, percent))
        sys.stdout.flush()
        if percent == 100:
            print '\n'

if __name__ == "__main__":
    ftp = FTP(HOST, PORT)
    ftp.connect()

