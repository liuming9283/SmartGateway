#!/usr/bin/env python
from SocketServer import *
import json
import httpclient

HOST = ''
PORT = 6535
ADDR = (HOST,PORT)

class MyRequestHandler(StreamRequestHandler):
    def handle(self):
        print '....connected from :', self.client_address
#        print self.rfile.readline().strip()
	self.data = self.request.recv(1024).strip()
	print self.data
        str_handle(self.data)

def get_devicekey(deviceid):
    f = file("devicd_db.json","r")
    dic_devicekey = json.load(f)
    f.close()
    if dic_devicekey.has_key(deviceid):
        return dic_devicekey[deviceid]
    else:
        return "failed"

def str_handle(recv):
    if recv.startswith("S:") and recv.endswith("E;"):
        head = recv.split("S:")
        end = head[1].split("E;")
        devid = end[0]
        devid = str(get_devicekey(devid[2:8]))
        new = "Devkey="+ devid +"/"+end[0]
        new = new.replace("@","/").split("&")
        t = new[0].replace("N="+end[0][2:8]+"/","")
        temp = t
        humi = t.split("/")
        humi = humi[0] + "/" + humi[1] + "/" + humi[2] + "/" + humi[3] + "/" + humi[4]+"/" + new[1]
        print temp
        print humi
        httpclient.postHttp(temp)
        httpclient.postHttp(humi)

def main():
    tcpServ = TCPServer(ADDR, MyRequestHandler)
    print "----------tcp server for wifi start--------------"
    tcpServ.serve_forever()

if __name__ == '__main__':

    main()
