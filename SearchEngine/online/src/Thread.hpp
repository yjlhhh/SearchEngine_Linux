#ifndef __Thread_H__
#define __Thread_H__


#include <pthread.h>
#include <functional>

namespace wd
{

using ThreadCallback=std::function<void()>;

class Thread
{
public:
    //在构造函数中注册函数对象
    Thread(ThreadCallback && cb)
    : _pthid(0)
    , _cb(std::move(cb))
    {}

    ~Thread() {}

    void start();//开启子线程的运行
    void join();

private:
    static void * start_routine(void*);

    //交给派生类实现的任务

private:
    pthread_t _pthid;
    ThreadCallback _cb;
};

}//end of namespace wd

#endif

