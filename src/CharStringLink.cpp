#include "CharStringLink.h"

void CharStringLink::add(const CharString& _content, unsigned int pos = 0) {
    auto node = new CharStringNode(_content);
    if (front) {    //如果不为空
        if (!pos) {
            front->prior = node;
            node->next = front;
            front = node;
        } else {
            if (pos < _length) { //插在中间
                (*this)[pos].prior = node;
                node->next = &(*this)[pos];
                (*this)[pos - 1].next = node;
                node->prior = &(*this)[pos - 1];
            } else {    //插在末尾
                rear->next = node;
                node->prior = rear;
                rear = node;
            }
        }
    } else {    //如果为空
        front = rear = node;
    }
    ++_length;
}

//删掉pos节点
void CharStringLink::remove(unsigned int pos = 0) {
    auto iter = &(*this)[pos];
    if (iter->next)
        iter->next->prior = iter->prior;    //可以赋值为nullptr
    if (iter->prior)
        iter->prior->next = iter->next;
    if (pos == 0)   //如果删掉了头节点
        front = iter->next;
    else if (pos >= _length - 1)    //如果删掉了尾节点
        rear = iter->prior;
    iter->prior = nullptr;
    iter->next = nullptr;
    iter->content.~CharString();
    delete iter;
    iter = nullptr;
    --_length;
}

unsigned int CharStringLink::search(const CharString& targetStr) {
	unsigned int count(0);
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

CharStringNode& CharStringLink::operator[](unsigned int pos) {
    CharStringNode *iter = front;
    for (unsigned int i = 0; i < pos && i < _length - 1; ++i) {
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

unsigned int CharStringLink::length() {
    return _length;
}

CharStringLink *CharStringLink::divideWord(CharString &line, int _startPos = 0) {
    auto words = new CharStringLink;
    int startPos = 0, endPos = line.indexOf(CharString(" "), 0);
    bool endLine = false;
    while (startPos != -1) {
        if (endPos == -1) {
            endLine = true;
            endPos = SIZE_MAX;
        }
        words->push_back(*line.substring(endPos - startPos, startPos));

        if (endLine)      //分词完毕
            break;
        startPos = endPos + 1;
        endPos = line.indexOf(CharString(" "), startPos); //找到下一个空格
    }
    return words;
}