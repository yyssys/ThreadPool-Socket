#pragma once
#include "tcpsocket.h"
class tcpServer
{
public:
    tcpServer();
    int setListen(unsigned short int port);
    tcpSocket acceptConnect();

private:
    tcpServer(const tcpServer &) = delete;
    tcpServer &operator=(const tcpServer &) = delete;
    int lfd; // 监听套接字
};