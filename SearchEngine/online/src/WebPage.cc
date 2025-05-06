#include "WebPage.hpp"
#include "EditDistance.hpp"

#include <math.h>
#include <stdlib.h>
#include <queue>
#include <algorithm>
#include <sstream>
#include <unordered_set>

using std::priority_queue;
using std::pair;
using std::istringstream;



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


WebPage::WebPage(const string & doc, const unordered_set<string> & stopWords, SplitTool * tool)
: _psplitTool(tool)
, _cnStopWords(stopWords)
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

	//提取文档的docid
	int bpos = doc.find(docIdHead);
	int epos = doc.find(docIdTail);
	string docId = doc.substr(bpos + docIdHead.size(), 
					epos - bpos - docIdHead.size());
	_docId = atoi(docId.c_str());

	//title
	bpos = doc.find(docTitleHead);
	epos = doc.find(docTitleTail);
	_docTitle = doc.substr(bpos + docTitleHead.size(), 
					epos - bpos - docTitleHead.size());

	//url
	bpos = doc.find(docUrlHead);
	epos = doc.find(docUrlTail);
	_docUrl = doc.substr(bpos + docUrlHead.size(), 
					epos - bpos - docUrlHead.size());
	//content
	bpos = doc.find(docContentHead);
	epos = doc.find(docContentTail);
	_docContent = doc.substr(bpos + docContentHead.size(),
					epos - bpos - docContentHead.size());

	//cout << "========" << endl << _docContent << endl;

	//分词
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

string WebPage::summary(const vector<string> & queryWords) const
{
	vector<string> summaryVec;

	istringstream iss(_docContent);
	string line;
	while(iss >> line)
	{
		for(auto & word : queryWords)
		{
			if(line.find(word) != string::npos)
			{
				summaryVec.push_back(line);
				break;
			}
		}

		if(summaryVec.size() >= 5)
		{	break;	}
	}

	string summary;
    int totalChar = 0;
	for(auto & line : summaryVec)
	{
        int tmp = totalChar;
        int n = length(line);//求取一行语句的字符个数
        totalChar += n;
        if(totalChar < 100) {
		    summary.append(line).append("\n");
        } else {//超过100个字符了
            int left = 100 - tmp;
            //截取left个字符
            string substr = mysubstr(line, 1, left);
            summary.append(substr).append("...");
            break;
        }
	}
	return summary;
}

}// end of namespace wd
