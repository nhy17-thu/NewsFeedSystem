#ifndef NEWSFEEDSYSTEM_HASHTABLE_H
#define NEWSFEEDSYSTEM_HASHTABLE_H
#include "Queue.h"
#include "CharString.h"

#define hashSize 24593 //仿照SGI STL，取一质数（但开链法哈希长度其实不一定需为质数）

class __hashTable_node {
    friend class hashTable;
private:
    Queue<CharString> val;
    void push(const std::string& strIn);
    bool has(const CharString& target);
};

class hashTable {   //采用开链法（separate chaining），避免线性探测
private:
    __hashTable_node buckets[hashSize];
	unsigned int num_elements = 0;

public:
    bool contained (const std::string& strIn);   //存在则返回下标，不存在则返回-1
    void insert (const std::string& strIn); //按GB2312，对首字符进行hash中文分词
	unsigned int size();
};


#endif //NEWSFEEDSYSTEM_HASHTABLE_H
