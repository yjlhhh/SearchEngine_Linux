#include "Thread.hpp"
#include <string.h>
#include <iostream>

using std::cout;
using std::endl;

namespace wd
{

void Thread::start()
{
    //第四个参数要传递this指针
    int ret = pthread_create(&_pthid, NULL, start_routine, this);
    if(ret != 0) {
        fprintf(stderr, "pthread_create:%s\n", strerror(ret));
        return;
    }
    cout << "sub thread id:" << _pthid << endl; 
}

//静态成员函数
void * Thread::start_routine(void * arg)
{
    //线程的入口函数
    //它要运行的就是线程的任务
    Thread * pthread = static_cast<Thread*>(arg);
    if(pthread) {
        pthread->_cb();//这里多态的体现
    }
    return NULL;
}

void Thread::join() 
{
    int ret = pthread_join(_pthid, NULL);
    if(ret != 0) {
        fprintf(stderr, "pthead_join:%s\n", strerror(ret));
    }
}
}//end of namespace wd
