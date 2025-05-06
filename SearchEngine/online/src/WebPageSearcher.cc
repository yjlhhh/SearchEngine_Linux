#include "WebPageSearcher.hpp"
#include "Configuration.hpp"
#include "Mylogger.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

using std::cout;
using std::endl;
using std::ifstream;
using std::stringstream;

namespace wd
{

WebPageSearcher * WebPageSearcher::_pInstance = getInstance();
    
WebPageSearcher * WebPageSearcher::getInstance()
{
    if(nullptr == _pInstance) {
        _pInstance = new WebPageSearcher();
    }
    return _pInstance;
}

struct SimilarityCompare
{
	SimilarityCompare(vector<double> & base)
	: _base(base)
	{}


	bool operator()(const pair<int, vector<double> > & lhs, 
					const pair<int, vector<double> > & rhs)
	{	// 都与基准向量进行计算
		double lhsCrossProduct = 0;
		double rhsCrossProduct = 0;
		double lhsVectorLength = 0;
		double rhsVectorLength = 0;
		
		for(int index = 0; index != _base.size(); ++index)
		{
			lhsCrossProduct += (lhs.second)[index] * _base[index];
			rhsCrossProduct += (rhs.second)[index] * _base[index];
			lhsVectorLength += pow((lhs.second)[index], 2);
			rhsVectorLength += pow((rhs.second)[index], 2);
		}

		if(lhsCrossProduct / sqrt(lhsVectorLength) < rhsCrossProduct / sqrt(rhsVectorLength))
		{	return false;	}
		else
		{	return true;	}
	}

