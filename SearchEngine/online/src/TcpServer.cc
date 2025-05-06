#include "TcpServer.hpp"
#include <iostream>

using std::cout;
using std::endl;


namespace wd
{

TcpServer::TcpServer(const string & ip, unsigned short port)
: _acceptor(ip, port)
, _loop(_acceptor)
{
}

void TcpServer::start()
{
    _acceptor.ready();
    _loop.loop();
}

//stop与start要运行在不同的线程
void TcpServer::stop()
{
    _loop.unloop();
}

}//end of namespace wd
