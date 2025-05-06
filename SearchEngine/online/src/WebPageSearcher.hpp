#ifndef _WD_WEBPAGESEARCHER_H_
#define _WD_WEBPAGESEARCHER_H_

#include "WebPage.hpp"
#include "SplitTool.hpp"

#include <vector>
#include <map>
#include <set>
#include <memory>
#include <string>

using std::vector;
using std::map;
using std::unordered_map;
using std::set;
using std::string;
using std::pair;
using std::make_pair;
using std::unique_ptr;
using namespace wd;

namespace wd
{

class WebPageSearcher
{
public:
    static WebPageSearcher * getInstance();

	string doQuery(const string & str);
private:
	WebPageSearcher();
    void initSplitTool();
    void initStopWords();
	void loadLibrary();

	vector<double> getQueryWordsWeightVector(vector<string> & queryWords);
	
	bool executeQuery(const vector<string> & queryWords, vector<pair<int, vector<double>>> & resultVec);
	
	string createJson(const vector<int> & docIdVec, const vector<string> & queryWords);

	string returnNoAnswer();
private:
    unique_ptr<SplitTool>                           _splitTool;//分词工具
	unordered_map<int, WebPage>                     _pageLib;
	unordered_map<int, pair<int, int> >             _offsetLib;
	unordered_map<string, set<pair<int, double>>>   _invertIndexTable;
    unordered_set<string>                           _cnStopWords;//中文停用词
    static WebPageSearcher *                        _pInstance;
};

} // end of namespce wd
#endif
