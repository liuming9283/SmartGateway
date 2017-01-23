#!/usr/bin/env python
# coding:utf-8
from SocketServer import *
import serial
'''webservice for local control
function is same to websocket'''

HOST = ''
PORT = 20000
ADDR = (HOST,PORT)

class MyRequestHandler(StreamRequestHandler):
    def handle(self):
        print '....connected from :', self.client_address
#        print self.rfile.readline().strip()
        self.data = self.request.recv(1024).strip()
        print self.data

	try:
        	ser = serial.Serial("/dev/ttyS0", 115200)
        	ser.write(self.data)
        except Exception,e:
        	print "连接异常，未发送成功！！",e
        finally:
        	ser.close()

def main():	
    tcpServ = TCPServer(ADDR, MyRequestHandler)
    print '-------tcp server for control start!--------'
    tcpServ.serve_forever()

if __name__ == '__main__':

    main()
