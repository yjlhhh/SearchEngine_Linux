#include "Configuration.hpp"
#include <string>
#include <iostream>

int testConfiguration()
{
    wd::Configuration::getInstance()->display();

    wd::Configuration * p = wd::Configuration::getInstance();
    
    std::string english_stop_words_path = p->getConfigMap()["en_stop_words"];
    std::cout << "stop words:" << english_stop_words_path << std::endl;

    return 0;
}

