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

    void add(const CharString& _content, unsigned int pos);  //���Ϊ��pos���ڵ㣨��������ͷ����Ϊ0����Ĭ�ϼ���ͷ��
    void remove(unsigned int pos); //ɾ����pos��Ԫ�أ�Ĭ��ɾ��ͷ��
    void push_front(const CharString& _content);
    void push_back(const CharString& _content);
    void pop_back();
    void pop_front();
	unsigned int search(const CharString& targetStr);   //�����ݲ���ĳԪ��λ�ã��Ҳ����򷵻�SIZE_T_MAX
	static CharStringLink *divideWord(CharString &line, int _startPos);		//���شӵ�ǰλ�ÿ�ʼ����һ�����ʣ���ѯ1ʹ�ã�

    CharStringNode& operator[] (unsigned int pos);   //���±귵��Ԫ�����ã������򷵻����һ��
	unsigned int length();
};


#endif //NEWSFEEDSYSTEM_CHARSTRINGLINK_H
