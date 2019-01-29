#ifndef THREADQUEUE_H
#define THREADQUEUE_H

#include <QList>
#include <iostream>
#include <mutex>
#include <condition_variable>

template <class T>
class ThreadQueue
{
public:
    ThreadQueue(int nMaxCount);
    void put(const T &t);
    void put(T &&t);
    T take(bool &bSuccess);
    void close();

protected:
    template <class F>
    void add(F &&t);
    bool notFull();
    bool notEmpty();

protected:
    std::mutex m_mutex;
    std::condition_variable m_NotEmptyCondition;
    std::condition_variable m_NotFullConditin;
    QList<T> m_queue;
    int m_nMaxCount = 0;
    bool m_bHasQuit = false;
};

template<class T>
ThreadQueue<T>::ThreadQueue(int nMaxCount)
    : m_nMaxCount(nMaxCount)
{

}

template <class T>
void ThreadQueue<T>::put(const T &t)
{
    add(t);
}

template <class T>
void ThreadQueue<T>::put(T &&t)
{
    add(std::forward<T>(t));
}

template <class T>
T ThreadQueue<T>::take(bool &bSuccess)
{
    std::unique_lock<std::mutex> locker(m_mutex);
    m_NotEmptyCondition.wait(locker, [this]{return notEmpty() || m_bHasQuit;});
    if (m_bHasQuit)
    {
        bSuccess = false;
        return std::move(T());
    }
    else
    {
        T t = m_queue.takeFirst();
        bSuccess = true;
        m_NotFullConditin.notify_one();
        return std::move(t);
    }
}

template <class T>
template <class F>
void ThreadQueue<T>::add(F &&t)
{
    std::unique_lock<std::mutex> locker(m_mutex);
    m_NotFullConditin.wait(locker, [this]{return notFull() || m_bHasQuit;});
    if (m_bHasQuit)
        return;
    m_queue.append(t);
    m_NotEmptyCondition.notify_one();
}

template <class T>
bool ThreadQueue<T>::notFull()
{
    return m_queue.size() < m_nMaxCount;
}

template <class T>
bool ThreadQueue<T>::notEmpty()
{
    return !m_queue.isEmpty();
}

template<class T>
void ThreadQueue<T>::close()
{
    std::unique_lock<std::mutex> locker(m_mutex);
    m_queue.clear();
    m_bHasQuit = true;
    m_NotEmptyCondition.notify_all();
    m_NotFullConditin.notify_all();
}

#endif // THREADQUEUE_H
