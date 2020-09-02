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

//ÿ�����ʶ���һ���ĵ�������¼�����������Щ�ĵ��У������Ӧ�ĳ��ִ���
class docList {
public:
    docListNode *_front = nullptr;
    docListNode *_rear = nullptr;
    CharString term;            //����
    int termID;                 //����ID

    int docNumber = 0;          //���ʳ����ڶ���ƪ�����У����ĵ������ȣ�
    int occur = 0;              //�����ܹ����ִ���

    docList(const CharString &_term, int id) : term(_term), termID(id) {}

    void add(int docID, int _times);        //�����µ��ĵ�
    docListNode *search(int docID);         //˳�������������Ƿ��ж�Ӧ���ĵ���¼���������򷵻ؿ�ָ��
    void edit(int oldID, int newID, int newTimes);
    void remove(int docID);
    void updateOrder();                  //���ݵ��ʳ��ִ���ˢ������
};

#endif //NEWSFEEDSYSTEM_DOCLIST_H
