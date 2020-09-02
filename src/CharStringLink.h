#ifndef NEWSFEEDSYSTEM_CHARSTRINGLINK_H
#define NEWSFEEDSYSTEM_CHARSTRINGLINK_H

#include "CharString.h"

class CharStringLink;

class CharStringNode {
    friend class CharStringLink;
private:
    explicit CharStringNode(const CharString _content) : content(_content) {}

public:
	CharStringNode *prior = nullptr;
	CharStringNode *next = nullptr;

    CharString content;
};

class CharStringLink {
private:
	unsigned int _length = 0;

public:
    CharStringNode *front = nullptr;
    CharStringNode *rear = nullptr;

    void add(const CharString& _content, unsigned int pos);  //添加为第pos个节点（例，加在头部则为0）；默认加在头部
    void remove(unsigned int pos); //删除第pos个元素；默认删除头部
    void push_front(const CharString& _content);
    void push_back(const CharString& _content);
    void pop_back();
    void pop_front();
	unsigned int search(const CharString& targetStr);   //按内容查找某元素位置，找不到则返回SIZE_T_MAX
	static CharStringLink *divideWord(CharString &line, int _startPos);		//返回从当前位置开始的下一个单词（查询1使用）

    CharStringNode& operator[] (unsigned int pos);   //按下标返回元素引用，超长则返回最后一个
	unsigned int length();
};


#endif //NEWSFEEDSYSTEM_CHARSTRINGLINK_H
