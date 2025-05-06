#ifndef _WD_WEB_PAGE_H_
#define _WD_WEB_PAGE_H_
#include "SplitTool.hpp"

#include <string>
#include <map>
#include <set>
#include <unordered_set>
#include <vector>

using std::string;
using std::map;
using std::vector;
using std::set;
using std::unordered_set;

namespace wd
{

class WebPage
{
	friend bool operator == (const WebPage & lhs, const WebPage & rhs); 
	friend bool operator < (const WebPage & lhs, const WebPage & rhs);
public:
	const static int TOPK_NUMBER = 10;

	WebPage(const string & doc, unordered_set<string> & stopWords, SplitTool * tool);

	int getDocId()
	{	return _docId;	}

	map<string, int> & getWordsMap()
	{	return _wordsMap;	}

	string getDoc()
	{	return _doc;	}

private:
	void processDoc(const string & doc);

	void calcTopK(vector<string> & wordsVec, int k);

private:
	string _doc;//从网页库中读取出来的带<doc>...</doc>这样的标签的数据
    SplitTool * _psplitTool;//分词工具
    unordered_set<string>  _cnStopWords;//中文停用词
	int    _docId;        //文档id
	string _docTitle;     //文档标题
	string _docUrl;		  //文档URL
	string _docContent;	  //文档内容

	vector<string>   _topWords;
	map<string, int> _wordsMap; // 存储文档的所有词(去停用词之后)和该项词的词频
};

}// end of namespace wd

#endif
