#ifndef _DICTPRODUCER_H
#define _DICTPRODUCER_H

#include "SplitTool.hpp"

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <unordered_set>

using namespace std;

class DictProducer
{
public:
	DictProducer();

    void buildEnDict();//构建英文词典
    void buildEnIndex();//构建英文词典索引
	void buildCnDict();//构建中文词典
    void buildCnIndex();//构建中文词典索引

	void showCnFiles() const;
	void showCnDict() const;
    void showEnDict() const;
    void showEnStopWords() const;
    void showCnStopWords() const;

    void storeEnDict() const;//存储英文词典
    void storeEnIndex() const;//存储英文词典索引
    void storeCnDict() const;//存储中文词典
    void storeCnIndex() const;//存储中文词典索引

private:
    void preprocessWord(string & word);//处理每一个英文单词
    //加载停用词集
    void initStopWords(unordered_set<string> & s, const string & filename);
    //针对于中文过滤停用词集
    void filterStopWords(const string & w, const unordered_set<string> & s);
    void initSplitTool();//加载中文分词库
	void getCnfiles();//获取中文语料的文件名字
    void processFile(const string & filename);//对某一篇中文文章进行分词
    
    //求取一个字符占据的字节数
    //ch: 一个字符的第一个字节
    static size_t nBytesCode(const char ch);
private:
	vector<string> _cnfiles;//存储中文的每篇文章
    map<string, int> _cndict;//统计时使用
	map<string, int> _endict;//统计时使用
    vector<pair<string, int>> _vecCnDict;//计算索引文件时使用
    vector<pair<string, int>> _vecEnDict;//计算索引文件时使用
    unordered_set<string> _enStopWords;//英文停用词   
    unordered_set<string> _cnStopWords;//中文停用词
    map<string, set<int>> _enIndex;//英文词典索引
    map<string, set<int>> _cnIndex;//中文词典索引
    unique_ptr<SplitTool> _splitTool;//中文分词工具   
};

#endif // end DICTPRODUCER_H
