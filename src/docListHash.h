#ifndef NEWSFEEDSYSTEM_DOCLISTHASH_H
#define NEWSFEEDSYSTEM_DOCLISTHASH_H

#include "Queue.h"
#include "CharString.h"
#include "docList.h"

#define hashSize 24593 //仿照SGI STL，取一质数（但开链法哈希长度其实不一定需为质数）

/*
 * 用哈希表存储单词（即倒排文档）
 * 每个单词都有对应的ID和文档链表
 * 每个文档链表中记录了这个单词出现的文档和每个文档中单词出现的次数
 */

//以下是用哈希表实现的倒排索引
class __docHash_node {
    friend class docHash;
private:
    Queue<docList> words;        //words中存储不同的单词，而对于每一个单词，有docList记录其在不同文档中出现的次数
    void push(const docList& nodeIn);       //存入新单词
    bool has(const CharString& target);     //是否已有单词
};

class docHash {   //采用开链法（separate chaining），避免线性探测
private:
    __docHash_node buckets[hashSize];

public:
    void importFromDoc(CharString &fileName);             //从文档导入：每次打开一个文档，将其中所有单词加入倒排索引
    docList *contained (const std::string& strIn);        //存在单词则返回指针，不存在则返回空指针
};


#endif //NEWSFEEDSYSTEM_DOCLISTHASH_H

/*
class docListNode {
    friend class docList;
    friend class docHash;
private:
    int docID;
    int times;

    docListNode *prior = nullptr;
    docListNode *next = nullptr;

    docListNode(int id, int _times) : docID(id), times(_times) {}
};

//每个单词都有一个文档链表，记录其出现在了哪些文档中，及其对应的出现次数
class docList {
public:
    docListNode *_front = nullptr;
    docListNode *_rear = nullptr;
    CharString term;
    int termID;

    docList(const CharString &_term, int id) : term(_term), termID(id) {}
    //按docID从小到大插入
    void add(int docID, int _times);
    //二分查找
    docListNode *search(int docID);         //查找这个单词是否有对应的文档记录，不存在则返回空指针
    void edit(int oldID, int newID, int newTimes);
    void remove(int docID);
};
 */