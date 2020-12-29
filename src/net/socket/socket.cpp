#include "socket.h"
#include <sstream>

static char LogL[4][8] = { "ERR","WARN","INFO","DEBUG" };
//默认日志输出
static void DefaultSockLog_CB(SOCKLogLevel lv, const char* msg)
{
	printf("l[%5s] %s\n", LogL[lv], msg);
}
//日志模块初始化
#if DEFAULT_OUTPUT_LOG
static SOCK_LOG_CB gLogOutPut= DefaultSockLog_CB;
#else
static SOCK_LOG_CB gLogOutPut = NULL;
#endif

#define _LOG(lv,msg)\
{	\
	std::stringstream os;\
	os<<msg;\
	if(gLogOutPut)\
		gLogOutPut(lv,os.str().c_str());\
}
#define _LOG_ERROR(msg) _LOG(ERR,msg)
#define _LOG_WARN(msg) _LOG(WARN,msg)
#define _LOG_INFO(msg) _LOG(INFO,msg)
#define _LOG_DEBUG(msg) _LOG(DEBUG,msg)

#ifdef OS_WINDOWS
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
//初始化函数
class WsInit
{
public:
	WsInit()
	{
		WSADATA wsaData;
		int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
		_LOG_DEBUG("WSAStartup "<< wsaData.wVersion<<" ret="<<ret);
	}
	~WsInit()
	{
		//终止 DLL 的使用
		WSACleanup();
		_LOG_DEBUG("WSACleanup ");
	}
};
#endif // OS_WINDOWS

//全局初始化
//私有函数 
static bool try_init()
{
#ifdef OS_WINDOWS
	static WsInit windows_net_init;
#endif
	return true;
}
static bool try_uninit()
{
	return true;
}

SOCKET Socket::get_socket()
{
	return sock_;
}

unsigned short Socket::get_family()
{
	return AF_INET;
}

void Socket::set_logcb(SOCK_LOG_CB cb)
{
	gLogOutPut = cb;
}

Socket::Socket():sock_(INVALID_SOCKET)
{
	try_init();
	sock_ = socket(AF_INET, SOCK_STREAM, 0);//默认tcp
}

Socket::Socket(SOCKET sock) : sock_(sock)
{
	try_init();
}

Socket::Socket(SockType type)
{
	try_init();
	if (type == TCP)
		sock_ = socket(AF_INET, SOCK_STREAM, 0);
	else if (type == UDP)
		sock_ = socket(AF_INET, SOCK_DGRAM, 0);
	else
		sock_ = INVALID_SOCKET;
}

Socket::Socket(int af, int type, int protocol)
{
	try_init();
	sock_ = socket(af, type, protocol);
}

//Socket::Socket(Socket& s):sock_(s.sock_)
//{
//	s.sock_ = INVALID_SOCKET;
//}
//
//Socket::Socket(Socket&& s):sock_(s.sock_)
//{
//	s.sock_ = INVALID_SOCKET;
//}

Socket::~Socket()
{
	//close();
	//try_uninit();
}

//void Socket::swap(Socket& s)
//{
//	SOCKET sock = s.sock_;
//	s.sock_ = sock_;
//	sock_ = sock;
//}


