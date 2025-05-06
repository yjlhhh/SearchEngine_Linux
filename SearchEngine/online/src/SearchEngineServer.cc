#include "SearchEngineServer.hpp"
#include "DataPacket.hpp"
#include "Mylogger.hpp"
#include "Service.hpp"

void SearchEngineServer::onConnection(wd::TcpConnectionPtr conn)
{   //获取连接的信息
    LogInfo("%s has connected.", conn->toString().c_str());
}

void SearchEngineServer::onMessage(wd::TcpConnectionPtr conn)
{   //消息到达时的处理方式
    wd::DataPacket data;
    conn->receivePacket(data);
    //将消息封装成一个任务交给线程池进行处理
    Mytask task(data, conn);
    _threadpool.addTask(std::bind(&Mytask::process, task));
}

void SearchEngineServer::onClose(wd::TcpConnectionPtr conn)
{   //连接断开时的处理
    cout << conn->toString() << " has closed.\n";
    LogInfo("%s has closed.", conn->toString().c_str());
}
