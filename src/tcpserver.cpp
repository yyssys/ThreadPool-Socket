#include "tcpserver.h"

tcpServer::tcpServer()
{
    lfd = socket(AF_INET, SOCK_STREAM, 0);
}

int tcpServer::setListen(unsigned short int port)
{
    struct sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int res = bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
    if (res == -1)
    {
        perror("bind");
        return -1;
    }

    res = listen(lfd, 128);
    if (res == -1)
    {
        perror("listen");
        return -1;
    }
    std::cout << "套接字设置绑定与监听成功" << std::endl;
    return res;
}

tcpSocket tcpServer::acceptConnect()
{
    struct sockaddr_in addr{};
    socklen_t len = sizeof(addr);
    int cfd = accept(lfd, (struct sockaddr *)&addr, &len);
    if (cfd == -1)
    {
        perror("accept");
        std::cout << "无法连接到客户端" << std::endl;
        return tcpSocket(cfd);
    }
    char ip[32];
    std::cout << "连接到客户端ip：" << inet_ntop(AF_INET, &addr.sin_addr.s_addr, ip, sizeof(ip)) << "，端口：" << ntohs(addr.sin_port) << std::endl;
    return tcpSocket(cfd);
}
