#include <arpa/inet.h>
#include "ThreadPool.h"
#include "tcpsocket.h"

int setListen(int lfd, unsigned short int port)
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

int acceptConnect(int lfd)
{
    struct sockaddr_in addr{};
    socklen_t len = sizeof(addr);
    int cfd = accept(lfd, (struct sockaddr *)&addr, &len);
    if (cfd == -1)
    {
        perror("accept");
        std::cout << "无法连接到客户端" << std::endl;
        return -1;
    }
    char ip[32];
    std::cout << "连接到客户端ip：" << inet_ntop(AF_INET, &addr.sin_addr.s_addr, ip, sizeof(ip)) << "，端口：" << ntohs(addr.sin_port) << std::endl;
    return cfd;
}

void worker(std::shared_ptr<tcpSocket> cfd)
{
    while (1)
    {
        std::string msg = cfd->recvMsg();
        if (msg.empty())
        {
            std::cout << "客户端断开了连接" << std::endl;
            break;
        }
        else
        {
            std::cout << "接收到：" << msg << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return -1;
    }

    // 创建监听套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    // 绑定并监听
    if (setListen(lfd, std::stoi(argv[1])) == -1)
    {
        return -1;
    }

    auto &pool = ThreadPool::getInstance();

    while (1)
    {
        // 阻塞等待客户端来连接，返回一个通信套接字
        int cfd = acceptConnect(lfd);
        if (cfd == -1)
        {
            continue;
        }
        auto cfd_ptr = std::make_shared<tcpSocket>(tcpSocket(cfd));
        std::cout << "有新的客户端连接到服务器了..." << std::endl;
        pool.addTask(worker, cfd_ptr);
    }
    return 0;
}
