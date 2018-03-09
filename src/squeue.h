//
// Created by jsszwc on 18-2-24.
//

#ifndef HTTPSERVER_SQUEUE_H
#define HTTPSERVER_SQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class Squeue
{
public:
    Squeue() = default;
    ~Squeue() = default;

    void clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        while(! m_que.empty())
            m_que.pop();
    }
    bool empty()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_que.empty();
    }

    int size()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_que.size();
    }
    bool front(T &value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_que.empty())
            return false;
        value = m_que.front();
        return true;
    }
    void push(T &value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_que.emplace(value);
        m_cond.notify_all();
    }
    bool pop(T &value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this](){
            return !m_que.empty();
        });
        if(m_que.empty())
            return false;

        value = m_que.front();
        m_que.pop();
        return true;
    }
    bool pop(T &value, int waitMs)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if(m_que.empty())
        {
            if(m_cond.wait_for(lock, std::chrono::microseconds(waitMs)) == std::cv_status::timeout)
                return false;
        }
        if(m_que.empty())
            return false;

        value = m_que.front();
        m_que.pop();
        return true;
    }

private:
    std::queue<T> m_que;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

#endif //HTTPSERVER_SQUEUE_H
