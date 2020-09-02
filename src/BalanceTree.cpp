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

    //���������ʳ�ʼ���ڵ�
    auto startWord = new docList(word, termID);
    ++startWord->occur;
    startWord->add(docID, 1);
    words.push_back(*startWord);
}

/*
 * ���ڵ�ǰ�ڵ㣬�����Ƿ���ڣ�
 *  ������ʴ��ڣ�����ĵ��Ƿ���docList�д��ڣ�
 *      ���docID�����ڣ���add�½�docListNode������docList
 *      ���docID���ڣ�occur���ĵ���times+1����
 *  ������ʲ����ڣ��½�docList����ѹ�����BalTreeNode��words�У�����Ҫ����ĵ�����
 */
void BalTreeNode::push(const CharString &wordIn, int docID) {
    docList *wordIter = has(wordIn);
    if (wordIter) {
        ++wordIter->occur;
        docListNode *docIter = wordIter->search(docID);
        if (docIter) {
            ++docIter->times;
            wordIter->updateOrder();        //�����ĵ����������ʳ���˳�򱣳�����
        }
        else
            wordIter->add(docID, 1);
    } else {
        wordIter = new docList(wordIn, termID);     //ͬһ�ڵ㣬ͬһID
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
    int leftHeight = 0, rightHeight = 0;    //�����ڵĽڵ㣬��߶�Ϊ0
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
            return -2;      //��Ҫ�����ҵ���ת
        if (bf_child == -1)
            return -1;      //��Ҫ�����������˫��ת
    } else {
        if (bf < -1) {
            bf_child = rightChild->getBalanceFactor();
            if (bf_child == -1)
                return 2;       //��Ҫ��������ת
            if (bf_child == 1)
                return 1;       //��Ҫ�������Һ���˫��ת
        }
    }
    return 0;
}

void BalTreeNode::updateHeight(bool onlyThisLevel = false) {
    if (onlyThisLevel) {
        int leftHeight = 0, rightHeight = 0;    //�����ڵĽڵ㣬��߶�Ϊ0
        if (leftChild)
            leftHeight = leftChild->nodeHeight;
        if (rightChild)
            rightHeight = rightChild->nodeHeight;
        ++leftHeight;
        ++rightHeight;

        //˫�׽ڵ�ĸ߶����ӽڵ��������+1
        nodeHeight = leftHeight > rightHeight ? leftHeight : rightHeight;
    } else {
        for (BalTreeNode *iter = this; iter; iter = iter->parent) {
            int leftHeight = 0, rightHeight = 0;    //�����ڵĽڵ㣬��߶�Ϊ0
            if (iter->leftChild)
                leftHeight = iter->leftChild->nodeHeight;
            if (iter->rightChild)
                rightHeight = iter->rightChild->nodeHeight;
            ++leftHeight;
            ++rightHeight;

            //˫�׽ڵ�ĸ߶����ӽڵ��������+1
            iter->nodeHeight = leftHeight > rightHeight ? leftHeight : rightHeight;
        }
    }
}

/*
 * ��root�ڵ�Ϊ����ӵ��ʣ����ĵ���
 * ��Ӧ����ID�Ľڵ��Ƿ���ڣ�
 *  ����ڵ���ڣ���push�����µ���
 *  ����ڵ㲻���ڣ����½�һ���ڵ�(ͬʱ�½����ʺ��ĵ�����ڵ�)
 */
void BalTree::insert(CharString &word, int docID) {
    int wordID = hashIndex(word);

    BalTreeNode *node = search(root, wordID);        //���rootΪ�գ�nodeҲΪ�գ�ֱ�Ӳ���

    if (node) {
        node->push(word, docID);
    } else {    //����ڵ㲻����
        //���ҵ����ʵĲ���λ��
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

        //Ȼ�������λ���½��ڵ�
        if (!parent) {  //������½����ڵ�
            root = new BalTreeNode(nullptr, word, docID);
        } else {
            pos = new BalTreeNode(parent, word, docID, asLeft);

            //��ƽ��
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

//��ת��ƽ���㷨������շ����޸ĵ�λ��
void BalTree::adjust(BalTreeNode *changedNode) {
    //�Ӹı�Ľڵ�������ϣ�������������iter
    for (BalTreeNode *iter = changedNode->parent; iter; iter = iter->parent) {
        int bal = iter->unbalanced();
        switch (bal) {
            case -2: {  //�ҵ���ת
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

                if (temp) { //���temp��Ϊ��
                    temp->parent = iter;
                }
                iter->leftChild = temp;

                if (iter == root) {
                    root = newRoot;
                }

                iter->updateHeight();       //���ǵø��¸߶ȣ�
                break;
            }
            case -1: {  //�������˫��ת
                //������ת
                BalTreeNode *newRoot = iter->leftChild->rightChild;
                BalTreeNode *temp = newRoot->leftChild;

                newRoot->leftChild = iter->leftChild;
                iter->leftChild->parent = newRoot;
                if (temp) { //���temp��Ϊ��
                    temp->parent = iter->leftChild;
                }
                iter->leftChild->rightChild = temp;
				newRoot->parent = iter;
				iter->leftChild = newRoot;
                iter->leftChild->updateHeight();    //���¸߶�
                
                //���ҵ���ת
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

                if (temp) { //���temp��Ϊ��
                    temp->parent = iter;
                }
                iter->leftChild = temp;

                if (iter == root) {
                    root = newRoot;
                }

                iter->updateHeight();
                break;
            }
            case 1: {   //���Һ���˫��ת
                //���ҵ���ת
                BalTreeNode *newRoot = iter->rightChild->leftChild;
				BalTreeNode *temp = newRoot->rightChild;

                newRoot->rightChild = iter->rightChild;
                iter->rightChild->parent = newRoot;
                if (temp) { //���temp��Ϊ��
                    temp->parent = iter->rightChild;
                }
                iter->rightChild->leftChild = temp;
                newRoot->parent = iter;
                iter->rightChild = newRoot;
                iter->rightChild->updateHeight();

                //������ת
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

                if (temp) { //���temp��Ϊ��
                    temp->parent = iter;
                }
                iter->rightChild = temp;
                
                if (iter == root) {
                    root = newRoot;
                }

                iter->updateHeight();
                break;
            }
            case 2: {   //����ת
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
                if (temp) { //���temp��Ϊ��
                    temp->parent = iter;
                }
                iter->rightChild = temp;
                if (iter == root) {
                    root = newRoot;
                }

                iter->updateHeight();       //���ǵø��¸߶ȣ�
                break;
            }
            default: {
                iter->updateHeight();   //��ʹδʧ�⣬Ҳ����Ҫ���¸߶�
            }
        }
		if (bal)
			break;  //��ת��߶ȱ�Ȼ��ԭ���ʿ���ֹ�ж�
    }
}

//todo: �༭AVL��ĳ�ڵ�
void BalTree::edit() {

}

void BalTree::importFromDoc(CharString &fileName) {
    int docID = fileName.remove_htmlSuffix()->getContentInt();
    std::ifstream txt("./output/" + fileName.remove_htmlSuffix()->getContentString() + ".txt");
    if (!txt)
        return;
    std::string wordLine;

    while (getline(txt, wordLine)) {
        CharString word(wordLine);      //��������Ϊ�Լ��ṹ
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

    if (iter == root) {     //���֮ǰ����root�ڵ㣬����Ҫ�仯���ڵ�
        root = newRoot;
    } else {    //������ǣ����к͸��ڵ�֮��ı仯
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