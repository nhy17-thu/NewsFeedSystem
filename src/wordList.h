#ifndef NEWSFEEDSYSTEM_WORDLIST_H
#define NEWSFEEDSYSTEM_WORDLIST_H

#include "CharString.h"

class wordListNode {
public:
    CharString term;
    int times;

    wordListNode *prior = nullptr;
    wordListNode *next = nullptr;

    explicit wordListNode(CharString &_term, int _times = 1) : term(_term), times(_times) {}
};

//为了解决需求2：对给定的新闻文档，建立对应的词汇链表，记录其中出现了哪些单词，及其对应的出现次数
class wordList {
public:
    wordListNode *_front = nullptr;
    wordListNode *_rear = nullptr;
    int docID;                 //文档ID
	int num_words = 0;			//总共出现的单词数目

    explicit wordList(int _docID) : docID(_docID) {}

    void add(CharString &term);        //插入新的单词
    wordListNode *search(CharString &term);         //顺序查找这个单词是否有对应的记录，不存在则返回空指针
    void updateOrder();                  //根据各个文档的times刷新排序
};

#endif //NEWSFEEDSYSTEM_WORDLIST_H
