#!/usr/bin/env python
# -*- coding:utf-8 -*-
__author__ = 'webber'
import os
import sys
import time
import httpclient

def cpuinfo():
    """
    get cpuinfo from '/proc/stat' and
    calculate the percentage of the cpu occupy.
    """
    f = open('/proc/stat','r')
    cpu = f.readline()
    f.close()
    #print "cpuinfo: ", cpu
    cpu = cpu.split(" ")
    total = 0
    usr = float(cpu[2])     #用户态cpu占用率
    _sys = float(cpu[4])    #内核态cpu占用率
    for info in cpu:
        if info.isdigit():
            total += float(info)
    print '\033[31mcpu info:  \033[0m',
    print 'usr: %.5f%%' % ((usr/total)*100),
    print '     sys: %.5f%%' % ((_sys/total)*100)
    return str((usr/total)*100)+'&'+str((_sys/total)*100)

def meminfo():
    """
    get meminfo from '/proc/meminfo' and
    calculate the percentage of the mem occupy
    used = total - free - buffers - cached
    """
    f = open('/proc/meminfo','r')
    mem = f.readlines()
    f.close()
    #print "meminfo", mem
    total, free, buffers, cached = 0, 0, 0, 0
    for info in mem:
        mem_item = info.lower().split()
        #print mem_item
        if mem_item[0] == 'memtotal:':
            total = float(mem_item[1])
        if mem_item[0] == 'memfree:':
            free = float(mem_item[1])
        if mem_item[0] == 'buffers:':
            buffers = float(mem_item[1])
        if mem_item[0] == 'cached:':
            cached = float(mem_item[1])
    used = total - free - buffers - cached
    print "\033[31mmeminfo:  \033[0m",
    print "total: %.3f GB" % (total/1024/1024),
    print "     used: %.5f%%" % (used/total*100)
    return str(total/1024/1024)+'&'+str(used/total)

def netinfo():
    """
    get real-time bandwidth
    """
    f = open('/proc/net/dev','r')
    net = f.readlines()
    f.close()

    net_item = []
    for info in net:
        if info.strip().startswith("eth0"):
            net_item = info.strip().split()
            break
    # print net_item
    recv = float(net_item[1])
    send = float(net_item[9])
    #print "recv:%s " % recv
    #print "send:%s " % send
    time.sleep(1)

    f2 = open('/proc/net/dev','r')
    _net = f2.readlines()
    f2.close()
    _net_item = []
    for info in _net:
        if info.strip().startswith("eth0"):
            _net_item = info.strip().split()
            break
    recv_2 = float(_net_item[1])
    send_2 = float(_net_item[9])

    #print "recv_2:%s " % recv_2
    #print "send_2:%s " % send_2
    print "\033[31m network info:  \033[0m"
    print "received: %.3f Kb/s" % (recv_2/1024 - recv/1024)
    print "transmit: %.3f kb/s" % (send_2/1024 - send/1024)
    return str((recv_2/1024 - recv/1024)) + '&' + str((send_2/1024 - send/1024))

def loadavg():
    pass


def main():
    while True:
        try:
#            os.system('clear')
            cpu = cpuinfo()
            print "=============================================="
            mem = meminfo()
            print "##############################################"
            net = netinfo()
            
            time.sleep(3)
            httpclient.postHttp('gateway/10.10.25.89/b8:27:eb:c9:4c:0a/'+cpu+'/'+mem+'/'+net+'/123456')
            time.sleep(5)

        except KeyboardInterrupt, e:  # 这里也可以用信号函数来处理
            print ''
            print "BYE-BYE"
            sys.exit(0)


if __name__ == '__main__':

    main()

