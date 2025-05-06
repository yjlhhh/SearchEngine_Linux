#include "Configuration.hpp"
#include "PageLib.hpp"
#include "DirScanner.hpp"
#include "FileProcessor.hpp"

#include <iostream>
#include <fstream>
using std::map;

namespace wd
{
PageLib::PageLib()
    : _dirScanner(),
	  _fileProcessor(_vecPages)
{}


void PageLib::create()
{
	vector<string> & vecFiles = _dirScanner.files();
	for(size_t idx = 0; idx != vecFiles.size(); ++idx)
	{
		_fileProcessor.process(vecFiles[idx]);
	}
}

void PageLib::store()
{
	//需要读取配置信息
	map<string, string> & confMap = Configuration::getInstance()->getConfigMap();
	string pageLibPath = confMap["ripepage"];
	string offsetLibPath = confMap["offset"];

	ofstream ofsPage(pageLibPath.c_str());
	ofstream ofsOffset(offsetLibPath.c_str());
	if(!(ofsPage.good() && ofsOffset.good()))
	{
		cout << "ofstream open error!" << endl;
		return;
	}

	for(size_t idx = 0; idx != _vecPages.size(); ++idx)
	{
		int id = idx + 1;
		int length = _vecPages[idx].size();
		ofstream::pos_type offset = ofsPage.tellp();
		ofsPage << _vecPages[idx];

		ofsOffset << id << '\t' << offset << '\t' << length << '\n';
	}

	ofsPage.close();
	ofsOffset.close();
}

}//end of namespace wd
