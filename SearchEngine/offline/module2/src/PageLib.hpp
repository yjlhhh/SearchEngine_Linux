#ifndef _WD_PAGELIB_H_
#define _WD_PAGELIB_H_
#include "DirScanner.hpp"
#include "FileProcessor.hpp"

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace wd
{

class Configuration;
class DirScanner;
class FileProcessor;

class PageLib
{
public:
	PageLib();

	void create();
	void store();

private:
	vector<string> _vecPages;
	DirScanner _dirScanner;
	FileProcessor _fileProcessor;
};


}//end of namespace wd

#endif
