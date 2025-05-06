#include <yjl.h>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;
using std::cin;

int main()
{
    //1. 客户端创建套接字
    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(cfd, -1, "socket");
    //2. 连接服务器
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8000);
    serveraddr.sin_addr.s_addr = inet_addr("192.168.145.136");
    int ret = connect(cfd, 
                      (const struct sockaddr *)&serveraddr,
                      sizeof(serveraddr));
    ERROR_CHECK(ret, -1, "connect");
    cout << "conn has connected.\n";

    while(1)
    {
        cout << "\n>> input format: [1-2] [word] \n"
             << "1 for key recommander\n"
             << "2 for web searching.\n";
        int id = 0;
        string line;
        cin >> id;
        cin >> line;
        int len = line.length();
        send(cfd, &id, 4, 0);
        send(cfd, &len, 4, 0);
        send(cfd, line.c_str(), line.size(), 0);
        cout << "send " << 8 + len << " bytes." << endl;

        char buff[645535] = {0};
        recv(cfd, buff, sizeof(buff), 0);
        cout << ">> recv from server:\n" << buff << endl;
    }
    close(cfd);
    return 0;
}
