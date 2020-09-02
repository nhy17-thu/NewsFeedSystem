#include "docList.h"

//Ĭ�ϲ������(���ʳ��ִ������)
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

//˳�����
docListNode *docList::search(int docID) {
    auto iter = _front;
    while (iter) {
        if (iter->docID == docID) {
            return iter;
        }
        iter = iter->next;
    }
    return nullptr; //�Ҳ����ͷ��ؿ�ָ��
}

void docList::edit(int oldID, int newID, int newTimes) {
    auto iter = search(oldID);
    if (iter) { //������ҵ����޸ģ��Ҳ����Ͳ��޸�
        iter->docID = newID;
        iter->times = newTimes;
        updateOrder();        //�����ĵ����������ʳ���˳�򱣳�����
    }
}

void docList::remove(int docID) {
    auto iter = search(docID);
    if (iter) { //������ҵ���ɾ�����Ҳ����Ͳ�ɾ��
        if (iter->prior) {
            if (iter->next)
                iter->next->prior = iter->prior;
            else
                _rear = iter->prior;
            iter->prior->next = iter->next;
        } else {
            if (iter->next)
                iter->next->prior = nullptr;    //iterΪͷ�ڵ�
            else
                _rear = nullptr;            //ǰ��û��
            _front = iter->next; //ע�⣬�˴�_front���ܱ���Ϊ��ָ�룬��docListΪ��
        }
        iter->prior = nullptr;
        iter->next = nullptr;
        delete iter;
    }
}

void docList::updateOrder() {
    //ð������
    //todo: ����Ч�ʿ����Ż�
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
//��С�����������
void docList::add(int docID, int _times){
    auto node = new docListNode(docID, _times);
    if (!_front) {
        _rear = _front = node;
    } else {
        if (node->docID < _front->docID) {      //���ڿ�ͷ
            _front->prior = node;
            node->next = _front;
            _front = node;
        } else {
            docListNode *iter = _front;
            while (iter->docID < node->docID) {
                if (iter->next) {
                    if (iter->next->docID > node->docID) {  //�����м�
                        node->next = iter->next;
                        iter->next->prior = node;
                        iter->next = node;
                        node->prior = iter;
                        break;
                    } else {
                        iter = iter->next;      //������Һ���λ��
                    }
                } else {    //�������һ��
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