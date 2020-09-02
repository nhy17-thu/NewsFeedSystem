#ifndef NEWSFEEDSYSTEM_HASHTABLE_H
#define NEWSFEEDSYSTEM_HASHTABLE_H
#include "Queue.h"
#include "CharString.h"

#define hashSize 24593 //����SGI STL��ȡһ����������������ϣ������ʵ��һ����Ϊ������

class __hashTable_node {
    friend class hashTable;
private:
    Queue<CharString> val;
    void push(const std::string& strIn);
    bool has(const CharString& target);
};

class hashTable {   //���ÿ�������separate chaining������������̽��
private:
    __hashTable_node buckets[hashSize];
	unsigned int num_elements = 0;

public:
    bool contained (const std::string& strIn);   //�����򷵻��±꣬�������򷵻�-1
    void insert (const std::string& strIn); //��GB2312�������ַ�����hash���ķִ�
	unsigned int size();
};


#endif //NEWSFEEDSYSTEM_HASHTABLE_H
