
#include "DirScanner.hpp"
#include "Configuration.hpp"
#include "Mylogger.hpp"
//#include "GlobalDefine.hpp"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <iostream>

namespace wd
{

DirScanner::DirScanner()
{
	_vecFiles.reserve(kFileNo);
    Configuration * p = Configuration::getInstance();
	string dirName = p->getConfigMap()["yuliao"];
	traverseFiles(dirName);
}

vector<string> & DirScanner::files()
{
	return _vecFiles;
}

void DirScanner::traverseFiles(const string & dir) {

    DIR * pDir = opendir(dir.c_str());
    if(!pDir) {
        perror("opendir");
        LogError("open directory %s  error", dir.c_str());
        return;
    }
    struct dirent *p;
    while((p = readdir(pDir)) !=0) {
        //排除.和..文件夹
        if(strcmp(p->d_name, ".") !=0 && strcmp(p->d_name, "..")!= 0) {
            _vecFiles.push_back(dir + "/" + p->d_name);
        }
    }
    closedir(pDir);
    LogInfo("get all rss file name over");
}


} // end of namespace wd
