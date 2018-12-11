#include "CharStringLink.h"

void CharStringLink::add(const CharString& _content, size_t pos = 0) {
    auto node = new CharStringNode(_content);
    if (front) {    //�����Ϊ��
        if (!pos) {
            front->prior = node;
            node->next = front;
            front = node;
        } else {
            if (pos < _length) { //�����м�
                (*this)[pos].prior = node;
                node->next = &(*this)[pos];
                (*this)[pos - 1].next = node;
                node->prior = &(*this)[pos - 1];
            } else {    //����ĩβ
                rear->next = node;
                node->prior = rear;
                rear = node;
            }
        }
    } else {    //���Ϊ��
        front = rear = node;
    }
    ++_length;
}

void CharStringLink::remove(size_t pos = 0) {
    auto iter = &(*this)[pos];
    if (iter->next)
        iter->next->prior = iter->prior;    //���Ը�ֵΪnullptr
    if (iter->prior)
        iter->prior->next = iter->next;
    if (pos == 0)   //���ɾ����ͷ�ڵ�
        front = iter->next;
    else if (pos >= _length - 1)    //���ɾ����β�ڵ�
        rear = iter->prior;
    iter->prior = nullptr;
    iter->next = nullptr;
    iter->content.~CharString();
    --_length;
}

size_t CharStringLink::search(const CharString& targetStr) {
    size_t count(0);
    auto iter = front;
    while(iter) {
        if (iter->content == targetStr) {
            return count;
        }
        ++count;
        iter = iter->next;
    }
    return SIZE_MAX;
}

CharStringNode& CharStringLink::operator[](size_t pos) {
    CharStringNode *iter = front;
    for (size_t i = 0; i < pos && i < _length - 1; ++i) {
        iter = iter->next;
    }
    return *iter;
}

void CharStringLink::push_front(const CharString& _content) {
    add(_content);
}

void CharStringLink::push_back(const CharString& _content){
    add(_content, SIZE_MAX);
}

void CharStringLink::pop_front() {
    remove();
}

void CharStringLink::pop_back() {
    remove(SIZE_MAX);
}

size_t CharStringLink::length() {
    return _length;
}