SockRet Socket::connect(const char* ipaddr, unsigned short port)
{
	//创建sockaddr_in结构体变量
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
	serv_addr.sin_family = get_family();  //使用IPv4地址
	serv_addr.sin_addr.s_addr = inet_addr(ipaddr);  //具体的IP地址
	serv_addr.sin_port = htons(port);  //端口
	//将套接字和IP、端口绑定
	int ret = ::connect(sock_, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (ret != 0)
	{
		_LOG_DEBUG("connect fail ret=" << ret << ",err:" << errno);
	}
	return ret;
}

SockRet Socket::bind(const char* ipaddr, unsigned short port)
{
	//创建sockaddr_in结构体变量
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
	serv_addr.sin_family = get_family();  //使用IPv4地址
	//htonl (INADDR_ANY)
	if(ipaddr)
		serv_addr.sin_addr.s_addr = inet_addr(ipaddr);  //具体的IP地址
	else
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //所有ip

	serv_addr.sin_port = htons(port);  //端口
	//将套接字和IP、端口绑定
	int ret = ::bind(sock_, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (ret != 0)
	{
		_LOG_DEBUG("bind fail ret=" << ret << ",err:" << errno);
	}
	return ret;
}

SockRet Socket::listen(int backlog)
{
	int ret = ::listen(sock_, backlog);
	if (ret != 0)
	{
		_LOG_DEBUG("listen fail ret=" << ret << ",err:" << errno);
	}
	return ret;
}

Socket Socket::accept()
{
	int ret = ::accept(sock_, NULL,0);
	if (ret == INVALID_SOCKET)
	{
		_LOG_DEBUG("accept fail ret=" << ret << ",err:" << errno);
	}
	return Socket((SOCKET)ret);
}

Socket Socket::accept(char* remote_ip, unsigned int ip_len, unsigned short* remote_port)
{
	//创建sockaddr_in结构体变量
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));  //每个字节都用0填充

#ifdef OS_WINDOWS
	int addr_len = sizeof(addr);
#endif

#ifdef OS_LINUX
	socklen_t addr_len = sizeof(addr);
#endif
	int ret = ::accept(sock_, (struct sockaddr*)&addr,&addr_len);
	if (ret == INVALID_SOCKET)
	{
		_LOG_DEBUG("accept fail ret=" << ret << ",err:" << errno);
	}
	//需要返回ip
	if (remote_ip&&ip_len!=0)
	{
		char* temp = ::inet_ntoa(addr.sin_addr);
		if (temp)
		{
			unsigned int temp_len = ::strlen(temp);
			if (temp_len < ip_len)//缓存足够
			{
				::strcpy(remote_ip, temp);
			}
		}
	}
	if (remote_port)
	{
		*remote_port = ::ntohs(addr.sin_port);
	}
	return Socket((SOCKET)ret);
	
}

SockRet Socket::send(const char* data, unsigned int data_len)
{
	int ret = ::send(sock_, data, data_len, 0);
	if (ret <= 0)
	{
		_LOG_DEBUG("send fail ret=" << ret << ",err:" << errno);
	}
	return ret;
}

SockRet Socket::sendto(const char* data, unsigned int data_len,
	const char* ipaddr, unsigned short port)
{
	//创建sockaddr_in结构体变量
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
	serv_addr.sin_family = get_family();  //使用IPv4地址
	serv_addr.sin_addr.s_addr = inet_addr(ipaddr);  //具体的IP地址
	serv_addr.sin_port = htons(port);  //端口
	//将套接字和IP、端口绑定
	int ret = ::sendto(sock_, data, data_len, 0, 
		(struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (ret <= 0)
	{
		_LOG_DEBUG("sendto fail ret=" << ret << ",err:" << errno);
	}
	return ret;
}

SockRet Socket::recv(char* data, unsigned int data_len)
{
	int ret = ::recv(sock_, data, data_len, 0);
	if (ret <= 0)
	{
		_LOG_DEBUG("recv fail ret=" << ret << ",err:" << errno);
	}
	return ret;
}

SockRet Socket::recvfrom(char* data, unsigned int data_len, 
	const char* ipaddr, unsigned short port)
{
	//创建sockaddr_in结构体变量
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
	serv_addr.sin_family = get_family();  //使用IPv4地址
	serv_addr.sin_addr.s_addr = inet_addr(ipaddr);  //具体的IP地址
	serv_addr.sin_port = htons(port);  //端口
#ifdef OS_WINDOWS
	int add_len = sizeof(serv_addr);
#endif

#ifdef OS_LINUX
	socklen_t add_len = sizeof(serv_addr);
#endif
	
	//将套接字和IP、端口绑定
	int ret = ::recvfrom(sock_, data, data_len, 0,
		(struct sockaddr*)&serv_addr,&add_len);
	if (ret <= 0)
	{
		_LOG_DEBUG("recvfrom fail ret=" << ret << ",err:" << errno);
	}
	return ret;
}

SockRet Socket::close()
{
	SOCKET temp=sock_;
	sock_ = INVALID_SOCKET;

#ifdef OS_WINDOWS
	return ::closesocket(temp);
#endif

#ifdef OS_LINUX
	return ::close(temp);
#endif
}
