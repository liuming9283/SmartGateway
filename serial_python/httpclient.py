#!/usr/bin/env python
# -*- coding:utf-8 -*-
import urllib2
import time
import serial
import json
import gateway_monitor
import base64

def postHttp(temp):
    url="http://10.10.25.89:6666/myApp"
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


def main():  # 这里采用的是轮询串口的方式，每三秒轮询一次网关上的串口，看是否有数据过来；另一种想法是采用多线程方式,但是这样会产生同步的问题，而且两个子线程分别阻塞在自己的线程之内，不适合在子线程中处理，最后还是尝试了多进程。
	while True:
	    ser_blue = serial.Serial("/dev/ttyUSB0",9600)      # 蓝牙串口
            ser_zigbee = serial.Serial("/dev/ttyS0",115200)      # ZigBee串口
            print "=======轮询串口======="
            if ser_blue.isOpen():
                time.sleep(1)
                count = ser_blue.inWaiting()
                if count !=0:
                    recv = ser_blue.read(count)
                    print recv
                    str_handle(recv)
            ser_blue.close()

            if ser_zigbee.isOpen():
                time.sleep(1)
                count = ser_zigbee.inWaiting()
                if count != 0:
                    recv = ser_zigbee.read(count)
                    print recv
                    str_handle(recv)
            ser_zigbee.close()
            time.sleep(2)



if __name__ == "__main__":
 
	main()
