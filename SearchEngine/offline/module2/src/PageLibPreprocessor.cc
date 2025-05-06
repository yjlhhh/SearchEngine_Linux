#include "PageLibPreprocessor.hpp"
#include "Configuration.hpp"
#include "Mylogger.hpp"

#include <stdio.h>
#include <time.h>
#include <fstream>
#include <sstream>

using std::ifstream;
using std::ofstream;
using std::stringstream;

namespace wd
{

PageLibPreprocessor::PageLibPreprocessor()
{
    initStopWords();
    initSplitTool();
}

void PageLibPreprocessor::initStopWords()
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

void PageLibPreprocessor::initSplitTool()
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


void PageLibPreprocessor::doProcess()
{
	readInfoFromFile();
	time_t t1 = time(NULL);
	//cutRedundantPages();//(暂时未实现)网页去重
	buildInvertIndexTable();//创建倒排索引
	time_t t2 = time(NULL);
	printf("preprocess time: %ld min\n", (t2 - t1)/60);

	storeOnDisk();
	time_t t3 = time(NULL);
	printf("store time: %ld min\n", (t3 - t2)/60);
}

void PageLibPreprocessor::readInfoFromFile()
{
	map<string, string> &configMap = wd::Configuration::getInstance()->getConfigMap();
	string pageLibPath = configMap["ripepage"];
	string offsetLibPath = configMap["offset"];

	ifstream pageIfs(pageLibPath);
	ifstream offsetIfs(offsetLibPath);

	if((!pageIfs.good()) || (!offsetIfs.good()))
	{
        LogError("pagelib or offset lib open error");
        return;
	}

	string line;
	int docId, docOffset, docLen;
	while(getline(offsetIfs, line))
	{
		stringstream ss(line);
		ss >> docId >> docOffset >> docLen;
		
		string doc;
		doc.resize(docLen, ' ');
		pageIfs.seekg(docOffset, pageIfs.beg);
		pageIfs.read(&*doc.begin(), docLen);

		WebPage webPage(doc, _cnStopWords, _splitTool.get());
		_pageLib.push_back(webPage);

		_offsetLib.insert(std::make_pair(docId, std::make_pair(docOffset, docLen)));
	}
#if 0
	for(auto mit : _offsetLib)
	{
		cout << mit.first << "\t" << mit.second.first << "\t" << mit.second.second << endl;
	}
#endif
}

void PageLibPreprocessor::cutRedundantPages()
{
	for(size_t i = 0; i != _pageLib.size() - 1; ++i)
	{
		for(size_t j = i + 1; j != _pageLib.size(); ++j)
		{
			if(_pageLib[i] == _pageLib[j])
			{
				_pageLib[j] = _pageLib[_pageLib.size() - 1];
				_pageLib.pop_back();
				--j;
			}
		}
	}
}

void PageLibPreprocessor::buildInvertIndexTable()
{
	for(auto page : _pageLib)
	{
		map<string, int> & wordsMap = page.getWordsMap();
		for(auto wordFreq : wordsMap)
		{
			_invertIndexTable[wordFreq.first].push_back(std::make_pair(
					page.getDocId(), wordFreq.second));
		}
	}
	
	//计算每篇文档中的词的权重,并归一化
	map<int, double> weightSum;// 保存每一篇文档中所有词的权重平方和. int 代表docid

	int totalPageNum = _pageLib.size();
	for(auto & item : _invertIndexTable)
	{	
		int df = item.second.size();
		//求关键词item.first的逆文档频率
		double idf = log(static_cast<double>(totalPageNum)/ df + 0.05) / log(2);
		
		for(auto & sitem : item.second)
		{
			double weight = sitem.second * idf;

			weightSum[sitem.first] += pow(weight, 2);
			sitem.second = weight;
		}
	}

	for(auto & item : _invertIndexTable)
	{	//归一化处理
		for(auto & sitem : item.second)
		{
			sitem.second = sitem.second / sqrt(weightSum[sitem.first]);
		}
	}


#if 0 // for debug
	for(auto item : _invertIndexTable)
	{
		cout << item.first << "\t";
		for(auto sitem : item.second)
		{
			cout << sitem.first << "\t" << sitem.second <<  "\t";
		}
		cout << endl;
	}
#endif
}

void PageLibPreprocessor::storeOnDisk()
{
	sort(_pageLib.begin(), _pageLib.end());	

	ofstream ofsPageLib(wd::Configuration::getInstance()->getConfigMap()["webpages"]);
	ofstream ofsOffsetLib(wd::Configuration::getInstance()->getConfigMap()["weboffset"]);

	if( !ofsPageLib.good() || !ofsOffsetLib.good())
	{	
		cout << "new page or offset lib ofstream open error!" << endl;
	}

	for(auto & page : _pageLib)
	{
		int id = page.getDocId();
		int length = page.getDoc().size();
		ofstream::pos_type offset = ofsPageLib.tellp();
		ofsPageLib << page.getDoc();

		ofsOffsetLib << id << '\t' << offset << '\t' << length << '\n';
	}

	ofsPageLib.close();
	ofsOffsetLib.close();


	// invertIndexTable
	ofstream ofsInvertIndexTable(wd::Configuration::getInstance()->getConfigMap()["invertindex"]);
	if(!ofsInvertIndexTable.good())
	{
		cout << "invert index table ofstream open error!" << endl;
	}
	for(auto item : _invertIndexTable)
	{
		ofsInvertIndexTable << item.first << "\t";
		for(auto sitem : item.second)
		{
			ofsInvertIndexTable << sitem.first << "\t" << sitem.second <<  "\t";
		}
		ofsInvertIndexTable << endl;
	}
	ofsInvertIndexTable.close();
}

}// end of namespace wd
