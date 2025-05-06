#ifndef __WD_TCPCONNECTION_H__
#define __WD_TCPCONNECTION_H__

#include "EventLoop.hpp"
#include "Noncopyable.hpp"
#include "Socket.hpp"
#include "InetAddress.hpp"
#include "SocketIO.hpp"
#include "DataPacket.hpp"

#include <functional>
#include <memory>
#include <string>
using std::string;

namespace wd
{

class EventLoop;
class TcpConnection;
using TcpConnectionPtr=shared_ptr<TcpConnection>;
using TcpConnecitonCallback=std::function<void(TcpConnectionPtr)>;

class TcpConnection
: Noncopyable
, public std::enable_shared_from_this<TcpConnection>
{   //通过this指针获取本对象的智能指针shared_ptr
public:
	TcpConnection(int fd, EventLoop * p);
	~TcpConnection();

	string receiveLine();
    void receivePacket(DataPacket & packet);
	void send(const string & msg);
    void sendInLoop(const string & msg);//在计算线程调用

	string toString() const;//获取五元组信息
	void shutdown();

    void setAllCallbacks(const TcpConnecitonCallback & cb1, 
                         const TcpConnecitonCallback & cb2,
                         const TcpConnecitonCallback & cb3)
    {   
        _onConnectionCb = cb1;//完成复制操作
        _onMessageCb = cb2;  
        _onCloseCb = cb3;
    }

    void handleConnectionCallback();
    void handleMessageCallback();
    void handleCloseCallback();
    bool isClosed();
private:
	InetAddress getLocalAddr(int fd);
	InetAddress getPeerAddr(int fd);
private:
	Socket _sock;
	SocketIO _socketIo;
	InetAddress _localAddr;
	InetAddress _peerAddr;
	bool _isShutdwonWrite;//是否要主动关闭连接
    EventLoop * _loop;
                          
    TcpConnecitonCallback _onConnectionCb;
    TcpConnecitonCallback _onMessageCb;
    TcpConnecitonCallback _onCloseCb;
};

}//end of namespace wd

#endif
