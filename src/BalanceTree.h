#ifndef NEWSFEEDSYSTEM_BALANCETREE_H
#define NEWSFEEDSYSTEM_BALANCETREE_H

#include "CharString.h"
#include "Queue.h"
#include "docList.h"

/*
 * ��AVL���洢���ʣ��������ĵ���
 * ÿ�����ʶ��ж�Ӧ��ID���ĵ�����
 * ÿ���ĵ������м�¼��������ʳ��ֵ��ĵ���ÿ���ĵ��е��ʳ��ֵĴ���
 */

//��������ƽ�����������(AVL)ʵ�ֵĵ�������
class BalTreeNode {         //ͬһ�ڵ㣬ͬһID
    friend class BalTree;
private:
    //�½�����ʼ��һ���������ڵ㣬û��˫��ʱ��ʡ������asLeft
    BalTreeNode(BalTreeNode* _parent, CharString &word, int docID, bool asLeft);
    int getBalanceFactor(); //��ȡ��ǰ�ڵ�ƽ������

public:
    BalTreeNode *parent = nullptr;
    BalTreeNode *leftChild = nullptr;
    BalTreeNode *rightChild = nullptr;

    int nodeHeight = 1;     //Ĭ����ײ�ڵ�߶�Ϊ1��������������
    int termID;
    Queue<docList> words;   //words�д洢ͬһID��ͬ���ʣ�������ÿһ�����ʣ���docList��¼���ڲ�ͬ�ĵ��г��ֵĴ���

    void push(const CharString& wordIn, int docID);           //�����µ���
    docList *has(const CharString& target);     //���ID�Ƿ����и������ʣ����򷵻��ĵ�����������������Ϣ����û���򷵻ؿ�ָ��
    void updateHeight(bool onlyThisLevel);    //�ӵ�ǰ�ڵ������Ĭ�����ϸ��¸߶ȣ�����true��ֻ���±���߶ȣ�
    /*
     * �ж��Ե�ǰ�ڵ�Ϊ���ڵ�����Ƿ�ƽ��
     * ��Ҫ�����ҵ���ת����-2
     * ��Ҫ�����������˫��ת����-1
     * ����Ҫ��ת����ƽ�⣩����0
     * ��Ҫ�������Һ���˫��ת����1
     * ��Ҫ��������ת����2
     */
    int unbalanced();
};

//ƽ�������������AVL
class BalTree {
public:
    BalTreeNode *root = nullptr;

    //��root�ڵ�Ϊ����ӵ���
    void insert(CharString &word, int docID);
    static BalTreeNode *search(BalTreeNode *node, int id);  //��node�������Ҹ���ID�Ľڵ㲢���أ��Ҳ����򷵻ؿ�ָ��
    docList *searchWord(CharString &target);      //����Ŷ������������������ʣ��Ҳ����򷵻ؿ�ָ��
    void adjust(BalTreeNode *changedNode);      //����������ʹƽ��
    void edit();        //�༭ĳ�ڵ�

    void importFromDoc(CharString &fileName);             //���ĵ����룺ÿ�δ�һ���ĵ������������е��ʼ��뵹������
};

#endif //NEWSFEEDSYSTEM_BALANCETREE_H
