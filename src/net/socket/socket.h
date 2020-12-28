/**
 * @file socket.h
 * @author myhql (https://github.com/bianc2018)
 * @brief 
 *      对于不同平台的socket函数做一下统一化处理
 * @version 0.1
 * @date 2020-12-22
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef HWH_SOCKET_H_
#define HWH_SOCKET_H_

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #ifndef OS_WINDOWS
        #define OS_WINDOWS
    #endif
    #include <stdio.h>
    #include <winsock2.h>
    #ifndef INVALID_SOCKET
        #define INVALID_SOCKET (SOCKET)(~0)
    #endif
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #ifndef OS_LINUX
        #define OS_LINUX
    #endif
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #define SOCKET int
    #ifndef INVALID_SOCKET
        #define INVALID_SOCKET -1
    #endif
#else
    #error "不支持的平台"
#endif

//默认输出日志
#define DEFAULT_OUTPUT_LOG 1

enum SOCKLogLevel
{
    ERR,
    WARN,
    INFO,
    DEBUG
};

//简单的日志输出
typedef void(*SOCK_LOG_CB)(SOCKLogLevel lv,const char *msg);

//类型
enum SockType
{
    TCP,
    UDP
};

//返回的错误吗
typedef int SockRet;
//INVALID_SOCKET
class Socket
{
private:
    /* data */
    //原始套接字
    SOCKET sock_;
public:
    //构造函数
    Socket(/* args */);
    Socket(SOCKET sock);
    // SOCK_STREAM tcp SOCK_DGRAM udp
    Socket(SockType type);
    Socket(int af, int type, int protocol);

    //参数为左值引用的深拷贝构造函数，转移堆内存资源所有权，改变 source.ptr_ 的值
    Socket(Socket& s);

    //移动构造函数，与参数为左值引用的深拷贝构造函数基本一样
    Socket(Socket&& source);

    ~Socket();

    void swap(Socket& s);
private:
    //禁止拷贝
    //Socket() {};
    //Socket(const Socket&) {};
    Socket& operator=(const Socket&) {};
public:
    //封装的函数
    SockRet connect(const char*ipaddr,unsigned short port);
    SockRet bind(const char*ipaddr,unsigned short port);
    SockRet listen(int backlog);
    Socket accept();
    Socket accept(char*remote_ip,unsigned int ip_len,unsigned short *remote_port);
    SockRet send(const char*data,unsigned int data_len);
    SockRet sendto(const char* data, unsigned int data_len,\
                    const char* ipaddr, unsigned short port);
    SockRet recv(char*data,unsigned int data_len);
    SockRet recvfrom(char* data, unsigned int data_len,\
        const char* ipaddr, unsigned short port);

    SockRet close();
public:
    //拓展
    SOCKET get_socket();//获取原始socket
    unsigned short get_family();
public:
    //通用接口 设置日志输出
    static void set_logcb(SOCK_LOG_CB cb);

private:
};

#endif  //!HWH_SOCKET_H_