#include "tcpserver.h"
#include "ThreadPool.h"

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
    auto &pool = ThreadPool::getInstance();
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return -1;
    }
    tcpServer lfd;
    int res = lfd.setListen(std::stoi(argv[1]));
    if (res == -1)
    {
        return -1;
    }
    while (1)
    {
        // 阻塞等待客户端来连接，返回一个通信套接字
        tcpSocket client = lfd.acceptConnect();
        if (!client.isValid())
        {
            continue;
        }
        auto cfd_ptr = std::make_shared<tcpSocket>(std::move(client));
        std::cout << "有新的客户端连接到服务器了..." << std::endl;
        pool.addTask(worker, cfd_ptr);
    }
    return 0;
}
