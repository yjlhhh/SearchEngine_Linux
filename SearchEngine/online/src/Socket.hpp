#ifndef __WD_SOCKET_H__
#define __WD_SOCKET_H__

#include "Noncopyable.hpp"

namespace wd
{

class Socket
: Noncopyable  //���ഴ���Ķ��󲻿��Խ��и��ƣ�����������
{
public:
	Socket();
	explicit
	Socket(int fd);

	int fd() const;

	void shutdownWrite();//�����Ͽ����ӣ��ر�д��

	~Socket();

private:
	int _fd;
};

}//end of namespace wd


#endif
