#include "socket.h"
#include <thread>
void srv_thread()
{
    Socket s;
    s.bind(NULL, 80);
    s.listen(3);
    Socket c =s.accept();
    const int buff_len = 1024;
    char buff[1024] = { 0 };
    while (true)
    {
        int ret = c.recv(buff, buff_len);
        printf("recv:%d %s\n",ret, buff);
        if (ret <= 0)
            break;
    }
    
    //c.send(buff, buff_len);
}
int main(int argc,char*argv[])
{
    std::thread th(srv_thread);

    Socket s;
    auto ret = s.connect("127.0.0.1", 80);
    printf("connect %d\n", ret);
    while (true)
    {
        int ss = s.send("hello world!", strlen("hello world!"));
        if (ss <= 0)
            break;
    }
    
    if(th.joinable())
        th.join();
    return 0;
}