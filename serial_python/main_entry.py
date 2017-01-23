#!/usr/bin/env python
# coding:utf-8
# 项目的主程序入口，利用python多进程
# created by webber 2017/1/13
import sys
import multiprocessing
import httpclient
import http_multi_threading
import gateway_monitor
import websocket_test
import tcp_wifi
import tcp_control

plist = []

def worker_1():
    print '=========worker---1   start  !!!=========='
#    httpclient.main()
#    http_multi_process()
    http_multi_threading.main()

def worker_2():
    print '=========worker---2   start  !!!=========='
    gateway_monitor.main()

def worker_3():
    print '=========worker---3   start  !!!=========='
    websocket_test.main()

def worker_4():
    print '=========worker---4   start  !!!=========='
    tcp_wifi.main()

def worker_5():
    print '=========worker---5   start  !!!=========='
    tcp_control.main()

def main():
    p1 = multiprocessing.Process(target=worker_1,args=())    # httpclient
    plist.append(p1)
    p2 = multiprocessing.Process(target=worker_2,args=())    # gateway_monitor
    plist.append(p2)
    p3 = multiprocessing.Process(target=worker_3,args=())    # websocket
    plist.append(p3)
    p4 = multiprocessing.Process(target=worker_4,args=())    # tcp_wifi
    plist.append(p4)
    p5 = multiprocessing.Process(target=worker_5,args=())    # tcp_control
    plist.append(p5)

    for p in plist:
        p.start()
    p.join()

if __name__ == '__main__':

    main()
