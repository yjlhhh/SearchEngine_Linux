#ifndef __MutexLock_H__
#define __MutexLock_H__

#include <pthread.h>

namespace wd
{

class MutexLock
{
public:
    MutexLock();
    ~MutexLock();

    void lock();
    void unlock();
    pthread_mutex_t * getMutexPtr() {   return &_mutex;}

private:
    pthread_mutex_t _mutex;
};

class MutexLockGuard
{
public:
    MutexLockGuard(MutexLock & m)
    : _mutex(m)
    {
        _mutex.lock();
    }

    ~MutexLockGuard() {
        _mutex.unlock();
    }

private:
    MutexLock & _mutex;
};


}//end of namespace wd


#endif

