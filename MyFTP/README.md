# MyFTP
My_FTP
闲来无事，自己写了个FTP的小项目，里面有很多瑕疵，后面使用的时候可以边用边改。

## Server端：主要有以下几个方法：
    ---authentication方法：用于用户的身份认证，这里会在verrify_table的字典中进行匹配，这里其实应该把该文件进行MD5加密，偷了个懒。
    ---handle方法：这里用到了反射，将socket传过来的字符串以“|”分隔开，根据各段字符串的内容，对应到各自的方法中，分别反射到：file_transfer、file_query、file_quit中。
       ---file_transfer方法：主要用于处理put和get请求
       ---file_query方法：用于查询本地的文件
       ---file_quit方法：用于退出。
### 在这里通过学习有了以下收获：
   在Server端存在一个socket“粘包”的问题，也就是如果待发送的消息很小，那么TCP默认会把消息放在缓冲区中，等待更多的消息（最多达到MTU1500）一起发送，只有当缓冲区满了或者到达超时时间，TCP才会把缓冲区中的内容发送出去。这是典型的TCP中Nagle会引起的问题。  
### 解决方法：
    1、关闭Nagle算法，调用_socket模块中的TCP_NODELAY，将它的默认值由1改为0。（TCP/IP详解一书中指出）。
    2、立即调用recv方法接收，阻塞它等待client的确认。
         

## Client端：主要有以下几个方法：
    ---put：主动向Server端传送文件。
    ---get：从Server端拉取文件。
    ---list：查询本地文件库中的文件。
    ---server_file：查询Server端的文件库中的文件。
    ---help：查看帮助。
  
## 用法：
   根据Server端部署的实际的IP，首先为Server指定IP和端口，本例中指定的IP为10.10.24.15，端口号为5000，然后客户端要与Server端的IP和端口保持一致，默认的用户名为webber，密码为123456 。
   分别启动Server端和client端，在client端输入用户名、密码，进入命令行交互界面，可以通过help获取命令列表，通过键入相关命令来完成文件的传输和查询，命令列表如下：
   
       get|put|list|server_file 
       get [file path] 
       put [file path] 
       list (display local file) 
       server_file (display server file) 
       quit (quit the FTP client) 
