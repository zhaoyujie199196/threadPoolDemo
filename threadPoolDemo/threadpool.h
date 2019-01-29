#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <memory>
#include <vector>
#include <thread>
#include <type_traits>
#include "threadqueue.h"

//区分指针类型，对象，智能指针
template<class T>
struct RunHelper
{
    static void run(T t)
    {
        t.run();
    }
};

template<class T>
struct RunHelper<T *>
{
    static void run(T *t)
    {
        t->run();
    }
};

template<class T>
struct RunHelper<std::shared_ptr<T>>
{
    static void run(std::shared_ptr<T> t)
    {
        t->run();
    }
};

template <class T>
class ThreadPool
{
public:
    ThreadPool(int nThreadCount);
    ~ThreadPool();
    void init();
    void close();
    void addTask(const T &t);
    void addTask(T &&t);

protected:
    void run();

protected:
    std::shared_ptr<ThreadQueue<T>> m_queue;
    int m_nThreadCount;
    std::vector<std::shared_ptr<std::thread>> m_threadVec;
    bool m_bHasQuit = false;
};

template<class T>
ThreadPool<T>::ThreadPool(int nThreadCount)
    : m_nThreadCount(nThreadCount)
{

}

template<class T>
ThreadPool<T>::~ThreadPool()
{
    if (!m_bHasQuit)
        close();
    for (int i = 0; i < m_threadVec.size(); ++i)
    {
        m_threadVec.at(i)->join();
    }
    m_threadVec.clear();
}

template<class T>
void ThreadPool<T>::init()
{
    m_queue = std::shared_ptr<ThreadQueue<T>>(new ThreadQueue<T>(20));
    for (int i = 0; i < m_nThreadCount; ++i)
    {
        std::shared_ptr<std::thread> t(new std::thread(&ThreadPool::run, this));
        m_threadVec.push_back(t);
    }
}

template<class T>
void ThreadPool<T>::run()
{
    while (true)
    {
        bool bSuccess;
        T t = m_queue->take(bSuccess);
        if (!bSuccess)
            break;
        RunHelper<T>::run(t);
    }
}

template<class T>
void ThreadPool<T>::close()
{
    std::cout<<"ThreadPool close begin"<<std::endl;
    m_queue->close();
    m_bHasQuit = true;
}

template<class T>
void ThreadPool<T>::addTask(const T &t)
{
    m_queue->put(t);
}

template<class T>
void ThreadPool<T>::addTask(T &&t)
{
    m_queue->put(std::forward<T>(t));
}

#endif // THREADPOOL_H
