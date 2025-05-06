#ifndef __EventLoop_H__
#define __EventLoop_H__
#include "MutexLock.hpp"

#include <sys/epoll.h>

#include <memory>
#include <map>
#include <vector>
#include <functional>
using std::shared_ptr;
using std::map;
using std::vector;


namespace wd
{

class TcpConnection;
using TcpConnectionPtr=shared_ptr<TcpConnection>;
using TcpConnectionCallback=std::function<void(TcpConnectionPtr)>;
using Functor=std::function<void()>;

class Acceptor;
class EventLoop
{
public:
    EventLoop(Acceptor & acceptor);
    ~EventLoop();

    void loop();
    void unloop();
    void runInLoop(Functor && cb);//
    void setAllCallbacks(TcpConnectionCallback && cb1, 
                         TcpConnectionCallback && cb2,
                         TcpConnectionCallback && cb3)
    {
        _onConnectionCb = std::move(cb1);
        _onMessageCb = std::move(cb2);
        _onCloseCb = std::move(cb3);
    }
private:
    void waitEpollFd();//封装的是循环执行体
                       
    void handleNewConnection();
    void handleMessage(int fd);

    int createEpollFd();
    void addEpollReadEvent(int fd);
    void delEpollReadEvent(int fd);

    int createEventfd();
    void handleRead();
    void wakeup();
    void doPendingFunctors();

private:
    bool _isLooping;
    int _epfd;
    int _eventfd;
    Acceptor & _acceptor;
    map<int, TcpConnectionPtr> _conns;
    vector<struct epoll_event> _evtArr;
    MutexLock _mutex;
    vector<Functor> _pendingFunctors;

    //这三个函数对象为了传递给TcpConnection对象
    TcpConnectionCallback _onConnectionCb;
    TcpConnectionCallback _onMessageCb;
    TcpConnectionCallback _onCloseCb;

};

}//end of namespace wd



#endif

