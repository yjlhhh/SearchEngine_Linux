#include "TcpServer.hpp"
#include "Threadpool.hpp"

#include <unistd.h>
#include <iostream>
using std::cout;
using std::endl;


class SearchEngineServer
{
public:
    SearchEngineServer(const string & ip, unsigned short port, int threadNum, int size)
    : _threadpool(threadNum, size)
    , _server(ip, port)
    {}

    void start() {
        using namespace std::placeholders;
        _threadpool.start();
        _server.setAllCallbacks(
            std::bind(&SearchEngineServer::onConnection, this, _1),
            std::bind(&SearchEngineServer::onMessage, this, _1),
            std::bind(&SearchEngineServer::onClose, this, _1));
        _server.start();
    }

    void onConnection(wd::TcpConnectionPtr conn);
    void onMessage(wd::TcpConnectionPtr conn);
    void onClose(wd::TcpConnectionPtr conn);

private:
    wd::Threadpool _threadpool;
    wd::TcpServer _server;
};

