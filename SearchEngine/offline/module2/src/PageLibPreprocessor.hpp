#ifndef _WD_PAGE_LIB_PREPROCESSOR_H_
#define _WD_PAGE_LIB_PREPROCESSOR_H_


#include "WebPage.hpp"
//#include "WordSegmentation.hpp"
#include "SplitTool.hpp"

#include <map>
#include <set>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::map;
using std::set;
using std::unique_ptr;
using std::unordered_set;
using std::unordered_map;

using namespace wd;

namespace wd
{

class PageLibPreprocessor
{
public:
	PageLibPreprocessor();

	void doProcess();
private:
    void initSplitTool();
    void initStopWords();
	void readInfoFromFile();

	void cutRedundantPages();

	void buildInvertIndexTable();

	void storeOnDisk();

private:
    unordered_set<string>                            _cnStopWords;
    unique_ptr<SplitTool>                            _splitTool;
	vector<WebPage>                                  _pageLib;
	unordered_map<int, pair<int, int> >              _offsetLib;
	unordered_map<string, vector<pair<int, double>>> _invertIndexTable;
};


}// end of namespace wd


#endif
