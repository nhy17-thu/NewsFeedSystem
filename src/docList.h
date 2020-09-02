#ifndef NEWSFEEDSYSTEM_DOCLIST_H
#define NEWSFEEDSYSTEM_DOCLIST_H

#include "CharString.h"

class docListNode {
public:
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
    CharString term;            //单词
    int termID;                 //单词ID

    int docNumber = 0;          //单词出现在多少篇文章中（即文档链表长度）
    int occur = 0;              //单词总共出现次数

    docList(const CharString &_term, int id) : term(_term), termID(id) {}

    void add(int docID, int _times);        //插入新的文档
    docListNode *search(int docID);         //顺序查找这个单词是否有对应的文档记录，不存在则返回空指针
    void edit(int oldID, int newID, int newTimes);
    void remove(int docID);
    void updateOrder();                  //根据单词出现次数刷新排序
};

#endif //NEWSFEEDSYSTEM_DOCLIST_H
