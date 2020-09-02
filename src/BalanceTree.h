#ifndef NEWSFEEDSYSTEM_BALANCETREE_H
#define NEWSFEEDSYSTEM_BALANCETREE_H

#include "CharString.h"
#include "Queue.h"
#include "docList.h"

/*
 * 用AVL树存储单词（即倒排文档）
 * 每个单词都有对应的ID和文档链表
 * 每个文档链表中记录了这个单词出现的文档和每个文档中单词出现的次数
 */

//以下是用平衡二叉排序树(AVL)实现的倒排索引
class BalTreeNode {         //同一节点，同一ID
    friend class BalTree;
private:
    //新建并初始化一个二叉树节点，没有双亲时可省略输入asLeft
    BalTreeNode(BalTreeNode* _parent, CharString &word, int docID, bool asLeft);
    int getBalanceFactor(); //获取当前节点平衡因子

public:
    BalTreeNode *parent = nullptr;
    BalTreeNode *leftChild = nullptr;
    BalTreeNode *rightChild = nullptr;

    int nodeHeight = 1;     //默认最底层节点高度为1，向上依次增加
    int termID;
    Queue<docList> words;   //words中存储同一ID不同单词，而对于每一个单词，有docList记录其在不同文档中出现的次数

    void push(const CharString& wordIn, int docID);           //存入新单词
    docList *has(const CharString& target);     //这个ID是否已有给定单词，有则返回文档链表（包含其所有信息），没有则返回空指针
    void updateHeight(bool onlyThisLevel);    //从当前节点出发，默认向上更新高度（传入true则只更新本层高度）
    /*
     * 判断以当前节点为根节点的树是否平衡
     * 需要进行右单旋转返回-2
     * 需要进行先左后右双旋转返回-1
     * 不需要旋转（已平衡）返回0
     * 需要进行先右后左双旋转返回1
     * 需要进行左单旋转返回2
     */
    int unbalanced();
};

//平衡二叉排序树，AVL
class BalTree {
public:
    BalTreeNode *root = nullptr;

    //以root节点为根添加单词
    void insert(CharString &word, int docID);
    static BalTreeNode *search(BalTreeNode *node, int id);  //从node出发查找给定ID的节点并返回，找不到则返回空指针
    docList *searchWord(CharString &target);      //在这颗二叉树中搜索给定单词，找不到则返回空指针
    void adjust(BalTreeNode *changedNode);      //调整二叉树使平衡
    void edit();        //编辑某节点

    void importFromDoc(CharString &fileName);             //从文档导入：每次打开一个文档，将其中所有单词加入倒排索引
};

#endif //NEWSFEEDSYSTEM_BALANCETREE_H
