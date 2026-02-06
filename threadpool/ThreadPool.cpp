#include "ThreadPool.h"

ThreadPool::ThreadPool(int min, int max) : minThreadNum(min), maxThreadNum(max), aliveThreadNum(min),
                                           idleThreadNum(min), willKillNum(0), isShutdown(false)
{
    // 创建工作者线程
    for (int i = 0; i < min; i++)
    {
        std::thread t(&ThreadPool::worker, this);
        workerThreads.emplace(t.get_id(), std::move(t));
    }
    // 创建管理者线程
    manager_t = std::thread(&ThreadPool::manager, this);
}

int ThreadPool::getTaskNum()
{
    std::lock_guard<std::mutex> lock(m_taskQueue);
    int num = taskQueue.size();
    return num;
}

int ThreadPool::getKillNum()
{
    std::lock_guard<std::mutex> lock(m_killArray);
    int num = killArray.size();
    return num;
}

ThreadPool::~ThreadPool()
{
    isShutdown = true;

    taskQueueNotEmpty.notify_all(); // 通知所有在阻塞等待的线程，告知他们线程池要关闭了。

    if (manager_t.joinable())
        manager_t.join();

    for (auto &it : workerThreads)
    {
        std::cout << "线程" << it.first << "因线程池退出而销毁" << std::endl;
        it.second.join();
    }
}

void ThreadPool::manager()
{
    while (!isShutdown)
    {
        std::cout << "线程总数：" << aliveThreadNum << "，其中空闲的" << idleThreadNum << "个" << std::endl;
        // 任务繁忙，创建新线程来执行任务
        if (idleThreadNum == 0 && aliveThreadNum < maxThreadNum && getTaskNum() > 1)
        {
            std::thread t(&ThreadPool::worker, this);
            std::cout << "任务过多，创建新线程" << t.get_id() << "来执行任务" << std::endl;
            idleThreadNum++;
            aliveThreadNum++;
            workerThreads.emplace(t.get_id(), std::move(t));
        }
        else if (idleThreadNum > aliveThreadNum / 2 && aliveThreadNum > minThreadNum) // 空闲线程太多，退出线程
        {
            willKillNum++;
            taskQueueNotEmpty.notify_one();
        }
        if (getKillNum() != 0)
        {
            std::lock_guard<std::mutex> lock(m_killArray);
            for (auto it : killArray)
            {
                auto i = workerThreads.find(it);
                if (i != workerThreads.end())
                {
                    i->second.join();
                    std::cout << "退出的线程" << i->first << "被销毁" << std::endl;
                    workerThreads.erase(i);
                }
            }
            killArray.clear();
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

void ThreadPool::worker()
{
    while (!isShutdown)
    {
        std::unique_lock<std::mutex> lock(m_taskQueue);
        taskQueueNotEmpty.wait(lock, [this]()
                               { return isShutdown || taskQueue.size() > 0 || willKillNum != 0; });
        if (isShutdown)
            return;
        if (willKillNum != 0)
        {
            willKillNum--;
            aliveThreadNum--;
            idleThreadNum--;
            std::cout << "线程" << std::this_thread::get_id() << "因空闲线程太多而退出..." << std::endl;
            std::lock_guard<std::mutex> lock(m_killArray);
            killArray.emplace_back(std::this_thread::get_id());
            return;
        }
        auto task = std::move(taskQueue.front());
        taskQueue.pop();
        lock.unlock();
        std::cout << "线程" << std::this_thread::get_id() << "取出一个任务执行中..." << std::endl;
        idleThreadNum--;
        task();
        idleThreadNum++;
    }
}
