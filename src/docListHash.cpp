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


//将分好词的文档中的单词信息读入到哈希表中
void docHash::importFromDoc(CharString &fileName) {
    int docID = fileName.remove_htmlSuffix()->getContentInt();
    std::ifstream txt("./output/" + fileName.remove_htmlSuffix()->getContentString() + ".txt");
    if (!txt)
        return;
    std::string wordLine;

    /*
     * 对于一个单词：这个单词在哈希表中是否出现过？
     * 如果没有这个单词，创建新的docList并压入这个__docHash_node的words中，而且要添加文档数据
     * 如果已经有这个单词了，这个文档是否在倒排文档中出现过？
     *  如果倒排文档中没有，创建新的docListNode并接入这个docList
     *  如果已经有这个文档了，其docID对应的docNode的times+1
     */
    while (getline(txt, wordLine)) {
        CharString word(wordLine);      //立即保存为自己结构
        int id = hashIndex(wordLine);
        docList *iter = contained(wordLine);
        if (!iter) {        //如果没有这个单词
            auto node = docList(word, id);
            ++node.occur;
            node.add(docID, 1);                 //新单词必然是新文档，将新文档插入文档链表
            buckets[id].words.push_back(node);  //将新单词开链法压入哈希表
        } else {
            ++iter->occur;
            docListNode *doc = iter->search(docID);
            if (!doc) { //如果倒排文档中没有这个文档
                iter->add(docID, 1);
            } else {
                ++doc->times;
                iter->updateOrder();        //更新文档链表，按单词出现顺序保持排序
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