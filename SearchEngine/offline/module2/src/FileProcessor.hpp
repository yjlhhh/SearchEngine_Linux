#ifndef _WD_FILEPROCESSOR_H_
#define _WD_FILEPROCESSOR_H_


#include <string>
#include <vector>
using std::string;
using std::vector;

namespace wd
{
class Configuration;

class FileProcessor
{
public:
	FileProcessor(vector<string> & pages)
    : _vecPages(pages)
    {}

	void process(const string & rssFilename);
private:

private:
    vector<string> & _vecPages;
};

}// end of namespace wd
#endif
