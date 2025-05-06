#ifndef __KeyRecommander_H__
#define __KeyRecommander_H__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <iostream>

using std::string;
using std::vector;
using std::map;
using std::set;
using std::pair;
using std::cout;
using std::endl;
using std::priority_queue;

struct MyResult
{
	string _word;
	int _iFreq;
	int _iDist;
};

struct MyCompare
{
	bool operator()(const MyResult & lhs, const MyResult & rhs)
	{
		if(lhs._iDist > rhs._iDist)
		{	return true;	}
		else if(lhs._iDist == rhs._iDist &&
				lhs._iFreq < rhs._iFreq)
		{	return true;	}
		else if(lhs._iDist == rhs._iDist &&
				lhs._iFreq == rhs._iFreq && 
				lhs._word > rhs._word)
		{	return true;	}
		else
			return false;
	}
};

class KeyRecommander
{
public:
    static KeyRecommander * getInstance()
    {
        if(nullptr== _pInstance) {
            _pInstance = new KeyRecommander();
        }
        return _pInstance;
    }

    void showEnDict() const
    {
       cout << "endict'size:" << _vecEndict.size() << endl;
    }

    void showEnIndex() const
    {
       cout << "enindex'size:" << _enIndex.size() << endl;
    }

    string doQuery(const string & sought) const;

private:
    KeyRecommander();
    void init();

    void readDict(const string & filename, vector<pair<string,int>> & vec);
    void readIndex(const string & filename, map<string, set<int>> & m);

    void statistic(const string & sought, const set<int> & s, 
                   const vector<pair<string, int>> & dict,
                   priority_queue<MyResult, vector<MyResult>, MyCompare> & que) const;
    string createJson(priority_queue<MyResult, vector<MyResult>, MyCompare> & que) const;

private:
    vector<pair<string, int>> _vecEndict;
    vector<pair<string, int>> _vecCndict;
    map<string, set<int>> _enIndex;
    map<string, set<int>> _cnIndex;
    static KeyRecommander * _pInstance;
};

#endif

