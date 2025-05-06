#ifndef __TcpServer_H__
#define __TcpServer_H__

#include "Acceptor.hpp"
#include "EventLoop.hpp"
#include "TcpConnection.hpp"

namespace wd {

class TcpServer
{
public:
    TcpServer(const string & ip, unsigned short port);
    void start();
    void stop();
    void setAllCallbacks(TcpConnectionCallback && cb1,
                         TcpConnectionCallback && cb2,
                         TcpConnectionCallback && cb3)
    {   
        _loop.setAllCallbacks(std::move(cb1),
                              std::move(cb2),
                              std::move(cb3));
    }

private:
    Acceptor _acceptor;
    EventLoop _loop;
};

}//end of namespace wd


#endif

