#include "DictProducer.hpp"
#include "Configuration.hpp"
#include "Mylogger.hpp"
#include "SplitTool.hpp"

#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include <cctype>
#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <algorithm>


DictProducer::DictProducer()
{
    string filename1 = wd::Configuration::getInstance()->getConfigMap()["en_stop_words"];
    LogInfo("loading english stop words, file %s", filename1.c_str());
    initStopWords(_enStopWords, filename1);
    string filename2 = wd::Configuration::getInstance()->getConfigMap()["zh_stop_words"];
    LogInfo("loading chinese stop words, file %s", filename2.c_str());
    initStopWords(_cnStopWords, filename2);
    //showEnStopWords();
    showCnStopWords();
}

void DictProducer::initStopWords(unordered_set<string> & s, const string & filename)
{
    ifstream ifs(filename);
    if(!ifs) {
        LogError("ifsteram open file %s error", filename.c_str());
        return;
    }

    //停用词是一行一个，所以只需要定义一个word，然后插入hash表即可
    string word;
    while(getline(ifs, word)) {
        word.pop_back();//去除停用词末尾的'\r'
        s.insert(word);
    }
    ifs.close();
}
    

void DictProducer::showEnStopWords() const
{
    auto it = _enStopWords.begin();
    ++it;
    string str = *it;

    cout << ">>" << str << ", size:" << str.size() << endl;
    cout << "english stop words ' size:" << _enStopWords.size() << endl;
}

void DictProducer::showCnStopWords() const
{
    auto it = _cnStopWords.begin();
    ++it;
    string str = *it;

    cout << ">>" << str << ", size:" << str.size() << endl;
    cout << "chinese stop words ' size:" << _cnStopWords.size() << endl;
}

void DictProducer::buildEnDict()
{
    //字符串 "en_yuliao"是配置文件中的key
    string enfile = wd::Configuration::getInstance()->getConfigMap()["en_yuliao"];
    ifstream ifs(enfile);
    if(!ifs) {
        cout << "ifstream error," << enfile << " file does not exist" << endl;
        return;
    }
    LogInfo("reading en_yuliao %s", enfile.c_str());

    string line;
    while(getline(ifs, line))
    {
        istringstream iss(line);
        string word;
        while(iss >> word)
        {
            preprocessWord(word);
            if(word == string()) continue;//过滤掉空串
            //在停用词集中查找word
            int cnt = _enStopWords.count(word);
            if(cnt == 1) {
                //如果出现在了停用词集中，就不统计了
                continue;
            } else {
                //否则，就进行统计
                ++_endict[word];
            }
        }
    }
    ifs.close();

    //统计完毕，再将map中的元素存储到vector中
    copy(_endict.begin(), _endict.end(), back_inserter(_vecEnDict));
    LogInfo("reading en_yuliao %s is over", enfile.c_str());
}

void DictProducer::buildEnIndex()
{
    LogInfo("building english index");
    //遍历英文词典中的每一个单词，截取每一个英文字符
    //比如hello在数组中的下标为110
    //则分别截取h, e, l, o四个字符, 将其作为key，下标放在set中
    //添加到map<string, set<int>> _enIndex 中
    for(int i = 0; i < _vecEnDict.size(); ++i) {
        string word = _vecEnDict[i].first;
        for(int j = 0; j < word.size(); ++j) {
            string ch(1, word[j]);//将char类型转换为string类型
            _enIndex[ch].insert(i);
        }
    }
    //双重循环后，就结束了
}


void DictProducer::preprocessWord(string & word)
{
    for(int i = 0; i < word.size(); ++i) {
        if(!isalpha(word[i])) {
            word = string();
            return;
        }
        if(isupper(word[i])) {
            word[i] = tolower(word[i]);
        }
    }
}

void DictProducer::buildCnDict() {
    initSplitTool();//初始化中文分词库
    getCnfiles();//获取所有的中文语料文件名
    showCnFiles();//打印中文语料文件名
    
    //processFile(_cnfiles[0]);//测试一篇文章
    for(auto & filename : _cnfiles) {
        processFile(filename);//对每一篇文章进行分词
    }

    //showCnDict();
    copy(_cndict.begin(), _cndict.end(), back_inserter(_vecCnDict));
    LogInfo("spliting chines words is over");
}

void DictProducer::buildCnIndex()//构建中文词典索引
{
    /* LogInfo("building chinese index"); */
    //遍历中文词典中的每一个单词，截取每一个字符
    //比如"中国人"在数组中的下标为110
    //则分别截取'中'，'国'，'人' 3个字符, 将其作为key，下标放在set中
    //添加到map<string, set<int>> _cnIndex 中
    for(int i = 0; i < _vecCnDict.size(); ++i) {
        string word = _vecCnDict[i].first;
        for(int j = 0; j < word.size(); ++j) {
            int len = nBytesCode(word[j]);//获取一个字符占据的字节数
            string ch = word.substr(j, len);//截取一个字符
            _cnIndex[ch].insert(i);
            j += (len -1);
        }
    }
}

void DictProducer::initSplitTool()
{
    wd::Configuration * p = wd::Configuration::getInstance();
    string jiebaDictPath =  p->getConfigMap()["jieba_dict_path"];
    string jiebaHmmPath = p->getConfigMap()["jieba_hmm_path"];
    string jiebaUserDictPath = p->getConfigMap()["jieba_user_dict_path"];
    string jieabaIdfPath = p->getConfigMap()["jieba_idf_path"];
    string jieabaStopWordPath = p->getConfigMap()["jieba_stop_word_path"];
    //处理中文时才真正加载中文分词库
    _splitTool.reset(new Cppjieba(
            jiebaDictPath,
            jiebaHmmPath,
            jiebaUserDictPath,
            jieabaIdfPath,
            jieabaStopWordPath));
    LogInfo("loading cppjieba is over");
}

