#include "tcpsocket.h"

tcpSocket::tcpSocket()
{
    cfd = socket(AF_INET, SOCK_STREAM, 0);
}

tcpSocket::tcpSocket(int fd)
{
    cfd = fd;
}

tcpSocket::tcpSocket(tcpSocket &&other) noexcept
{
    cfd = other.cfd;
    other.cfd = -1;
}

tcpSocket &tcpSocket::operator=(tcpSocket &&other) noexcept
{
    if (this != &other)
    {
        if (cfd != -1)
            close(cfd);
        cfd = other.cfd;
        other.cfd = -1;
    }
    return *this;
}

int tcpSocket::connectToServer(const std::string ip, unsigned short int port)
{
    struct sockaddr_in addr{};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.data(), &addr.sin_addr.s_addr);

    int res = connect(cfd, (struct sockaddr *)&addr, sizeof(addr));
    if (res == -1)
    {
        perror("connect");
        return -1;
    }

    return res;
}

int tcpSocket::sendMsg(const std::string msg)
{
    int len = msg.size();
    int len_t = htonl(len);
    std::string s(msg.size() + 4, '\0');
    std::memcpy(&s[0], &len_t, 4);
    std::memcpy(&s[4], msg.data(), msg.size());
    int res = writen(s, s.size());
    return res;
}

std::string tcpSocket::recvMsg()
{
    std::string l = readn(4);
    if (l.size() < 4)
    {
        return std::string();
    }
    int len = 0;
    std::memcpy(&len, l.data(), 4);
    len = ntohl(len);
    std::cout << "接收数据长度：" << len << std::endl;
    std::string res = readn(len);
    return res;
}

tcpSocket::~tcpSocket()
{
    if (cfd != -1)
        close(cfd);
}

int tcpSocket::writen(const std::string s, int len)
{
    int left = len;
    const char *tmp = &s[0];
    while (left > 0)
    {
        int n = write(cfd, tmp, left);
        if (n == -1)
        {
            perror("write");
            return -1;
        }
        else if (n == 0)
        {
            std::cout << "断开连接" << std::endl;
            return -1;
        }
        left -= n;
        tmp += n;
    }
    return len;
}

std::string tcpSocket::readn(int len)
{
    std::string s(len, '\0');
    char *ptr = &s[0];
    int left = len;
    while (left > 0)
    {
        int n = read(cfd, ptr, left);
        if (n == -1)
        {
            perror("read");
            return std::string();
        }
        else if (n == 0)
        {
            return std::string();
        }
        left -= n;
        ptr += n;
    }
    return s;
}
