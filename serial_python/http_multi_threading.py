#!/usr/bin/env python
# -*- coding:utf-8 -*-
import urllib2
import time
import serial
import json
import gateway_monitor
import base64
import threading,signal,sys

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
    f = file("/home/pi/serial_python/devicd_db.json","r")
    dic_devicekey = json.load(f)
    f.close()
    if dic_devicekey.has_key(deviceid):
        return dic_devicekey[deviceid]
    else:
        return "failed"

def str_handle(data):
    try:
        recv_lst = data.split(";")
        if len(recv_lst) == 2:
            recv = recv_lst[0]
            if "Temp" in recv and recv.startswith("S:") and recv.endswith("E"):
                head = recv.split("S:")
                end = head[1].split("E")
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
            elif "HeartBeat" in recv and recv.startswith("S:") and recv.endswith("E"):
                head_h = recv.split("S:")
                end_h = head_h[1].split("E")
                devid_h = end_h[0]
                devid_h = str(get_devicekey(devid_h[2:8]))
                new_h = "Devkey="+ devid_h +"/"+end_h[0]
                new_h = new_h.replace("@","/").split("&")
                heart = new_h[0].replace("N="+end_h[0][2:8]+"/","")
                print heart
                postHttp(heart)
    
            else:
                print "recv 接收错误！！"
        else:
            print "------节点产生粘包  丢弃数据-----"

    except Exception,e:
        print "-----recv 接收错误！！-----"

def blue_thread():
    ser_blue = serial.Serial("/dev/ttyUSB0", 9600)  # 蓝牙串口
    print '\033[31;1m=======蓝牙子线程启动=======\033[0m'
    while ser_blue.isOpen():
        count = ser_blue.inWaiting()
        if count != 0:
            recv = ser_blue.read(count)
            print recv
            str_handle(recv)
        time.sleep(1)
    ser_blue.close()

def zigbee_thread():
    ser_zigbee = serial.Serial("/dev/ttyS0", 115200)  # zigbee串口
    print '\033[33;1m=======Zigbee子线程启动=======\033[0m'
    while ser_zigbee.isOpen():
        count = ser_zigbee.inWaiting()
        if count != 0:
            recv = ser_zigbee.read(count)
            print recv
            str_handle(recv)
        time.sleep(1)
    ser_zigbee.close()

def signal_handler(num,frame):
    print '\033[33;1m===BYEBYE====\033[0m'
    sys.exit(0)

def main():
# 由于python的多线程模块没有实现对信号的处理，所以父线程阻塞，我们需要安装一个信号处理函数，能让我们方便退出程序，并且要把子线程设为daemon
    signal.signal(signal.SIGINT,signal_handler)

    threads = []
    threads.append(threading.Thread(target=blue_thread,args=()))
    threads.append(threading.Thread(target=zigbee_thread,args=()))
    for t in threads:
        t.setDaemon(True)
        t.start()
#    t.join()
# 不能使用join，要使用isAlive方法模拟父线程阻塞，不断轮询子线程的运行状况，这个情况值得记录一下
    while True:
        alive = False
        for t in threads:  
            alive = alive or t.isAlive()  
            if alive == True:  
                break  
        if not alive:  
            break

if __name__ == "__main__":

    main()
