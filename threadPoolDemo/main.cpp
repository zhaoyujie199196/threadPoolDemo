#include "mainwindow.h"
#include <QApplication>
#include <threadpool.h>
#include "threadqueue.h"

class Task
{
public:
    Task(int i)
    {
        m_index = i;
    }

    void run()
    {
        std::cout<<"run Task "<<m_index<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

protected:
    int m_index = 0;
};

int main(int argc, char *argv[])
{
    std::shared_ptr<ThreadPool<std::shared_ptr<Task>>> pThreadPool(new ThreadPool<std::shared_ptr<Task>>(4));
    pThreadPool->init();

    for (int i = 0; i < 100; ++i)
    {
        std::shared_ptr<Task> pTask(new Task(i));
        pThreadPool->addTask(pTask);
        //std::this_thread::sleep_for(std::chrono::milliseconds(45));
    }

    pThreadPool->close();
}
