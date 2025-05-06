#ifndef __Condition_H__
#define __Condition_H__

#include <pthread.h>

namespace wd
{

class MutexLock;//类的前向声明
class Condition
{
public:
    Condition(MutexLock & m);
    ~Condition();

    void wait();
    void notify();
    void notifyAll();

private:
    pthread_cond_t _cond;
    MutexLock & _mutex;
};



}//end of namespace wd


#endif

