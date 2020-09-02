#ifndef NEWSFEEDSYSTEM_DOCLISTHASH_H
#define NEWSFEEDSYSTEM_DOCLISTHASH_H

#include "Queue.h"
#include "CharString.h"
#include "docList.h"

#define hashSize 24593 //����SGI STL��ȡһ����������������ϣ������ʵ��һ����Ϊ������

/*
 * �ù�ϣ��洢���ʣ��������ĵ���
 * ÿ�����ʶ��ж�Ӧ��ID���ĵ�����
 * ÿ���ĵ������м�¼��������ʳ��ֵ��ĵ���ÿ���ĵ��е��ʳ��ֵĴ���
 */

//�������ù�ϣ��ʵ�ֵĵ�������
class __docHash_node {
    friend class docHash;
private:
    Queue<docList> words;        //words�д洢��ͬ�ĵ��ʣ�������ÿһ�����ʣ���docList��¼���ڲ�ͬ�ĵ��г��ֵĴ���
    void push(const docList& nodeIn);       //�����µ���
    bool has(const CharString& target);     //�Ƿ����е���
};

class docHash {   //���ÿ�������separate chaining������������̽��
private:
    __docHash_node buckets[hashSize];

public:
    void importFromDoc(CharString &fileName);             //���ĵ����룺ÿ�δ�һ���ĵ������������е��ʼ��뵹������
    docList *contained (const std::string& strIn);        //���ڵ����򷵻�ָ�룬�������򷵻ؿ�ָ��
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

//ÿ�����ʶ���һ���ĵ�������¼�����������Щ�ĵ��У������Ӧ�ĳ��ִ���
class docList {
public:
    docListNode *_front = nullptr;
    docListNode *_rear = nullptr;
    CharString term;
    int termID;

    docList(const CharString &_term, int id) : term(_term), termID(id) {}
    //��docID��С�������
    void add(int docID, int _times);
    //���ֲ���
    docListNode *search(int docID);         //������������Ƿ��ж�Ӧ���ĵ���¼���������򷵻ؿ�ָ��
    void edit(int oldID, int newID, int newTimes);
    void remove(int docID);
};
 */