#include "BalanceTree.h"
#include <fstream>

#define nodeSize 24593
int hashIndex(CharString &GBword) {
    auto temp = (((unsigned char)GBword[0] - 176) * 94 + (unsigned char)GBword[1] - 161) % nodeSize;
    if (temp < 0)
        temp = -temp;
    return temp;
}

BalTreeNode::BalTreeNode(BalTreeNode *_parent, CharString &word, int docID, bool asLeft = true) {
    termID = hashIndex(word);
    parent = _parent;
    if (parent) {
        if (asLeft)
            parent->leftChild = this;
        else
            parent->rightChild = this;
    }

    //用所给单词初始化节点
    auto startWord = new docList(word, termID);
    ++startWord->occur;
    startWord->add(docID, 1);
    words.push_back(*startWord);
}

/*
 * 对于当前节点，单词是否存在？
 *  如果单词存在，这个文档是否在docList中存在？
 *      如果docID不存在，用add新建docListNode并接入docList
 *      如果docID存在，occur和文档的times+1即可
 *  如果单词不存在，新建docList，并压入这个BalTreeNode的words中，而且要添加文档数据
 */
void BalTreeNode::push(const CharString &wordIn, int docID) {
    docList *wordIter = has(wordIn);
    if (wordIter) {
        ++wordIter->occur;
        docListNode *docIter = wordIter->search(docID);
        if (docIter) {
            ++docIter->times;
            wordIter->updateOrder();        //更新文档链表，按单词出现顺序保持排序
        }
        else
            wordIter->add(docID, 1);
    } else {
        wordIter = new docList(wordIn, termID);     //同一节点，同一ID
        ++wordIter->occur;
        wordIter->add(docID, 1);
        words.push_back(*wordIter);
    }
}

docList *BalTreeNode::has(const CharString &target) {
    if (words.empty())
        return nullptr;
    auto iter = &words.front();
    while (iter) {
        if (iter->content.term == target) {
            return &iter->content;
        }
        iter = iter->next;
    }
    return nullptr;
}

int BalTreeNode::getBalanceFactor() {
    int leftHeight = 0, rightHeight = 0;    //不存在的节点，其高度为0
    if (leftChild)
        leftHeight = leftChild->nodeHeight;
    if (rightChild)
        rightHeight = rightChild->nodeHeight;
    return leftHeight - rightHeight;
}

int BalTreeNode::unbalanced() {
    int bf = getBalanceFactor();
    int bf_child = 0;
    if (bf > 1) {
        bf_child = leftChild->getBalanceFactor();
        if (bf_child == 1)
            return -2;      //需要进行右单旋转
        if (bf_child == -1)
            return -1;      //需要进行先左后右双旋转
    } else {
        if (bf < -1) {
            bf_child = rightChild->getBalanceFactor();
            if (bf_child == -1)
                return 2;       //需要进行左单旋转
            if (bf_child == 1)
                return 1;       //需要进行先右后左双旋转
        }
    }
    return 0;
}

void BalTreeNode::updateHeight(bool onlyThisLevel = false) {
    if (onlyThisLevel) {
        int leftHeight = 0, rightHeight = 0;    //不存在的节点，其高度为0
        if (leftChild)
            leftHeight = leftChild->nodeHeight;
        if (rightChild)
            rightHeight = rightChild->nodeHeight;
        ++leftHeight;
        ++rightHeight;

        //双亲节点的高度是子节点中最高者+1
        nodeHeight = leftHeight > rightHeight ? leftHeight : rightHeight;
    } else {
        for (BalTreeNode *iter = this; iter; iter = iter->parent) {
            int leftHeight = 0, rightHeight = 0;    //不存在的节点，其高度为0
            if (iter->leftChild)
                leftHeight = iter->leftChild->nodeHeight;
            if (iter->rightChild)
                rightHeight = iter->rightChild->nodeHeight;
            ++leftHeight;
            ++rightHeight;

            //双亲节点的高度是子节点中最高者+1
            iter->nodeHeight = leftHeight > rightHeight ? leftHeight : rightHeight;
        }
    }
}

