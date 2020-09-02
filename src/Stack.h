#ifndef NEWSFEEDSYSTEM_STACK_H
#define NEWSFEEDSYSTEM_STACK_H

template <typename ContentClass>
class Stack;

template <typename ContentClass>
class StackNode {
    friend class Stack<ContentClass>;

private:
    ContentClass content;

    StackNode *prior = nullptr;
    StackNode *next = nullptr;

    explicit StackNode(const ContentClass _content) : content(_content) {}
};

template <typename ContentClass>
class Stack {
private:
    StackNode<ContentClass> *_front = nullptr;
    StackNode<ContentClass> *_rear = nullptr;
    unsigned int _length = 0;

public:
    bool empty();           //�ж�ջ�Ƿ�Ϊ��
    void push_back(const ContentClass& _node);  //ѹջ
    void pop_back();                     //��ջ todo: ��θ�Ϊ�������ã�
    ContentClass &top();    //ȡ�õ�ǰջ��Ԫ��
    unsigned int length();
};



template <typename ContentClass>
bool Stack<ContentClass>::empty() {
    if (_length) {
        return false;
    }
    return true;
}

template <typename ContentClass>
void Stack<ContentClass>::push_back(const ContentClass &_node) {
    auto node = new StackNode<ContentClass>(_node);
    if (!_front) {
        _front = node;
        _rear = _front;
    } else {
        _rear->next = node;
        node->prior = _rear;
        _rear = node;
    }
    _rear->next = nullptr;
    ++_length;
}

template <typename ContentClass>
void Stack<ContentClass>::pop_back() {
    if (_rear) {	//�ж�ջ��Ϊ��
        if (_rear == _front) {
            delete _rear;
            _rear = nullptr;
            _front = nullptr;
        } else {
            _rear = _rear->prior;
            delete _rear->next;
            _rear->next = nullptr;
        }
        --_length;
    }
}

template <typename ContentClass>
ContentClass& Stack<ContentClass>::top() {
    return _rear->content;
}

template <typename ContentClass>
unsigned int Stack<ContentClass>::length() {
    return _length;
}

#endif //NEWSFEEDSYSTEM_STACK_H
