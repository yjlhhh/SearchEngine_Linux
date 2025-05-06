#ifndef __Service_H__
#define __Service_H__

#include "DataPacket.hpp"
#include "TcpConnection.hpp"
#include <string>

using std::string;

class Mytask
{
public:
    Mytask(const wd::DataPacket & d, wd::TcpConnectionPtr conn)
    : _data(d)
    , _conn(conn)
    {}

    void process();
private:
    void doKeyRecommand();
    void doWebSearch();
private:
    wd::DataPacket _data;
    wd::TcpConnectionPtr _conn;
};


#endif

