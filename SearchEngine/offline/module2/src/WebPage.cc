#include "WebPage.hpp"
#include "SplitTool.hpp"

#include <stdlib.h>
#include <iostream>
#include <queue>
#include <algorithm>

using std::cout;
using std::endl;
using std::priority_queue;
using std::pair;
using std::make_pair;


namespace wd
{

struct WordFreqCompare
{
	bool operator()(const pair<string, int> & left, const pair<string, int> & right)
	{
		if(left.second < right.second)
		{	return true;	}
		else if(left.second == right.second && left.first > right.first)
		{	return true;	}
		else
		{	return false;	}
	}
};


WebPage::WebPage(const string & doc, unordered_set<string> & stopWords, SplitTool * tool)
: _doc(doc)
, _cnStopWords(stopWords)
, _psplitTool(tool)
{
	//cout << "WebPage()" << endl;
	_topWords.reserve(TOPK_NUMBER);
	processDoc(doc);
}


void WebPage::processDoc(const string & doc)
{
	string docIdHead = "<docid>";
	string docIdTail = "</docid>";
	string docUrlHead = "<docurl>";
	string docUrlTail = "</docurl>";
	string docTitleHead = "<doctitle>";
	string docTitleTail = "</doctitle>";
	string docContentHead = "<doccontent>";
	string docContentTail = "</doccontent>";

	//提取网页的docid
	int bpos = doc.find(docIdHead);
	int epos = doc.find(docIdTail);
	string docId = doc.substr(bpos + docIdHead.size(), 
					   epos - bpos - docIdHead.size());
	_docId = atoi(docId.c_str());

	//提取网页的title
	bpos = doc.find(docTitleHead);
	epos = doc.find(docTitleTail);
	_docTitle = doc.substr(bpos + docTitleHead.size(), 
					epos - bpos - docTitleHead.size());

    //提取网页的url
    bpos = doc.find(docUrlHead);
    epos = doc.find(docUrlTail);
    _docUrl = doc.substr(bpos + docUrlHead.size(),
                  epos - bpos - docUrlHead.size());

	//cout << "========" << endl << _docTitle << endl;
	//提取网页的content
	bpos = doc.find(docContentHead);
	epos = doc.find(docContentTail);
	_docContent = doc.substr(bpos + docContentHead.size(),
					epos - bpos - docContentHead.size());

	//cout << "========" << endl << _docContent << endl;


	//中文分词
	vector<string> wordsVec;
    _psplitTool->cut(_docContent, wordsVec);
	calcTopK(wordsVec, TOPK_NUMBER);
}


void WebPage::calcTopK(vector<string> & wordsVec, int k)
{
	for(auto iter = wordsVec.begin(); iter != wordsVec.end(); ++iter)
	{
		auto sit = _cnStopWords.find(*iter);
		if(sit == _cnStopWords.end())
		{
			++_wordsMap[*iter];
		}
	}

	
	priority_queue<pair<string, int>, vector<pair<string, int> >, WordFreqCompare>
		wordFreqQue(_wordsMap.begin(), _wordsMap.end());

	while(!wordFreqQue.empty())
	{
		string top = wordFreqQue.top().first;
		wordFreqQue.pop();
		if(top.size() == 1 && (static_cast<unsigned int>(top[0]) == 10 ||
			static_cast<unsigned int>(top[0]) == 13))
		{	continue;	}

		_topWords.push_back(top);
		if(_topWords.size() >= TOPK_NUMBER)
		{
			break;
		}
	}


#if 0
	for(auto mit : _wordsMap)
	{
		cout << mit.first << "\t" << mit.second << std::endl;	
	}
	cout << endl;

	for(auto word : _topWords)
	{
		cout << word << "\t" << word.size() << "\t" << static_cast<unsigned int>(word[0]) << std::endl;
	}
#endif
}

// 判断两篇文档是否相同
bool operator == (const WebPage & lhs, const WebPage & rhs) 
{
	int commNum = 0;
	auto lIter = lhs._topWords.begin();
	for(;lIter != lhs._topWords.end(); ++lIter)
	{
		commNum += std::count(rhs._topWords.begin(), rhs._topWords.end(), *lIter);
	}
	
	int lhsNum = lhs._topWords.size();
	int rhsNum = rhs._topWords.size();
	int totalNum = lhsNum < rhsNum ? lhsNum : rhsNum;

	if( static_cast<double>(commNum) / totalNum > 0.75 )
	{	return true;	}
	else 
	{	return false;	}
}

//对文档按照docId进行排序
bool operator < (const WebPage & lhs, const WebPage & rhs)
{
	if(lhs._docId < rhs._docId)
		return true;
	else 
		return false;
}

}// end of namespace wd
