#include "SearchEngineServer.hpp"
#include "Configuration.hpp"

int testSearchEngine()
{
    string ip = wd::Configuration::getInstance()->getConfigMap()["ip"];
    string port = wd::Configuration::getInstance()->getConfigMap()["port"];
    string threadNum = wd::Configuration::getInstance()->getConfigMap()["thread_num"];
    string queSize = wd::Configuration::getInstance()->getConfigMap()["queue_size"];
    SearchEngineServer server(ip, 
        std::stoi(port), 
        std::stoi(threadNum), 
        std::stoi(queSize));

    server.start();

    return 0;
}

