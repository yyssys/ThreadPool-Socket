#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <unordered_map>
#include <condition_variable>
#include <iostream>
#include <vector>
#include <atomic>
#include <functional>
#include <future>
#include <type_traits>
#include <stdexcept>

class ThreadPool
{
public:
    static ThreadPool &getInstance()
    {
        static ThreadPool pool;
        return pool;
    }

    int getTaskNum();
    int getKillNum();

    template <class F, class... Args>
    auto addTask(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using R = typename std::result_of<F(Args...)>::type;
        auto ptr = std::make_shared<std::packaged_task<R()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<R> res = ptr->get_future();
        std::unique_lock<std::mutex> lock(m_taskQueue);
        if (isShutdown)
            throw std::runtime_error("ThreadPool is shutdown");
        taskQueue.emplace([ptr]()
                          { (*ptr)(); });
        taskQueueNotEmpty.notify_one();
        return res;
    }

private:
    ~ThreadPool();
    void manager();
    void worker();

    ThreadPool(int min = 2, int max = std::thread::hardware_concurrency());
    // 单例模式删除拷贝/移动构造和拷贝/移动负值函数
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

    std::queue<std::function<void()>> taskQueue;                    // 任务队列
    std::unordered_map<std::thread::id, std::thread> workerThreads; // 工作线程
    std::thread manager_t;                                          // 管理者线程

    std::atomic<bool> isShutdown;    // 线程池状态
    std::atomic<int> aliveThreadNum; // 存活线程个数
    std::atomic<int> idleThreadNum;  // 空闲线程个数
    std::atomic<int> willKillNum;    // 将要销毁的线程个数

    std::vector<std::thread::id> killArray; // 要销毁的线程id

    std::mutex m_taskQueue; // 任务队列锁
    std::mutex m_killArray; // 线程销毁容器锁

    int minThreadNum;
    int maxThreadNum;

    std::condition_variable taskQueueNotEmpty; // 任务队列不空条件变量
};
