#include <string>
#include <fstream>
#include <thread>
#include "tcpsocket.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << "<ip> <port>" << std::endl;
        return -1;
    }
    tcpSocket cfd = tcpSocket();
    int res = cfd.connectToServer(argv[1], std::stoi(argv[2]));
    if (res == -1)
    {
        return -1;
    }
    std::ifstream file("../message.txt");
    if (!file)
    {
        std::cout << "打开文件失败" << std::endl;
        return -1;
    }
    std::string s;
    while(std::getline(file, s))
    {
        std::cout << "读取一行：" << s << std::endl;
        cfd.sendMsg(s);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}