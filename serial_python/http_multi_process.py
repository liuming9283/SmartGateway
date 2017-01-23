#!/usr/bin/env python
# -*- coding:utf-8 -*-
import urllib2
import time
import serial
import json
import gateway_monitor
import base64
import multiprocessing

def postHttp(temp):
    url="http://10.10.24.234:6666/myApp"
    temp = base64.b64encode(temp)
    postdata=temp
    #postdata=urllib.urlencode(postdata)
    try:
        request = urllib2.Request(url,postdata)
        response=urllib2.urlopen(request)
       # print response
    except Exception,e:
        print "Something Wrong!!",e

def get_devicekey(deviceid):
    f = file("devicd_db.json","r")
    dic_devicekey = json.load(f)
    f.close()
    if dic_devicekey.has_key(deviceid):
        return dic_devicekey[deviceid]
    else:
        return "failed"

def str_handle(recv):
    if recv.startswith("S:") and recv.endswith("E;"):  # 数据包
        head = recv.split("S:")
        end = head[1].split("E;")
        devid = end[0]
        devid = str(get_devicekey(devid[2:8]))
        new = "Devkey="+ devid +"/"+end[0]
        new = new.replace("@","/").split("&")
        t = new[0].replace("N="+end[0][2:8]+"/","")
        temp = t
        humi = t.split("/")
        humi = humi[0] + "/" + humi[1] + "/" + humi[2] + "/" + humi[3] + "/" +humi[4]+ "/" + new[1]
        print temp
        print humi
        postHttp(temp)
        postHttp(humi)
    else:
        print "recv 接收错误！！"


def blue_pro():
    ser_blue = serial.Serial("/dev/ttyUSB0", 9600)  # 蓝牙串口
    print '\033[31;1m=======蓝牙子进程启动=======\033[0m'
    while ser_blue.isOpen():
        count = ser_blue.inWaiting()
        if count != 0:
            recv = ser_blue.read(count)
            print recv
            str_handle(recv)
        time.sleep(1)
    ser_blue.close()

def zigbee_pro():
    ser_zigbee = serial.Serial("/dev/ttyS0", 115200)  # zigbee串口
    print '\033[33;1m=======Zigbee子进程启动=======\033[0m'
    while ser_zigbee.isOpen():
        count = ser_zigbee.inWaiting()
        if count != 0:
            recv = ser_zigbee.read(count)
            print recv
            str_handle(recv)
        time.sleep(1)
    ser_zigbee.close()

def main():
    plist = []
    plist.append(multiprocessing.Process(target=blue_pro,args=()))
    plist.append(multiprocessing.Process(target=zigbee_pro,args=()))
    for p in plist:
        p.start()
    p.join()

if __name__ == "__main__":
 
	main()
