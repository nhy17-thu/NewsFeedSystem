#ifndef NEWSFEEDSYSTEM_QUEUE_H
#define NEWSFEEDSYSTEM_QUEUE_H

template <typename ContentClass>
class Queue;

template <typename ContentClass>
class QueueNode {
public:
    ContentClass content;

    QueueNode *prior = nullptr;
    QueueNode *next = nullptr;

    explicit QueueNode(const ContentClass _content) : content(_content) {}
};

template <typename ContentClass>
class Queue {
public:
    QueueNode<ContentClass> *_front = nullptr;
    QueueNode<ContentClass> *_rear = nullptr;
    unsigned int _length = 0;

    bool empty();           //�ж϶����Ƿ�Ϊ��
    void push_back(const ContentClass& _node);  //�Ӻ�ѹ�����
    void push_front(const ContentClass* _node); //��ǰ��ѹ�����
    void pop_back();                     //�Ӻ��˳�����
    void pop_front();                    //��ǰ���˳�����
    QueueNode<ContentClass> &rear();    //ȡ�õ�ǰ����β��Ԫ��
    QueueNode<ContentClass> &front();  //ȡ�õ�ǰ����ͷ��Ԫ��
    unsigned int length();
	ContentClass frontContent();

};


template <typename ContentClass>
bool Queue<ContentClass>::empty() {
    if (_length) {
        return false;
    }
    return true;
}

template <typename ContentClass>
void Queue<ContentClass>::push_back(const ContentClass &_node) {
    auto node = new QueueNode<ContentClass>(_node);
    if (!_front) {
        _rear = _front = node;
    } else {
        _rear->next = node;
        node->prior = _rear;
        _rear = node;
    }
    _rear->next = nullptr;
    ++_length;
}

template <typename ContentClass>
void Queue<ContentClass>::push_front(const ContentClass *_node) {
    auto node = new QueueNode<ContentClass>(_node);
    if (!_front) {
        _rear = _front = node;
    } else {
        _front->prior = node;
        node->next = _front;
        _front = node;
    }
    _rear->next = nullptr;
    ++_length;
}

template <typename ContentClass>
void Queue<ContentClass>::pop_back() {
    if (_rear) {
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
void Queue<ContentClass>::pop_front(){
    if (_front) {
        if (_rear == _front) {
            delete _front;
            _rear = nullptr;
            _front = nullptr;
        } else {
            _front = _front->next;
            delete _front->prior;
            _front->prior = nullptr;
        }
        --_length;
    }
}

template <typename ContentClass>
QueueNode<ContentClass>& Queue<ContentClass>::rear() {
    return *_rear;
}

template <typename ContentClass>
QueueNode<ContentClass>& Queue<ContentClass>::front() {
    return *_front;
}

template <typename ContentClass>
ContentClass Queue<ContentClass>::frontContent() {
	return _front->content;
}

template <typename ContentClass>
unsigned int Queue<ContentClass>::length() {
    return _length;
}

#endif //NEWSFEEDSYSTEM_QUEUE_H
