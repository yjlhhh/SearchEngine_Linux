#include "TcpConnection.hpp"
#include "DataPacket.hpp"
#include "InetAddress.hpp"
#include "Mylogger.hpp"

#include <errno.h>
#include <sys/socket.h>

#include <sstream>
#include <iostream>
using std::cout;
using std::endl;

namespace wd
{
TcpConnection::TcpConnection(int fd, EventLoop * p)
: _sock(fd)
, _socketIo(fd)
, _localAddr(getLocalAddr(fd))
, _peerAddr(getPeerAddr(fd))
, _isShutdwonWrite(false)
, _loop(p)
{
}

TcpConnection::~TcpConnection()
{
	if(!_isShutdwonWrite) {
		shutdown();
	}
}

string TcpConnection::receiveLine()
{
	char buff[65536] = {0};
    //这里使用的是readline,所以消息的边界为'\n'
	_socketIo.readline(buff, sizeof(buff));
	return string(buff);
}

void TcpConnection::receivePacket(DataPacket & data)
{
    int msgid = -1;
    int msglen = -1;
    char * pbuf = nullptr;
    _socketIo.readn((char*)&msgid, sizeof(int));//读取消息ID
    _socketIo.readn((char*)&msglen, sizeof(int));//读取消息长度
    LogInfo("msgid: %d, msglen: %d", msgid, msglen);
    if(msglen > 0) {
        pbuf = new char[msglen + 1]();
        _socketIo.readn(pbuf, msglen);//读取消息内容
        LogInfo("msgcontent: %s", pbuf);
        data._msg.assign(pbuf, msglen);//将内容放入string中
        data._id = msgid;
        data._len = msglen;
    }
    delete [] pbuf;
    pbuf = nullptr;
}
	
void TcpConnection::send(const string & msg)
{
	_socketIo.writen(msg.c_str(), msg.size());
}

void TcpConnection::sendInLoop(const string & msg)//在计算线程调用
{
    if(_loop) {
        //将send以及要发送的消息封装成一个function<void()>
        _loop->runInLoop(std::bind(&TcpConnection::send, this, msg));
    }
}

void TcpConnection::shutdown()
{
	if(!_isShutdwonWrite) {
		_isShutdwonWrite = true;
		_sock.shutdownWrite();
	}
}

string TcpConnection::toString() const
{
	std::ostringstream oss;
	oss << "tcp " << _localAddr.ip() << ":" << _localAddr.port() << " --> "
		<< _peerAddr.ip() << ":" << _peerAddr.port();
	return oss.str();
}

bool TcpConnection::isClosed()
{
    int ret;
    do {
        char buff[20] = {0};
        ret = recv(_sock.fd(), buff, sizeof(buff), MSG_PEEK);
    }while(ret == -1 && errno == EINTR);
    return ret == 0;
}

void TcpConnection::handleConnectionCallback()
{
    if(_onConnectionCb) {
        //this --> shared_ptr<this>
        //通过this指针获取本对象的智能指针shared_ptr
        _onConnectionCb(shared_from_this());//函数对象调用时，需要传递shared_ptr的对象过去
    }
}

void TcpConnection::handleMessageCallback()
{
    if(_onMessageCb) {
        _onMessageCb(shared_from_this());
    }
}

void TcpConnection::handleCloseCallback()
{
    if(_onCloseCb) {
        _onCloseCb(shared_from_this());
    }
}


InetAddress TcpConnection::getLocalAddr(int fd)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(struct sockaddr);
	if(getsockname(_sock.fd(), (struct sockaddr*)&addr, &len) == -1) {
		perror("getsockname");
	}
	return InetAddress(addr);
}

InetAddress TcpConnection::getPeerAddr(int fd)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(struct sockaddr);
	if(getpeername(_sock.fd(), (struct sockaddr*)&addr, &len) == -1) {
		perror("getpeername");
	}
	return InetAddress(addr);
}

}//end of namespace wd