void DictProducer::processFile(const string & filename)
{
    ifstream ifs(filename);
    if(!ifs.good()) {
        LogError("open file %s error", filename.c_str());
        return;
    }

    //读取每一篇文章的内容
    ifs.seekg(0, std::ios::end);//偏移到文件的末尾
    size_t length = ifs.tellg();//获取文件的长度
    ifs.seekg(0, std::ios::beg);//偏移到文件的开始为止
    char * pbuf = new char[length+1]();
    ifs.read(pbuf, length);//一次性读取文件的所有内容
    string content(pbuf, length);//得到文件内容的string版本

    delete [] pbuf;//回收
    ifs.close();//关闭文件流
    
    //对中文内容进行分词
    vector<string> words;
    _splitTool->cut(content, words);//执行分词操作,分词结果存入words中

    //对每一个词进行统计操作
    for(const auto & word : words) {
        //先判断该词是否在停用词中，如果在，就抛弃掉，不统计
        filterStopWords(word, _cnStopWords);
        filterStopWords(word, _enStopWords);
    }
}

void DictProducer::filterStopWords(const string & w, const unordered_set<string> & s)
{
    int cnt = s.count(w);
    if(cnt == 1) {
        return;//在停用词集中, 直接跳过，不统计
    } else {
        ++_cndict[w];//不在停用词集中，就加入map中进行统计
    }
}

void DictProducer::showCnDict() const
{
    for(auto & word : _cndict) {
        cout << word.first << " " << word.second << endl;
    }
}

void DictProducer::showCnFiles() const
{
    for(auto & filename : _cnfiles) {
        cout << filename << endl;
    }
    cout << endl;
}

void DictProducer::storeCnDict() const
{
    string filename = wd::Configuration::getInstance()->getConfigMap()["cn_dict"];
    ofstream ofs(filename);
    if(!ofs) {
        ofs << "ofstream open file " << filename << " error" << endl;
        return ;
    }

    for(auto & elem : _vecCnDict) {
        ofs << elem.first << " " << elem.second << endl;
    }
    ofs.close();
    LogInfo("store chinese dict in file %s", filename.c_str());
}
    

void DictProducer::storeCnIndex() const//存储中文词典索引
{
    string filename = wd::Configuration::getInstance()->getConfigMap()["cn_index"];
    ofstream ofs(filename);
    if(!ofs) {
        ofs << "ofstream open file " << filename << " error" << endl;
        return ;
    }

    //双重循环，写入索引文件，格式为
    //key1 idx1 idx2 idx3
    //key2 idx11 idx22 idx33
    //依次类推
    for(auto & elem : _cnIndex) {
        ofs << elem.first;
        const set<int> & s = elem.second;
        for(auto & idx : s) {
            ofs << " " << idx;
        }
        ofs << endl;
    }

    ofs.close();
    LogInfo("store chinese index in file %s", filename.c_str());
}

void DictProducer::storeEnDict() const
{
    string filename = wd::Configuration::getInstance()->getConfigMap()["en_dict"];
    ofstream ofs(filename);
    if(!ofs) {
        ofs << "ofstream open file " << filename << " error" << endl;
        return ;
    }

    for(auto & elem : _vecEnDict) {
        ofs << elem.first << " " << elem.second << endl;
    }
    ofs.close();
    LogInfo("store english dict in file %s", filename.c_str());
}


void DictProducer::storeEnIndex() const
{
    string filename = wd::Configuration::getInstance()->getConfigMap()["en_index"];
    ofstream ofs(filename);
    if(!ofs) {
        ofs << "ofstream open file " << filename << " error" << endl;
        return ;
    }

    //双重循环，写入索引文件，格式为
    //key1 idx1 idx2 idx3
    //key2 idx11 idx22 idx33
    //依次类推
    for(auto & elem : _enIndex) {
        ofs << elem.first;
        const set<int> & s = elem.second;
        for(auto & idx : s) {
            ofs << " " << idx;
        }
        ofs << endl;
    }

    ofs.close();
    LogInfo("store english index in file %s", filename.c_str());
}

void DictProducer::showEnDict() const
{
    for( auto & elem : _endict) {
        cout << elem.first << " " << elem.second << endl;
    }
}


//获取所有的中文语料文件名
void DictProducer::getCnfiles()
{
    //获取存放中文语料的目录
    string dir = wd::Configuration::getInstance()->getConfigMap()["cn_yuliao"];
    DIR * pDir = opendir(dir.c_str());
    if(!pDir) {
        perror("opendir");
        LogError("open directory %s  error", dir.c_str());
        return;
    }
    struct dirent *p;
    while((p = readdir(pDir)) !=0) {
        //排除.和..文件夹
        if(strcmp(p->d_name, ".") !=0 && strcmp(p->d_name, "..")!= 0) {
            _cnfiles.push_back(dir + "/" + p->d_name);
        }
    }
    closedir(pDir);
    LogInfo("get all chinese file name over");
}

size_t DictProducer::nBytesCode(const char ch)
{//1000 0000
 //1xxx xxxx
	if(ch & (1 << 7))
	{
		int nBytes = 1;
		for(int idx = 0; idx != 6; ++idx)
		{
			if(ch & (1 << (6 - idx)))
			{
				++nBytes;	
			}
			else
				break;
		}
		return nBytes;
	}
	return 1;
}  
