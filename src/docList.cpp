#include "docList.h"

//默认插在最后(单词出现次数最低)
void docList::add(int docID, int _times = 1) {
    auto node = new docListNode(docID, _times);
    if (!_front) {
        _rear = _front = node;
    } else {
        _rear->next = node;
        node->prior = _rear;
        _rear = node;
    }
    ++docNumber;
}

//顺序查找
docListNode *docList::search(int docID) {
    auto iter = _front;
    while (iter) {
        if (iter->docID == docID) {
            return iter;
        }
        iter = iter->next;
    }
    return nullptr; //找不到就返回空指针
}

void docList::edit(int oldID, int newID, int newTimes) {
    auto iter = search(oldID);
    if (iter) { //如果能找到才修改，找不到就不修改
        iter->docID = newID;
        iter->times = newTimes;
        updateOrder();        //更新文档链表，按单词出现顺序保持排序
    }
}

void docList::remove(int docID) {
    auto iter = search(docID);
    if (iter) { //如果能找到才删除，找不到就不删除
        if (iter->prior) {
            if (iter->next)
                iter->next->prior = iter->prior;
            else
                _rear = iter->prior;
            iter->prior->next = iter->next;
        } else {
            if (iter->next)
                iter->next->prior = nullptr;    //iter为头节点
            else
                _rear = nullptr;            //前后都没有
            _front = iter->next; //注意，此处_front可能被赋为空指针，即docList为空
        }
        iter->prior = nullptr;
        iter->next = nullptr;
        delete iter;
    }
}

void docList::updateOrder() {
    //冒泡排序
    //todo: 排序效率可以优化
    for (auto iter_out = _rear; iter_out->prior; iter_out = iter_out->prior) {
        for (auto iter_in = _front; iter_in != iter_out; iter_in = iter_in->next) {
            if (iter_in->times < iter_in->next->times) {
                int tempID = iter_in->docID, tempTimes = iter_in->times;
                iter_in->docID = iter_in->next->docID;
                iter_in->times = iter_in->next->times;
                iter_in->next->docID = tempID;
                iter_in->next->times = tempTimes;
            }
        }
    }
}

/*
//从小到大按升序插入
void docList::add(int docID, int _times){
    auto node = new docListNode(docID, _times);
    if (!_front) {
        _rear = _front = node;
    } else {
        if (node->docID < _front->docID) {      //插在开头
            _front->prior = node;
            node->next = _front;
            _front = node;
        } else {
            docListNode *iter = _front;
            while (iter->docID < node->docID) {
                if (iter->next) {
                    if (iter->next->docID > node->docID) {  //插在中间
                        node->next = iter->next;
                        iter->next->prior = node;
                        iter->next = node;
                        node->prior = iter;
                        break;
                    } else {
                        iter = iter->next;      //往后查找合适位置
                    }
                } else {    //插在最后一个
                    _rear->next = node;
                    node->prior = _rear;
                    _rear = node;
                    break;
                }
            }
        }
    }
    ++docNumber;
}
 */