	vector<double> _base;
};


WebPageSearcher::WebPageSearcher()
{
    initSplitTool();   
    initStopWords();
	loadLibrary();
}

void WebPageSearcher::initStopWords()
{
    string filename = wd::Configuration::getInstance()->getConfigMap()["zh_stop_words"];
    ifstream ifs(filename);
    if(!ifs) {
        LogError("ifsteram open file %s error", filename.c_str());
        return;
    }

    //停用词是一行一个，所以只需要定义一个word，然后插入hash表即可
    string word;
    while(getline(ifs, word)) {
        word.pop_back();//去除停用词末尾的'\r'
        _cnStopWords.insert(word);
    }
    ifs.close();
}

void WebPageSearcher::initSplitTool()
{
    wd::Configuration * p = wd::Configuration::getInstance();
    string jiebaDictPath =  p->getConfigMap()["jieba_dict_path"];
    string jiebaHmmPath = p->getConfigMap()["jieba_hmm_path"];
    string jiebaUserDictPath = p->getConfigMap()["jieba_user_dict_path"];
    string jieabaIdfPath = p->getConfigMap()["jieba_idf_path"];
    string jieabaStopWordPath = p->getConfigMap()["jieba_stop_word_path"];
    //处理中文时才真正加载中文分词库
    _splitTool.reset(new Cppjieba(
            jiebaDictPath,
            jiebaHmmPath,
            jiebaUserDictPath,
            jieabaIdfPath,
            jieabaStopWordPath));
    LogInfo("loading cppjieba is over");
}

void WebPageSearcher::loadLibrary()
{
    wd::Configuration * p = wd::Configuration::getInstance();
    string weboffset = p->getConfigMap()["weboffset"];
    string webpages = p->getConfigMap()["webpages"];
	ifstream ifsOffset(weboffset);
	ifstream ifsPage(webpages);

	if(!(ifsOffset.good() && ifsPage.good()))
	{
		cout << "offset page lib ifstream open error!" << endl;
	}
	
	int docid, offset, length;
	string line;
	int cnt = 0;
	while(getline(ifsOffset, line))
	{
		stringstream ss(line);
		ss >> docid >> offset >> length;

		string doc;
		doc.resize(length, ' ');
		ifsPage.seekg(offset, ifsPage.beg);
		ifsPage.read(&*doc.begin(), length);

		WebPage webPage(doc, _cnStopWords, _splitTool.get());
		_pageLib.insert(make_pair(docid, webPage));

		_offsetLib.insert(make_pair(docid, make_pair(offset, length)));
//		if(++cnt == 3)
//			break;
	}
	ifsOffset.close();
	ifsPage.close();
#if 0
		for(auto item : _offsetLib)
		{
			cout << item.first << "\t" << item.second.first << "\t" << item.second.second << endl;
		}
#endif

	// load invertIndexTable
    string invertindex = wd::Configuration::getInstance()->getConfigMap()["invertindex"];
	ifstream ifsInvertIndex(invertindex);
	if(!ifsInvertIndex.good())
	{	cout << "invert index ifstream open error!" << endl;}

	string word;
	double weight;
	cnt = 0;
	while(getline(ifsInvertIndex, line))
	{
		stringstream ss(line);
		ss >> word;
		set<pair<int, double> > setID;
		while(ss >> docid >> weight)
		{
			setID.insert(make_pair(docid, weight));
		}
		_invertIndexTable.insert(make_pair(word, setID));

//		if(++cnt == 21)
//			break;
	}
	ifsInvertIndex.close();

	cout << "loadLibrary() end" << endl;

	//for debug
#if 0
	for(auto item : _invertIndexTable)
	{
		cout << item.first << "\t";
		for(auto sitem : item.second)
		{
			cout << sitem.first << "\t" << sitem.second << "\t";
		}
		cout << endl;
	}
#endif
}

string WebPageSearcher::doQuery(const string & str)
{
	//对输入字符串进行分词
	vector<string> queryWords;
	if(str.size() > 0)
	{
        _splitTool->cut(str, queryWords);
	}

	for(auto item : queryWords)
	{
		cout << item << '\n';
		auto uit = _invertIndexTable.find(item);
		if(uit == _invertIndexTable.end())
		{
			cout << "can not found " << item << endl;
			return returnNoAnswer();
		}
	}
	cout << endl;

	//计算每个词的权重
	vector<double> weightList = getQueryWordsWeightVector(queryWords);
	SimilarityCompare similarityCmp(weightList);

	//执行查询
	vector<pair<int, vector<double> > > resultVec;// int -> docid
	if(executeQuery(queryWords, resultVec))
	{
		stable_sort(resultVec.begin(), resultVec.end(), similarityCmp);
		vector<int> docIdVec;

		for(auto item : resultVec)
		{
			docIdVec.push_back(item.first);
		}

		return createJson(docIdVec, queryWords);

		//将查询结果封装成Json数据
	}
	else
	{
		return returnNoAnswer();
	}
}

string WebPageSearcher::returnNoAnswer()
{
    using nlohmann::json;
    
    json arr;
    json elem;
    elem["title"] = "404, not found";
    elem["summary"] = "i cannot find what you want";
    elem["url"] = "";
    arr.push_back(elem);

    json root;
    root["files"] = arr;
    string jstr = root.dump(2);
    return jstr;
}

vector<double> WebPageSearcher::getQueryWordsWeightVector(vector<string> & queryWords)
{
	//统计词频
	map<string, int> wordFreqMap;
	for(auto item : queryWords)
	{
		++wordFreqMap[item];
	}

	//计算权重
	vector<double> weightList;
	double weightSum = 0;
	int totalPageNum = _offsetLib.size();

	for(auto & item : queryWords)
	{
		int df = _invertIndexTable[item].size();
		double idf = log(static_cast<double>(totalPageNum) / df + 0.05) / log(2); 
		int tf = wordFreqMap[item];
		double w = idf * tf;
		weightSum += pow(w, 2);
		weightList.push_back(w);
	}

	//归一化
	for(auto & item : weightList)
	{
		item /= sqrt(weightSum);
	}

	cout << "weightList's elem: ";
	for(auto item : weightList)
	{
		cout << item << "\t";
	}
	cout << endl;
	return weightList;
}

//执行查询
bool WebPageSearcher::executeQuery(const vector<string> & queryWords, 
		vector<pair<int, vector<double> > > & resultVec)
{
	cout << "executeQuery()" << endl;
	if(queryWords.size() == 0)
	{
		cout <<"empty string not find" << endl;
		return false;
	}	

	typedef	set<pair<int, double> >::iterator setIter;
	vector<pair<setIter, int> > iterVec;//保存需要求取交集的迭代器
	int minIterNum = 0x7FFFFFFF;
	for(auto item : queryWords)
	{
		int sz = _invertIndexTable[item].size();
		if(sz == 0)
			return false;
		
		if(minIterNum > sz)
			minIterNum = sz;

		iterVec.push_back(make_pair(_invertIndexTable[item].begin(), 0));
	}
	cout << "minIterNum = " << minIterNum << endl;

	bool isExiting = false;
	while(!isExiting)
	{
		int idx = 0;
		for(; idx != iterVec.size() - 1; ++idx)
		{
			if((iterVec[idx].first)->first != iterVec[idx+1].first->first)
				break;
		}
		
		if(idx == iterVec.size() - 1)
		{	// 找到相同的docid
			vector<double> weightVec;
			int docid = iterVec[0].first->first;
			for(idx = 0; idx != iterVec.size(); ++idx)
			{
				weightVec.push_back(iterVec[idx].first->second);
				++(iterVec[idx].first);//相同时，将每一个迭代器++
				++(iterVec[idx].second);//同时记录迭代器++的次数
				if(iterVec[idx].second == minIterNum)
				{	isExiting = true;	}
			}
			resultVec.push_back(make_pair(docid, weightVec));
		}
		else
		{	//找到最小的docid，并将其所在的iter++
			int minDocId = 0x7FFFFFFF;
			int iterIdx;//保存minDocId的迭代器位置
			for(idx = 0; idx != iterVec.size(); ++idx)
			{
				if(iterVec[idx].first->first < minDocId)
				{
					minDocId = iterVec[idx].first->first;
					iterIdx = idx;
				}
			}
			
			++(iterVec[iterIdx].first);
			++(iterVec[iterIdx].second);
			if(iterVec[iterIdx].second == minIterNum)
			{	isExiting = true;	}
		}
	}
	return true;
}

string WebPageSearcher::createJson(const vector<int> & docIdVec, const vector<string> & queryWords)
{
    using nlohmann::json;
    json root;
    json arr;

	int cnt = 0;
	for(auto id : docIdVec)
	{
        auto it = _pageLib.find(id);
        if(it == _pageLib.end()) {
            continue;
        }
		string summary = it->second.summary(queryWords);
		string title = it->second.getTitle();
		string url = it->second.getUrl();

        json elem;
		elem["title"] = title;
		elem["summary"] = summary;
		elem["url"] = url;
		arr.push_back(elem);
		if(++cnt == 10)// 最多记录10条
			break;
	}

	root["files"] = arr;
    return root.dump(2);
}

}// end of namespace wd
