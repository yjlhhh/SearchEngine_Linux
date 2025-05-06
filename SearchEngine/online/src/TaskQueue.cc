#include "TaskQueue.hpp"
#include "MutexLock.hpp"
#include <iostream>

using std::cout;
using std::endl;


namespace wd
{

TaskQueue::TaskQueue(int sz)
: _queSize(sz)
, _que()
, _mutex()
, _cond(_mutex)
, _flag(true)
{}

bool TaskQueue::empty() const
{
    return _que.size() == 0;
}

bool TaskQueue::full() const
{
    return _que.size() == _queSize;
}

//运行在生产者线程
void TaskQueue::push(ElemType e)
{
    //_mutex.lock();
    //autoLock是一个局部对象，当push函数执行结束时
    //一定会调用析构函数, 这样也可以完成解锁操作
    MutexLockGuard autoLock(_mutex);
    //当队列已经满了的情况，需要进行等待
    while(full()) {
        _cond.wait();
    }

    //队列中已经有空间了，可以存放数据了
    _que.push(e);
    //_mutex.unlock();//希望该语句一定要执行
    //通知消费者取数据
    _cond.notify();
}

//运行在消费者线程
ElemType TaskQueue::pop()
{
    MutexLockGuard autoLock(_mutex);
    //当队列为空时，需要进行等待
    while(_flag && empty()) {
        _cond.wait();
    }

    if(_flag) {
        //队列中有数据，那就需要取出数据
        ElemType ret = _que.front();
        _que.pop();//取走队头的元素

        //通知生产者放数据
        _cond.notify();
        return ret;
    } else {
        return NULL;
    }
}

void TaskQueue::wakeup()
{
    _flag = false;
    _cond.notifyAll();
}

}//end of namespace wd
