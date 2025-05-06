#ifndef __Threadpool_H__
#define __Threadpool_H__

#include "TaskQueue.hpp"
#include "Thread.hpp"
#include <vector>
#include <memory>
#include <functional>

using std::vector;
using std::unique_ptr;

using Task=std::function<void()>;

namespace wd
{

//class Thread;
class Threadpool
{
public:
    Threadpool(int threadNum, int queSize);
    void start();//启动线程池
    void stop();//停止线程池
    void addTask(Task && t);//添加任务的接口
private:
    void doTask();//每一个子线程要执行的


private:
    int _threadNum;
    int _queSize;
    vector<unique_ptr<Thread>> _threads;
    TaskQueue _taskque;
    bool _isExit;
};
}//end of namespace wd
#endif