/*
 * 以root节点为根添加单词（新文档）
 * 对应单词ID的节点是否存在？
 *  如果节点存在，用push存入新单词
 *  如果节点不存在，就新建一个节点(同时新建单词和文档链表节点)
 */
void BalTree::insert(CharString &word, int docID) {
    int wordID = hashIndex(word);

    BalTreeNode *node = search(root, wordID);        //如果root为空，node也为空，直接插入

    if (node) {
        node->push(word, docID);
    } else {    //如果节点不存在
        //先找到合适的插入位置
        BalTreeNode *parent = nullptr, *pos = root;
        bool asLeft = true;
        while (pos) {
            parent = pos;
            if (pos->termID > wordID) {
                asLeft = true;
                pos = pos->leftChild;
            } else {
                asLeft = false;
                pos = pos->rightChild;
            }
        }

        //然后在这个位置新建节点
        if (!parent) {  //如果在新建根节点
            root = new BalTreeNode(nullptr, word, docID);
        } else {
            pos = new BalTreeNode(parent, word, docID, asLeft);

            //重平衡
            pos->updateHeight();
            adjust(pos);
        }
    }
}

BalTreeNode *BalTree::search(BalTreeNode *node, int id) {
    if (!node) {
        return nullptr;
    }
    if (id == node->termID) {
        return node;
    }
    if (id < node->termID) {
        return search(node->leftChild, id);
    } else {
        return search(node->rightChild, id);
    }
	return nullptr;
}

//旋转重平衡算法，传入刚发生修改的位置
void BalTree::adjust(BalTreeNode *changedNode) {
    //从改变的节点出发向上，逐层检查各代祖先iter
    for (BalTreeNode *iter = changedNode->parent; iter; iter = iter->parent) {
        int bal = iter->unbalanced();
        switch (bal) {
            case -2: {  //右单旋转
                BalTreeNode *newRoot = iter->leftChild;
                BalTreeNode *temp = newRoot->rightChild;

                if (iter->parent) {
                    if (iter == iter->parent->leftChild)
                        iter->parent->leftChild = newRoot;
                    else
                        iter->parent->rightChild = newRoot;
                }

                newRoot->rightChild = iter;
                newRoot->parent = iter->parent;
                iter->parent = newRoot;

                if (temp) { //如果temp不为空
                    temp->parent = iter;
                }
                iter->leftChild = temp;

                if (iter == root) {
                    root = newRoot;
                }

                iter->updateHeight();       //最后记得更新高度！
                break;
            }
            case -1: {  //先左后右双旋转
                //先左单旋转
                BalTreeNode *newRoot = iter->leftChild->rightChild;
                BalTreeNode *temp = newRoot->leftChild;

                newRoot->leftChild = iter->leftChild;
                iter->leftChild->parent = newRoot;
                if (temp) { //如果temp不为空
                    temp->parent = iter->leftChild;
                }
                iter->leftChild->rightChild = temp;
				newRoot->parent = iter;
				iter->leftChild = newRoot;
                iter->leftChild->updateHeight();    //更新高度
                
                //后右单旋转
                if (iter->parent) {
                    if (iter == iter->parent->leftChild)
                        iter->parent->leftChild = newRoot;
                    else
                        iter->parent->rightChild = newRoot;
                }
                temp = newRoot->rightChild;

                newRoot->rightChild = iter;
                newRoot->parent = iter->parent;
                iter->parent = newRoot;

                if (temp) { //如果temp不为空
                    temp->parent = iter;
                }
                iter->leftChild = temp;

                if (iter == root) {
                    root = newRoot;
                }

                iter->updateHeight();
                break;
            }
            case 1: {   //先右后左双旋转
                //先右单旋转
                BalTreeNode *newRoot = iter->rightChild->leftChild;
				BalTreeNode *temp = newRoot->rightChild;

                newRoot->rightChild = iter->rightChild;
                iter->rightChild->parent = newRoot;
                if (temp) { //如果temp不为空
                    temp->parent = iter->rightChild;
                }
                iter->rightChild->leftChild = temp;
                newRoot->parent = iter;
                iter->rightChild = newRoot;
                iter->rightChild->updateHeight();

                //后左单旋转
                if (iter->parent) {
                    if (iter == iter->parent->leftChild)
                        iter->parent->leftChild = newRoot;
                    else
                        iter->parent->rightChild = newRoot;
                }
                temp = newRoot->leftChild;

                newRoot->leftChild = iter;
                newRoot->parent = iter->parent;
                iter->parent = newRoot;

                if (temp) { //如果temp不为空
                    temp->parent = iter;
                }
                iter->rightChild = temp;
                
                if (iter == root) {
                    root = newRoot;
                }

                iter->updateHeight();
                break;
            }
            case 2: {   //左单旋转
                BalTreeNode *newRoot = iter->rightChild;
                BalTreeNode *temp = newRoot->leftChild;

                if (iter->parent) {
                    if (iter == iter->parent->leftChild)
                        iter->parent->leftChild = newRoot;
                    else
                        iter->parent->rightChild = newRoot;
                }

                newRoot->leftChild = iter;
                newRoot->parent = iter->parent;
                iter->parent = newRoot;
                if (temp) { //如果temp不为空
                    temp->parent = iter;
                }
                iter->rightChild = temp;
                if (iter == root) {
                    root = newRoot;
                }

                iter->updateHeight();       //最后记得更新高度！
                break;
            }
            default: {
                iter->updateHeight();   //即使未失衡，也可能要更新高度
            }
        }
		if (bal)
			break;  //旋转后高度必然复原，故可终止判断
    }
}

