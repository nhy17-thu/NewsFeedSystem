#include "wordList.h"

//新节点默认插在最后(单词出现次数最低)
void wordList::add(CharString &term) {
    auto node = search(term);
	++num_words;
    if (node) {
        ++node->times;
    } else {
        node = new wordListNode(term); 
        if (!_front) {
            _rear = _front = node;
        } else {
            _rear->next = node;
            node->prior = _rear;
            _rear = node;
        }
    }
}

wordListNode* wordList::search(CharString &term) {
    auto iter = _front;
    while (iter) {
        if (iter->term == term) {
            return iter;
        }
        iter = iter->next;
    }
    return nullptr; //找不到就返回空指针
}

void wordList::updateOrder() {
    //冒泡排序
    //todo: 排序效率可以优化
	if (_front && _rear) {		//为空时不需排序
		for (auto iter_out = _rear; iter_out->prior; iter_out = iter_out->prior) {
			for (auto iter_in = _front; iter_in != iter_out; iter_in = iter_in->next) {
				if (iter_in->times < iter_in->next->times) {
					CharString tempTerm = iter_in->term;
					int tempTimes = iter_in->times;
					iter_in->term = iter_in->next->term;
					iter_in->times = iter_in->next->times;
					iter_in->next->term = tempTerm;
					iter_in->next->times = tempTimes;
				}
			}
		}
	}	
}