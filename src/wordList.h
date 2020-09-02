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

//Ϊ�˽������2���Ը����������ĵ���������Ӧ�Ĵʻ�������¼���г�������Щ���ʣ������Ӧ�ĳ��ִ���
class wordList {
public:
    wordListNode *_front = nullptr;
    wordListNode *_rear = nullptr;
    int docID;                 //�ĵ�ID
	int num_words = 0;			//�ܹ����ֵĵ�����Ŀ

    explicit wordList(int _docID) : docID(_docID) {}

    void add(CharString &term);        //�����µĵ���
    wordListNode *search(CharString &term);         //˳�������������Ƿ��ж�Ӧ�ļ�¼���������򷵻ؿ�ָ��
    void updateOrder();                  //���ݸ����ĵ���timesˢ������
};

#endif //NEWSFEEDSYSTEM_WORDLIST_H
