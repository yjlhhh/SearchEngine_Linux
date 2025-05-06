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

namespace wd
{

class WebPage
{
	friend bool operator == (const WebPage & lhs, const WebPage & rhs); 
	friend bool operator < (const WebPage & lhs, const WebPage & rhs);
public:
	const static int TOPK_NUMBER = 20;

	WebPage(const string & doc, const unordered_set<string> & stopWords, SplitTool * tool);

	string summary(const vector<string> & queryWords) const;

	int getDocId() const {	return _docId;	}

	string getTitle()const {	return _docTitle;	}

	string getUrl()const {	return _docUrl;	}

	const map<string, int> & getWordsMap() const{	return _wordsMap;	}
private:
	void processDoc(const string & doc);

	void calcTopK(vector<string> & wordsVec, int k);

private:
    SplitTool * _psplitTool; 
    const unordered_set<string> & _cnStopWords;
	int    _docId;        //文档id
	string _docTitle;     //文档标题
	string _docUrl;		  //文档URL
	string _docContent;	  //文档内容
	string _docSummary;   //文档摘要，需要自动生成，不是确定的

	vector<string>   _topWords;
	map<string, int> _wordsMap; // 存储文档的所有词(去停用词之后)和该项词的词频
};

}// end of namespace wd

#endif
