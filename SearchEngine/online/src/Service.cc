#include "Service.hpp"
#include "KeyRecommander.hpp"
#include "WebPageSearcher.hpp"
#include "Mylogger.hpp"
#include <iostream>

using std::cout;
using std::endl;

void Mytask::process() 
{

    //进行业务逻辑处理   
    int id = _data._id;
    switch(id) {
    case 1: //关键字推荐
        doKeyRecommand();   break;
    case 2: //网页搜索
        doWebSearch();      break;
    default:break;
    }
}

void Mytask::doKeyRecommand()
{
    LogInfo("doKeyRecommand()");
    //关键字推荐
    string key = _data._msg;
    //得到结果集
    string result = KeyRecommander::getInstance()->doQuery(key);
    //交给IO线程去发送
    if(!result.empty())
      _conn->sendInLoop(result);
}

void Mytask::doWebSearch()
{
    LogInfo("doWebSearch()");
    string key = _data._msg;
    //执行网页搜索
    string result =  wd::WebPageSearcher::getInstance()->doQuery(key);
    cout << "result.size():" << result.size() << endl;
    if(!result.empty()) {
        _conn->sendInLoop(result);
    }
}
