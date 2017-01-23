#!/usr/bin/env python
# -*- coding:utf-8 -*-
__author__ = 'webber'
import websocket
from websocket import create_connection
import serial
import time

def main():
	ws = create_connection("ws://202.119.234.4/WebSocketUser/websocket.ws/relationId/123456")
	while True:
    		result = ws.recv()
    		print "Received %s" % result
    		try:
        		ser = serial.Serial("/dev/ttyUSB0", 115200)
                        for i in range(3):    
        			ser.write(result)
				time.sleep(0.5)
    		except Exception,e:
        		print "连接异常，未发送成功！！",e
    		finally:
        		ser.close()
	ws.close()

if __name__ == '__main__':

    main()
