#include "Threadpool.hpp"

#include <unistd.h>
#include <iostream>

using std::cout;
using std::endl;


namespace wd
{

Threadpool::Threadpool(int threadNum, int queSize)
: _threadNum(threadNum)
, _queSize(queSize)
, _threads()
, _taskque(_queSize)
, _isExit(false)
{
    _threads.reserve(_threadNum);   
}

void Threadpool::start()
{
    for(int i = 0; i < _threadNum; ++i) {
        //因为子线程要执行的是线程池中的doTask方法，所以需要通过std::bind函数
        //适配器加工之后，让其满足函数类型为 void() 即可
        //由于doTask方法是一个非静态成员函数，它有一个隐含的this指针, 需要提前
        //进行绑定
        unique_ptr<Thread> up(new Thread(std::bind(&Threadpool::doTask, this)));
        _threads.push_back(std::move(up));
    }

    //让每一个子线程运行起来
    for(auto & pthread : _threads) {
        pthread->start();
    }
}

//充当的是生产者的角色,往任务队列中存放任务
void Threadpool::addTask(Task && task)
{
    _taskque.push(task);
}

//每一个子线程不断地从任务队列中获取任务并执行
void Threadpool::doTask()
{
    while(!_isExit) {
        //Task是一个std::function<void()> 函数对象
        Task task = _taskque.pop();
        if(task) {
            task();//函数对象直接调用
        }
    }
}

void Threadpool::stop()
{
    //先确保任务队列中的任务都被执行完毕了
    while(!_taskque.empty()){
        sleep(1);
    }
    //当子线程执行任务的速度过快时，还没来得及将_isExit设置为true时，
    //每一个子线程都已经阻塞在任务队列的pop方法之上，这样会导致子线程
    //无法正常退出，此时需要调用任务队列的wakeup方法，让每个子线程在
    //任务队列为空时，也能退出来, 这样就可以让整个线程池退出了

    //让每一个子线程都停下来即可
    _isExit = true;
    _taskque.wakeup();
    for(auto & pthread : _threads) {
        pthread->join();
    }
}

}//end of namespace wd
