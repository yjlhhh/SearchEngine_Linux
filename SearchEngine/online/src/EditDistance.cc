#include "EditDistance.hpp"

using std::string;

size_t nBytesCode(const char ch)
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
  
std::size_t length(const std::string &str)
{
	std::size_t ilen = 0;
	for(std::size_t idx = 0; idx != str.size(); ++idx)
	{
		int nBytes = nBytesCode(str[idx]);
		idx += (nBytes - 1);
		++ilen;
	}
	return ilen;
}

//从第from个字符开始截取len个字符
std::string mysubstr(const std::string & s, int from, int len)
{
    int n = length(s);
    int pos;
    int pos2;
    int beg = 0, end = 0;
    int cnt = 0;
    for(pos = 0; pos < s.size();) {
        int nBytes = nBytesCode(s[pos]);
        ++cnt;
        if(cnt == from) {
            beg = pos;//找到第from个字符的位置
            pos2 = pos + nBytes;//记录下一个字符的第一个字节的位置
            break;
        }
        pos += nBytes;
    }

    string ret;
    if(from + len >= n) {
        ret = s.substr(pos);
    } else {
        cnt = 0;
        for(; pos2 < s.size(); ) {
            int nBytes = nBytesCode(s[pos2]);
            ++cnt;
            if(cnt == len) {
                end = pos2 + nBytes;
                break;
            }
            pos2 += nBytes;
        }
        int bytes = end - beg;
        ret = s.substr(pos, bytes);
    }
    return ret;
}

int triple_min(const int &a, const int &b, const int &c)
{
	return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

int editDistance(const std::string & lhs, const std::string &rhs)
{//计算最小编辑距离-包括处理中英文
	size_t lhs_len = length(lhs);
	size_t rhs_len = length(rhs);
	int editDist[lhs_len + 1][rhs_len + 1];
	for(size_t idx = 0; idx <= lhs_len; ++idx)
	{
		editDist[idx][0] = idx;
	}

	for(size_t idx = 0; idx <= rhs_len; ++idx)
	{
		editDist[0][idx] = idx;
	}
	
	std::string sublhs, subrhs;
	for(std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i, ++lhs_idx)
	{
		size_t nBytes = nBytesCode(lhs[lhs_idx]);
		sublhs = lhs.substr(lhs_idx, nBytes);
		lhs_idx += (nBytes - 1);

		for(std::size_t dist_j = 1, rhs_idx = 0; dist_j <= rhs_len; ++dist_j, ++rhs_idx)
		{
			nBytes = nBytesCode(rhs[rhs_idx]);
			subrhs = rhs.substr(rhs_idx, nBytes);
			rhs_idx += (nBytes - 1);
			if(sublhs == subrhs)
			{
				editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j - 1];
			}
			else
			{
				editDist[dist_i][dist_j] = triple_min(
					editDist[dist_i][dist_j - 1] + 1,
					editDist[dist_i - 1][dist_j] + 1,
					editDist[dist_i - 1][dist_j - 1] + 1);
			}
		}
	}
	return editDist[lhs_len][rhs_len];
}
