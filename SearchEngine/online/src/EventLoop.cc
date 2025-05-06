#include "EventLoop.hpp"
#include "Acceptor.hpp"
#include "MutexLock.hpp"
#include "TcpConnection.hpp"

#include <string.h>
#include <unistd.h>
#include <sys/eventfd.h>

#include <iostream>

using std::cout;
using std::endl;


namespace wd
{

EventLoop::EventLoop(Acceptor & acceptor)
: _isLooping(false)
, _epfd(createEpollFd())
, _eventfd(createEventfd())
, _acceptor(acceptor)
, _evtArr(1024)
{
    //关注Acceptor的监听套接字
    addEpollReadEvent(_acceptor.fd());
    //关注_eventfd
    addEpollReadEvent(_eventfd);
    _pendingFunctors.reserve(10);
}

EventLoop::~EventLoop()
{
    close(_epfd);
}

void EventLoop::loop()
{
    _isLooping = true;
    while(_isLooping) {
        waitEpollFd();
    }
}
//loop函数和unloop函数要运行在不同的线程
void EventLoop::unloop()
{
    _isLooping = false;
}
    
//该函数运行在计算线程的
void EventLoop::runInLoop(Functor && cb)
{
    //将函数对象放入vector中
    //_pendingFunctors是一个临界资源, 此时必须加锁
    {
    MutexLockGuard autolock(_mutex);
    _pendingFunctors.push_back(std::move(cb));
    }

    //再做通知操作
    wakeup();
}


void EventLoop::waitEpollFd()
{
    int nready = -1;
    do {
        nready = epoll_wait(_epfd, &_evtArr[0], _evtArr.size(), -1);
    }while(nready == -1 && errno == EINTR);

    if(nready == -1) {
        perror("epoll_wait");
        return;
    } else if(nready == 0) {
        cout << "epoll timeout.\n";
    } else {
        //nready 大于0
        for(int i = 0; i < nready; ++i) {
            int fd = _evtArr[i].data.fd;
            if(fd == _acceptor.fd()) {
                //新连接的处理
                handleNewConnection();
            } else if(fd == _eventfd)  {
                //计算线程通知IO线程
                handleRead();
                doPendingFunctors();
            }
            else {
                //已经建立好的连接的处理
                handleMessage(fd);
            }
        }
    }
}

void EventLoop::handleNewConnection()
{
    int newFd = _acceptor.accept();
    addEpollReadEvent(newFd);//监听已经建立好的连接
    TcpConnectionPtr conn(new TcpConnection(newFd, this));
    //将TcpConnection对象放入map中
    //_conns[newFd] = sp;
    _conns.insert(std::make_pair(newFd, conn));
    conn->setAllCallbacks(_onConnectionCb,
                          _onMessageCb,
                          _onCloseCb);

    //当新连接建立时的事件处理器
    conn->handleConnectionCallback();
}

void EventLoop::handleMessage(int fd)
{
    //fd所代表的连接是否已经断开
    auto it = _conns.find(fd);
    if(it != _conns.end()) {
        //查找成功
        bool isClosed = it->second->isClosed();
        if(isClosed) {
            //执行连接断开时的事件处理器
            it->second->handleCloseCallback();
            delEpollReadEvent(fd);//从epoll的监听中删除掉
            _conns.erase(it);//从管理的map的删掉掉
        } else {
            //没有断开连接的情况, 执行消息到达时的事件处理器
            it->second->handleMessageCallback();
        }
    }
}


int EventLoop::createEpollFd()
{
    int fd = epoll_create1(0);
    if(fd < 0) {
        perror("epoll_create1");
    }
    return fd;
}

void EventLoop::addEpollReadEvent(int fd)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
    if(ret < 0) {
        perror("epoll_ctl");
    }
}

void EventLoop::delEpollReadEvent(int fd)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &ev);
    if(ret < 0) {
        perror("epoll_ctl");
    }
}

int EventLoop::createEventfd()
{
    int fd = eventfd(0, 0);
    if(fd < 0) {
        perror("eventfd");
    }
    return fd;
}

void EventLoop::handleRead()
{
    uint64_t howmany = 0;
    int ret = read(_eventfd, &howmany, sizeof(howmany));
    if(ret != sizeof(howmany)) {
        perror("read");
    }
}

//每一次调用就是往内核计数器上加1
void EventLoop::wakeup()
{
    uint64_t one = 1;
    int ret = write(_eventfd, &one, sizeof(one));
    if(ret != sizeof(one)) {
        perror("write");
    }
}

//运行在IO线程
void EventLoop::doPendingFunctors()
{
    cout << "doPendingFunctors()" << endl;
    //对_pendingFunctors的每一个元素进行遍历
    vector<Functor> tmp;
    tmp.reserve(10);
    {
    MutexLockGuard autolock(_mutex);
    tmp.swap(_pendingFunctors);//O(1)
    }

    //cout << "tmp.size:" << tmp.size() << endl;
    //经过了交换之后，_pendingFunctors就会解放出来
    //之后每一个函数对象的执行，与_pendingsFunctors无关的
    for(int i = 0; i < tmp.size(); ++i) {
        tmp[i]();
    }
}


}//end of namespace wd
