#ifndef __SplitTool_H__
#define __SplitTool_H__
#include "../include/cppjieba/Jieba.hpp"

#include <vector>
#include <string>

using namespace std;
class SplitTool
{
public:
    virtual void cut(const string & str, vector<string> & vec)=0;
    virtual ~SplitTool() {}
};

class Cppjieba
: public SplitTool
{
public:
    Cppjieba(const string & dictPath,
             const string & modelPath,
             const string & userDictPath,
             const string & idfPath,
             const string & stopWordsPath)
    : _jieba(dictPath,
             modelPath,
             userDictPath,
             idfPath,
             stopWordsPath)
    {}

    void cut(const string & str, vector<string> & vec) override
    {   
        _jieba.CutAll(str, vec);
    }

private:
    cppjieba::Jieba _jieba;
};


#endif

