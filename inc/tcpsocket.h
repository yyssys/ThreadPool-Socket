#pragma once
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <cstring>
class tcpSocket
{
public:
    tcpSocket();
    tcpSocket(int fd);
    tcpSocket(tcpSocket &&) noexcept;
    tcpSocket &operator=(tcpSocket &&other) noexcept;

    // 连接到服务器
    int connectToServer(const std::string ip, unsigned short int port);

    // 发送消息
    int sendMsg(const std::string msg);
    // 接收消息
    std::string recvMsg();
    bool isValid() const { return cfd != -1; }
    ~tcpSocket();

private:
    int writen(const std::string s, int len);
    std::string readn(int len);

    tcpSocket(const tcpSocket &) = delete;
    tcpSocket &operator=(const tcpSocket &) = delete;
    int cfd; // 通信套接字
};
