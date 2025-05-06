#include "KeyRecommander.hpp"
#include "Configuration.hpp"
#include "Mylogger.hpp"
#include "EditDistance.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>

#include  <nlohmann/json.hpp>
#include <unordered_set>

using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;
using std::priority_queue;


KeyRecommander * KeyRecommander::_pInstance = getInstance();

KeyRecommander::KeyRecommander()
{
    init();
}

void KeyRecommander::init()
{
    LogInfo("loading dict files");
    wd::Configuration * p = wd::Configuration::getInstance();
    string endict = p->getConfigMap()["en_dict"];
    string enindex = p->getConfigMap()["en_index"];
    string cndict = p->getConfigMap()["cn_dict"];
    string cnindex = p->getConfigMap()["cn_index"];

    readDict(endict, _vecEndict);
    readIndex(enindex, _enIndex);
    readDict(cndict, _vecCndict);
    readIndex(cnindex, _cnIndex);
    showEnDict();
    showEnIndex();
}

void KeyRecommander::readDict(const string & filename, vector<pair<string,int>> & vec)
{
    ifstream ifs(filename);
    if(!ifs.good()) {
        LogError("ifstream open file %s error", filename.c_str());
        return;
    }
    string line;
    string key;
    int value;
    while(getline(ifs, line)) {
        istringstream iss(line);
        iss >> key >> value;
        vec.push_back(std::make_pair(key, value));
    }
    ifs.close();
}
    
void KeyRecommander::readIndex(const string & filename, map<string, set<int>> & m)
{
    ifstream ifs(filename);
    if(!ifs.good()) {
        LogError("ifstream open file %s error", filename.c_str());
        return;
    }
    string line;
    while(getline(ifs, line)) {
        istringstream iss(line);
        string key;
        iss >> key;
        int value;
        while(iss >> value) {
            m[key].insert(value);
        }
    }

    ifs.close();
}

string KeyRecommander::doQuery(const string & sought) const
{
    cout << "KeyRecommander::doQuery() is running" << endl;

	priority_queue<MyResult, vector<MyResult>, MyCompare> resultQue;
    
    //query in index lib
    string ch;
	for(int idx = 0; idx != sought.size();)
	{
		size_t nBytes = nBytesCode(sought[idx]);
		ch = sought.substr(idx, nBytes);
		idx += nBytes;
        if(nBytes == 1) {//查英文索引
            auto it = _enIndex.find(ch);
            if(it != _enIndex.end()) {
                statistic(sought, it->second, _vecEndict, resultQue);
            }
        } else if (nBytes > 1){//查中文索引
            auto cit = _cnIndex.find(ch);
            if(cit != _cnIndex.end()) {
                statistic(sought, cit->second, _vecCndict, resultQue);
            }
        }
    }
    string result = createJson(resultQue);
    return result;
}

void KeyRecommander::statistic(const string & sought, const set<int> & s, 
                const vector<pair<string, int>> & dict,
               priority_queue<MyResult, vector<MyResult>, MyCompare> & que) const
{
    unordered_set<int> hasStatisticNumbers;
    auto iter = s.begin();
	for(; iter != s.end(); ++iter)
	{
        int num = *iter;
        if(hasStatisticNumbers.count(num) > 0) {
            //已经统计过的就直接跳过
            continue;
        }
		string rhsWord = dict[num].first;
		int idist = editDistance(sought, rhsWord);//进行最小编辑距离的计算
        hasStatisticNumbers.insert(num);
		if(idist < 5) {
			MyResult res;
			res._word = rhsWord;
			res._iFreq = dict[num].second;
			res._iDist = idist;
			que.push(res);
		}
	}
}

string KeyRecommander::createJson(priority_queue<MyResult, vector<MyResult>, MyCompare> & que) const
{
    using nlohmann::json;
    json root;
    json arr;
    if(que.empty()) {
        string result("sorry, similar word not found!");
        arr.push_back(result);
    } else {
        unordered_set<string> words;

        int n = que.size();
        int cnt = 0;
        for(int  i = 0; i < n; ++i) {
            MyResult result = que.top();
            que.pop();
            if(words.count(result._word) > 0) {
                continue;
            }
            arr.push_back(result._word);
            words.insert(result._word);
            if(cnt == 10) {//最多获取10个关键词
                break;
            }
            ++cnt;
        }
    }
    root["keywords"] = arr;
    return root.dump();
}
