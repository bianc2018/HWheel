#include "socket.h"
#include <thread>

//客户端线程
void cli_th(Socket c)
{
    const int buff_len = 1024;
    char buff[1024] = { 0 };
    while (true)
    {
        int ret = c.recv(buff, buff_len);
        if (ret <= 0)
        {
            break;
           // printf("...\n");
           // continue;

        }
        if (ret < buff_len)
            buff[ret] = '\0';
        printf("recv:%d %s\n", ret, buff);
        //回复
        c.send(buff, ret);
    }
    c.close();
}

int main(int argc, char* argv[])
{
    unsigned short port = 80;
    if(argc>=2)
        port=::atoi(argv[1]);

    Socket s;
    int ret=s.bind(NULL, port);
    if (ret != 0)
    {
        printf("bind fail ret=%d\n", ret);
        return -1;
    }
    ret = s.listen(10);
    if (ret != 0)
    {
        printf("listen fail ret=%d\n", ret);
        return -1;
    }
    while (true)
    {
        Socket c = s.accept();
        std::thread cli([c]()mutable {cli_th(c); });
        cli.detach();//直接
       // cli.join();
    }
    return 0;
}