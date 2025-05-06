#ifndef _WD_DIRSCAN_H_
#define _WD_DIRSCAN_H_

#include <string>
#include <vector>

using std::vector;
using std::string;

namespace wd
{

const int kFileNo = 1000;

class DirScanner
{
public:
	DirScanner();

	vector<string> & files();
private:
	void traverseFiles(const string & dirName);

private:
	vector<string> _vecFiles;
};

} // end of namespace wd
#endif
