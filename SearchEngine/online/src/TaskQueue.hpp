#ifndef __TaskQueue_H__
#define __TaskQueue_H__
#include "MutexLock.hpp"
#include "Condition.hpp"
#include <queue>
#include <functional>

using std::queue;
namespace wd
{

using ElemType=std::function<void()>;

class TaskQueue
{
public:
    TaskQueue(int sz);
    bool full() const;
    bool empty() const;
    void push(ElemType e);
    ElemType pop();
    void wakeup();

private:
    int _queSize;
    queue<ElemType> _que;
    MutexLock _mutex;
    Condition _cond;
    bool _flag;
};

}//end of namespace wd


#endif

