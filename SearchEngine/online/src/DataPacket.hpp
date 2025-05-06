#ifndef __DataPacket_H__
#define __DataPacket_H__

#include <string>

using std::string;

namespace wd
{

struct DataPacket
{
    DataPacket()
    : _id(0), _len(0), _msg()
    {}

    int _id;
    int _len;
    string _msg;
};


}//end of namespace wd


#endif

