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
    bool empty();           //判断栈是否为空
    void push_back(const ContentClass& _node);  //压栈
    void pop_back();                     //退栈 todo: 如何改为返回引用？
    ContentClass &top();    //取得当前栈顶元素
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
    if (_rear) {	//判断栈不为空
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