//todo: 编辑AVL树某节点
void BalTree::edit() {

}

void BalTree::importFromDoc(CharString &fileName) {
    int docID = fileName.remove_htmlSuffix()->getContentInt();
    std::ifstream txt("./output/" + fileName.remove_htmlSuffix()->getContentString() + ".txt");
    if (!txt)
        return;
    std::string wordLine;

    while (getline(txt, wordLine)) {
        CharString word(wordLine);      //立即保存为自己结构
        insert(word, docID);
    }
}

docList* BalTree::searchWord(CharString &target) {
    BalTreeNode *targetNode = search(root, hashIndex(target));
    return targetNode->has(target);
}

/*
void BalTree::rotateR(BalTreeNode *iter) {
    BalTreeNode *newRoot = iter->leftChild;
    BalTreeNode *temp = newRoot->rightChild;

    if (temp) {
        iter->leftChild = temp;
        temp->parent = iter;
    }

    BalTreeNode *grandPa = iter->parent;
    newRoot->rightChild = iter;
    iter->parent = newRoot;

    if (iter == root) {     //如果之前就是root节点，不需要变化父节点
        root = newRoot;
    } else {    //如果不是，还有和父节点之间的变化
        if (grandPa->leftChild == iter) {
            grandPa->leftChild = newRoot;
        } else {
            grandPa->rightChild = newRoot;
        }
        newRoot->parent = grandPa;

        iter->updateHeight();
    }
}

void BalTree::rotateL(BalTreeNode *iter) {
    BalTreeNode *newRoot = iter->rightChild;
    BalTreeNode *temp = newRoot->leftChild;

    if (temp) {
        iter->rightChild = temp;
        temp->parent = iter;
    }

    BalTreeNode *grandPa = iter->parent;
    newRoot->leftChild = iter;
    iter->parent = newRoot;

    if (iter == root) {
        root = newRoot;
    } else {
        if (grandPa->leftChild == iter) {
            grandPa->leftChild = newRoot;
        } else {
            grandPa->rightChild = newRoot;
        }
        newRoot->parent = grandPa;

        iter->updateHeight();
    }
}
*/