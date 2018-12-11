#ifndef NEWSFEEDSYSTEM_CHARSTRINGLINK_H
#define NEWSFEEDSYSTEM_CHARSTRINGLINK_H

#include "CharString.h"

class CharStringLink;

class CharStringNode {
    friend class CharStringLink;

private:
    CharStringNode *prior = nullptr;
    CharStringNode *next = nullptr;

    explicit CharStringNode(const CharString _content) : content(_content) {}

public:
    CharString content;
};

class CharStringLink {
private:
    size_t _length = 0;

public:
    CharStringNode *front = nullptr;
    CharStringNode *rear = nullptr;

    void add(const CharString& _content, size_t pos);  //���Ϊ��pos���ڵ㣨��������ͷ����Ϊ0����Ĭ�ϼ���ͷ��
    void remove(size_t pos); //ɾ����pos��Ԫ�أ�Ĭ��ɾ��ͷ��
    void push_front(const CharString& _content);
    void push_back(const CharString& _content);
    void pop_back();
    void pop_front();
    size_t search(const CharString& targetStr);   //�����ݲ���ĳԪ��λ�ã��Ҳ����򷵻�SIZE_T_MAX

    CharStringNode& operator[] (size_t pos);   //���±귵��Ԫ�����ã������򷵻����һ��
    size_t length();
};


#endif //NEWSFEEDSYSTEM_CHARSTRINGLINK_H
