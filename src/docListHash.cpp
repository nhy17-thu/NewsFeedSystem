#include "docListHash.h"
#include <fstream>

int hashIndex(const std::string &GBword);

void __docHash_node::push(const docList &nodeIn) {
    words.push_back(nodeIn);
}

bool __docHash_node::has(const CharString &target) {
    if (words.empty())
        return false;
    auto iter = &words.front();
    while (iter) {
        if (iter->content.term == target)
            return true;
        else
            iter = iter->next;
    }
    return false;
}


//���ֺôʵ��ĵ��еĵ�����Ϣ���뵽��ϣ����
void docHash::importFromDoc(CharString &fileName) {
    int docID = fileName.remove_htmlSuffix()->getContentInt();
    std::ifstream txt("./output/" + fileName.remove_htmlSuffix()->getContentString() + ".txt");
    if (!txt)
        return;
    std::string wordLine;

    /*
     * ����һ�����ʣ���������ڹ�ϣ�����Ƿ���ֹ���
     * ���û��������ʣ������µ�docList��ѹ�����__docHash_node��words�У�����Ҫ����ĵ�����
     * ����Ѿ�����������ˣ�����ĵ��Ƿ��ڵ����ĵ��г��ֹ���
     *  ��������ĵ���û�У������µ�docListNode���������docList
     *  ����Ѿ�������ĵ��ˣ���docID��Ӧ��docNode��times+1
     */
    while (getline(txt, wordLine)) {
        CharString word(wordLine);      //��������Ϊ�Լ��ṹ
        int id = hashIndex(wordLine);
        docList *iter = contained(wordLine);
        if (!iter) {        //���û���������
            auto node = docList(word, id);
            ++node.occur;
            node.add(docID, 1);                 //�µ��ʱ�Ȼ�����ĵ��������ĵ������ĵ�����
            buckets[id].words.push_back(node);  //���µ��ʿ�����ѹ���ϣ��
        } else {
            ++iter->occur;
            docListNode *doc = iter->search(docID);
            if (!doc) { //��������ĵ���û������ĵ�
                iter->add(docID, 1);
            } else {
                ++doc->times;
                iter->updateOrder();        //�����ĵ����������ʳ���˳�򱣳�����
            }
        }
    }
}

docList *docHash::contained(const std::string &strIn) {
    auto iter = &buckets[hashIndex(strIn)].words.front();
    while (iter) {
        if (iter->content.term == strIn) {
            return &iter->content;
        }
        iter = iter->next;
    }
    return nullptr;
}