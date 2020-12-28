#include "socket.h"

#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
    bool send_once=true;
    if (argc < 3)
    {
        printf("use ./sock_send ip port message argc %d\n",argc);
        return -1;
    }
    std::string ip = argv[1];
    unsigned int port = atoi(argv[2]);
    std::string message = "";
    if (argc >= 4)
        message = argv[3];
    else
        send_once = false;//½»»¥Ä£Ê½
    Socket s;
    auto ret = s.connect(ip.c_str(), port);
    printf("connect[%d] %s:%d \n", ret, ip.c_str(),port);

    do
    {
        if (!send_once)
        {
            printf("message:");
            std::cin >> message;
        }
        ret = s.send(message.c_str(), message.size());
        printf("send[%d] :%s\n", ret, message.c_str());

        printf("recv :\n");
        const int buff_len = 1024;
        char buff[1024] = { 0 };
        while (true)
        {
            int ret = s.recv(buff, buff_len);
            if (ret <= 0)
            {
                break;
            }
            if (ret < buff_len)
                buff[ret] = '\0';
            printf(buff);
            break;
        }
        printf("\n");
    } while (!send_once);
    s.close();
    return 0;
}