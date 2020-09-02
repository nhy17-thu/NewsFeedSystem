#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include "Stack.h"
#include "CharString.h"
#include "CharStringLink.h"
#include "HashTable.h"
#include "docListHash.h"
#include "wordList.h"
#include "BalanceTree.h"
#include "GetAllFiles.h"
using namespace std;

void printHTML2txt(CharString& fileName);
void initDictionary(hashTable& dic);
CharStringLink *divideWords(CharString& fileName, hashTable& dic);
#define maxWordLength 20

void extractInfo(CharString& fileName, docHash& list);          //基于哈希表构建倒排文档
void extractInfo(CharString& fileName, BalTree& AVLtree);       //基于AVL树构建倒排文档
void respondQuery(BalTree &reversedDocs);
void respondRecommend(BalTree &reversedDocs, CharStringLink& files);

Queue<int> timeNotFound;

int main() {
	hashTable hash_dictionary;
	initDictionary(hash_dictionary);

    CharStringLink files;
    //此处扫描获得当前文件夹下所有文件名
	GetAllFiles(files);
    size_t length = files.length();

    /*
     * 这部分注释掉的为实验一内容
    for (size_t i = 0; i < length; ++i) {
		//扫描全文
        printHTML2txt(files[i].content);
    }
	
	for (size_t i = 0; i < length; ++i) {
		//分词
		auto wordsList = divideWords(files[i].content, hash_dictionary);
		ofstream info("./output/" + files[i].content.remove_htmlSuffix()->getContentString() + ".txt");
		CharString empty("");
		while (wordsList->length()) {
			CharString temp = wordsList->front->content;
			if (temp != empty) {
				info << temp << endl;
			}
			wordsList->pop_front();
		}
	}
     */

    BalTree reversedDocs;   //基于AVL树构建倒排文档
    for (size_t i = 0; i < length; ++i) {
        extractInfo(files[i].content, reversedDocs);
    }

    //实现批量查询功能
    respondQuery(reversedDocs);

    //实现新闻推荐功能
    respondRecommend(reversedDocs, files);

    return 0;
}

void initDictionary(hashTable& dic) {
    ifstream dictionary;
	dictionary.open("dictionary.dic");
    if (!dictionary)
        return;
    string _line;
    int countLine = 0;
    while (dictionary >> _line) {
        ++countLine;
        dic.insert(_line);  //保存为字典
    }
}

void printHTML2txt(CharString &fileName) {
    ifstream html("./input/" + fileName.getContentString());    //打开对应文档
    if (!html)
        return;
    ofstream info("./output/" + fileName.remove_htmlSuffix()->getContentString() + ".info");

    string _line;
    bool inBody(false), inScript(false), inText(false), inTextArea(false);
    CharString startBody("<body"), endBody("</body");
    Stack<CharString> tags;
    Stack<CharString> contentsStack;
    int countLine = 0;
	bool timeFound = false;

    while (getline(html, _line)) { //每次读入一行
        ++countLine;
        CharString line(_line); //立刻保存为自定义字符串类型
        int currentPos = -1;
		if (inScript && line.frontTag() == "</script")
			inScript = false;
        if (inBody && !inScript) {
            if (line.frontTag() == endBody) {
				if (!timeFound) {
					timeNotFound.push_back(fileName.remove_htmlSuffix()->getContentInt());
				}
                return;
            } else {
                CharString frontTag = line.frontTag();
                if ((frontTag.singleTag() && !line.startWithOmission()) || !line.frontTag().length()) {
                    continue;
                }
                if (line.frontTag() == CharString("<script")) {
                    if (line.indexOf(CharString("</script")) == -1) {
                        inScript = true;
                    }
                    continue;
                }
                while (!inText && !inTextArea) {  //处理当前行所有标签,注意indexOf找不到的时候返回-1
                    int nextStart = line.indexOf(CharString("<"), currentPos + 1);
                    if (nextStart == -1)    //如果剩下没有标签了
                        break;
                    int nextEnd = line.indexOf(CharString("</"), currentPos + 1);
                    if (nextStart < nextEnd || nextEnd == -1) {  //如果开始了新标签，压栈
                        currentPos = nextStart;
                        if (line.isText(currentPos) != -1) {    //如果当前新标签标志正文
                            inText = true;
                            break;
                        } else if (line.indexOf(CharString("<textarea")) != -1) {   //如果标志图集
                            inTextArea = true;
                            break;
                        }
                        int nextSpaceClose = line.indexOf(CharString(" "), currentPos + 1);
                        int nextTagClose = line.indexOf(CharString(">"), currentPos + 1);
                        int nextClose;
                        if (nextSpaceClose != -1 && nextTagClose != -1)
                            nextClose = nextSpaceClose > nextTagClose ? nextTagClose : nextSpaceClose;
                        if (nextSpaceClose != -1 && nextTagClose == -1)
                            nextClose = nextSpaceClose;
                        else if (nextSpaceClose == -1 && nextTagClose != -1)
                            nextClose = nextTagClose;
                        CharString *temp(line.substring(nextClose - currentPos, currentPos));
                        if (!temp->singleTag()) {
                            if (*temp == CharString("<script")) {
                                inScript = true;
                                break;
                            } else {
                                tags.push_back(*temp);  //压入的tags例如<div, <p
                            }
                        } else {
                            continue;
                        }
                    } else {    //如果结束了原来的标签，判断是否需要提取两个位置间内容，然后再出栈（或没有新标签了）
                        int lastPos(currentPos);
                        currentPos = nextEnd;
						if (!tags.empty()) {
							CharString top(tags.top());
							//标题名：<h1...>XXX</h1>
							//若有<a...>XXX</a>，则中间的XXX不换行
							//如果能找到对应的id编码且标签对应（用pairedTags函数），就输入">"到"<"位置间的内容
							//找到id编码：从栈顶位置（lastPos）开始，在下一个'>'之前能找到id="tagsWeWant"
							if (top == "<h1") { //如果是标题
								int headStart = line.indexOf(CharString(">")) + 1;
								int headEnd = line.indexOf(CharString("</h1"));
								info << *(line.substring(headEnd - headStart, headStart)) << endl;
							}
							else if (line.isNewsSource(lastPos) != -1) { //如果是来源
								int sourceStart = line.isNewsSource(lastPos);
								int sourceEnd = line.indexOf(CharString("</"), sourceStart);
								info << *(line.substring(sourceEnd - sourceStart, sourceStart)) << endl;
							}
							if (line.isTime(lastPos) != -1) {   //如果是时间
								info << *(line.substring(19, line.isTime(lastPos))) << endl;
								timeFound = true;
							}
							tags.pop_back();
						}
                    }
                }
                if (line.frontTag().filteredTag())
                    continue;
				int lastPos = 0;
                while (inText) {    //处理正文
                    if (line.nextTag(lastPos) == CharString("<div") && !lastPos)
                        contentsStack.push_back(CharString("<div"));
                    else if (line.nextTag(lastPos) == CharString("</div") && !lastPos) {
                        contentsStack.pop_back();
                        info << endl;
                    }
                    int textStart = line.indexOf(CharString(">"), lastPos) + 1;
                    if (textStart < lastPos)    //防止反复读取一行
                        break;
                    int textEnd = line.indexOf(CharString("<"), textStart);
                    if (textEnd == -1 && textStart < line.length()) //如果行尾还没有结束内容
                        textEnd = line.length();
					
                    if (contentsStack.empty()) {
                        inText = false;
						break;
                    }
					
                    if (textEnd == -1)  //若当前行读完，开始下一行
                        break;
                    lastPos = textEnd;
                    CharString nextTag(line.nextTag(lastPos++));
                    if (textEnd != textStart) {   //有内容才输出
                        info << *(line.substring(textEnd - textStart, textStart));
                        if (nextTag == CharString("</p"))
                            info << endl;
                    }
                    if (nextTag == CharString("<div"))
                        contentsStack.push_back(CharString("<div"));
					else if (nextTag == CharString("</div")) {
						contentsStack.pop_back();
					}
                    else if (nextTag == CharString(""))
                        break;
                    if (contentsStack.empty()) {
                        inText = false;
                    }
                }
                while (inTextArea) {    //处理图集
                    if (line.frontTag() == CharString("<textarea") && !lastPos)
                        contentsStack.push_back(CharString("<textarea"));
                    else if (line.nextTag(lastPos) == CharString("</textarea")) {
                        contentsStack.pop_back();
                        info << endl;
                    }
                    int textStart = line.indexOf(CharString(">"), lastPos) + 1;
                    int textEnd = line.indexOf(CharString("<"), textStart);
                    if (textEnd == -1)  //若当前行读完，开始下一行
                        break;
                    lastPos = textEnd;
                    CharString nextTag(line.nextTag(lastPos++));
                    if (textEnd != textStart) {   //有内容才输出
                        info << *(line.substring(textEnd - textStart, textStart));
                        if (nextTag == CharString("</p"))
                            info << endl;
                    }
                    if (nextTag == CharString("<textarea"))
                        contentsStack.push_back(CharString("<textarea"));
                    else if (nextTag == CharString("</textarea"))
                        contentsStack.pop_back();
                    else if (nextTag == CharString(""))
                        break;
                    if (contentsStack.empty()) {
                        inTextArea = false;
                    }
                }
            }
        } else if (line.frontTag() == startBody) {
            inBody = true;
        } else if (line.frontTag() == CharString("</script")) {
            inScript = false;
        }
    }
}

//正向最大匹配分词算法
CharStringLink *divideWords(CharString& fileName, hashTable& dic) {
    auto wordsList = new CharStringLink;
    ifstream info("./output/" + fileName.remove_htmlSuffix()->getContentString() + ".info");
    if (!info)
        return nullptr;
    string contentLine;
    bool start = false;
	if (!timeNotFound.empty() && timeNotFound.frontContent() == fileName.remove_htmlSuffix()->getContentInt()) {
		start = true;
		timeNotFound.pop_front();
	}
    while (getline(info, contentLine)) {
        CharString line(contentLine);   //立即保存为自己结构
        if (!start && ((line[0] == '2' && line[1] == '0') || (line[0] == '1' && line[2] == '9'))) {
            start = true;
            continue;
        }
        if (!start)
            continue;
		unsigned int startLength(maxWordLength + 2);
        while (line.length() > 3) {    //若剩下的子串为单字或全空
            startLength -= 2;       //逐步缩小长度
            if (startLength > line.length())
                startLength = line.length();
            else if (startLength < 4) {   //如果剩下非词单字
				auto temp = line.substring(SIZE_MAX, (int)startLength); //更新当前字符串
				if (!temp)
					break;
				line = *temp;
                startLength = maxWordLength;
            }
            auto part = line.substring(startLength, 0);    //从头部开始匹配
            if (dic.contained(part->getContentString())) {  //如果分词成功
                wordsList->push_back(*part);
				auto temp = line.substring(SIZE_MAX, (int)startLength); //更新当前字符串
				if (!temp)
					break;
				line = *temp;
				startLength = maxWordLength + 2;
            }
        }
    }
    return wordsList;
}

//基于哈希表构建倒排文档
void extractInfo(CharString &fileName, docHash &list) {
    list.importFromDoc(fileName);
}

//基于AVL树构建倒排文档
void extractInfo(CharString &fileName, BalTree &AVLtree) {
    AVLtree.importFromDoc(fileName);
}

class relatedFileNode{
public:
    int docID = -1;
    int wordsIncluded = 1;  //出现的不同关键词越多，这个node权重越高
    int totalTimes = 0;

    double weight = 0;     //在功能二中使用，实时更新文档权重
};

void respondQuery(BalTree &reversedDocs) {
    ifstream query("./query1.txt");
    ofstream result("./result1.txt");
    if (!query)
        return;
    string oneLine;

    while (getline(query, oneLine)) {
        CharString line(oneLine);       //保存为自定义字符串

        //分词，对每一个关键词进行统计
        Queue<relatedFileNode> answers;     //存储这一行的答案：哪些网页出现了所给关键词？分别出现了几个关键词？总共出现了几次？
        CharStringLink *words = CharStringLink::divideWord(line, 0);
        CharStringNode *word = words->front;
        while (word) {
            CharString target = word->content;
            /*
             * 对于每一个关键词：在AVL树中找到对应节点的docList
             * 对于docList中每一篇文章（docListNode）
             *  如果这篇文章已经在answers中存在（docID存在），其wordsIncluded+1，totalTimes加上这个docListNode的times
             *  如果这篇文章尚不存在，新建node，将docID赋给这个relatedFileNode的docID，totalTimes赋给times，最后加入
             */
            docList *info = reversedDocs.searchWord(target);

            for (docListNode *articles = info->_front; articles; articles = articles->next) {
                //先查找这篇文章是否已经存在
                relatedFileNode *article = nullptr;
                for (QueueNode<relatedFileNode> *iter = answers._front; iter; iter = iter->next) {
                    if (iter->content.docID == articles->docID) {
                        article = &iter->content;
                        break;
                    }
                }

                if (article) {  //同一篇文章中出现了新关键词
                    ++article->wordsIncluded;
                    article->totalTimes += articles->times;
                } else {
                    article = new relatedFileNode;
                    article->docID = articles->docID;
                    article->totalTimes = articles->times;
                    answers.push_back(*article);
                }
            }
            word = word->next;
        }

        /*
         * 对answers按权重降序排序
         * 权重定义为：wordsIncluded * totalTimes
         */
        for (QueueNode<relatedFileNode> *iter_out = answers._rear; iter_out; iter_out = iter_out->prior) {
            for (QueueNode<relatedFileNode> *iter_in = answers._front; iter_in != iter_out; iter_in = iter_in->next ) {
                int weight1 = iter_in->content.wordsIncluded * iter_in->content.totalTimes;
                int weight2 = iter_in->next->content.wordsIncluded * iter_in->next->content.totalTimes;
                if (weight1 < weight2) {
                    int tempID = iter_in->content.docID;
                    int tempWords = iter_in->content.wordsIncluded;
                    int tempTimes = iter_in->content.totalTimes;

                    iter_in->content.docID = iter_in->next->content.docID;
                    iter_in->content.wordsIncluded = iter_in->next->content.wordsIncluded;
                    iter_in->content.totalTimes = iter_in->next->content.totalTimes;

                    iter_in->next->content.docID = tempID;
                    iter_in->next->content.wordsIncluded = tempWords;
                    iter_in->next->content.totalTimes = tempTimes;
                }
            }
        }

        //按顺序打印answers即可
        for (QueueNode<relatedFileNode> *iter = answers._front; iter; iter = iter->next) {
            result << "(" << iter->content.docID << "," << iter->content.totalTimes << ") ";
        }
        result << endl;

        delete words;
    }
}

void respondRecommend(BalTree &reversedDocs, CharStringLink& files) {
    ifstream query("./query2.txt");
    ofstream result("./result2.txt");
    if (!query)
        return;
    string oneLine;

    /*
     * 根据输入的新闻标题，找到对应的文章及ID(只搜索$id$.info文件的第一行)
     * 统计该篇文章中出现的高频单词
     * 利用respondQuery中的方法，找到这些高频单词对应的文章
     * 最后推荐高频词汇出现权重最高的5篇文章
     *  权重的定义类似于批量检索中的定义
     */
    while (getline(query, oneLine)) {   //对于每个待查询的新闻标题
        CharString line(oneLine);       //将所要查询的新闻标题保存为自定义字符串
        int docID = -1;
        for (size_t i = 0; i < files.length(); ++i) {   //查找对应的文章
            CharString fileName = files[i].content;
            int tempID = fileName.remove_htmlSuffix()->getContentInt();
            ifstream info("./output/" + to_string(tempID) + ".info");
            string title;
            getline(info, title);

            if (line == title) {    //此处使用了自定义的重载==运算符
                docID = tempID;
                break;
            }
        }

        if (docID == -1) {      //如果没找到对应文章
            result << "该新闻不在数据库中，无法推荐" << endl;
            continue;
        }

        //利用分词结果统计文章中高频单词
        auto highFrequencyWords = new wordList(docID);
        ifstream txt("./output/" + to_string(docID) + ".txt");
        string wordLine;
        while (getline(txt, wordLine)) {
            CharString word(wordLine);
            highFrequencyWords->add(word);
        }
        highFrequencyWords->updateOrder();

        //找到高频单词所对应的文章
        Queue<relatedFileNode> answers;     //存储所有高频单词的答案：哪些网页出现了所给关键词？分别出现了几个关键词？总共出现了几次？
        wordListNode *word = highFrequencyWords->_front;
        while (word) {  //对于每一个关键词
            CharString target = word->term;
            /*
             * 对于每一个关键词：在AVL树中找到对应节点的docList
             * 对于docList中每一篇文章（docListNode）
             *  如果这篇文章已经在answers中存在（docID存在），更新其权重
             *  如果这篇文章尚不存在，新建node，将docID赋给这个relatedFileNode的docID，totalTimes赋给times，最后加入
             */
            docList *info = reversedDocs.searchWord(target);

            for (docListNode *articles = info->_front; articles; articles = articles->next) {   //对于docList中每一篇文章
                //先查找这篇文章是否已经存在
                relatedFileNode *article = nullptr;
                for (QueueNode<relatedFileNode> *iter = answers._front; iter; iter = iter->next) {
                    if (iter->content.docID == articles->docID) {
                        article = &iter->content;
                        break;
                    }
                }

                if (article) {  //同一篇文章中出现了新关键词，更新该文档的权重
                    article->weight += word->times * articles->times;
                } else {
                    article = new relatedFileNode;
                    article->docID = articles->docID;
                    article->totalTimes = articles->times;
                    article->weight = word->times * articles->times;
                    answers.push_back(*article);
                }
            }
            word = word->next;
        }

        /*
         * 最后推荐权重最高的5篇（可能更少）文章
         * 对answers按权重降序排序
         * 权重定义为：Sigma(原文档词频 * 新文章词频)，对关键词求和
         */
        for (QueueNode<relatedFileNode> *iter_out = answers._rear; iter_out; iter_out = iter_out->prior) {
            for (QueueNode<relatedFileNode> *iter_in = answers._front; iter_in != iter_out; iter_in = iter_in->next ) {
                if (iter_in->content.weight < iter_in->next->content.weight) {
                    int tempID = iter_in->content.docID;
                    int tempWords = iter_in->content.wordsIncluded;
                    int tempTimes = iter_in->content.totalTimes;
                    int tempWeight = iter_in->content.weight;

                    iter_in->content.docID = iter_in->next->content.docID;
                    iter_in->content.wordsIncluded = iter_in->next->content.wordsIncluded;
                    iter_in->content.totalTimes = iter_in->next->content.totalTimes;
                    iter_in->content.weight = iter_in->next->content.weight;

                    iter_in->next->content.docID = tempID;
                    iter_in->next->content.wordsIncluded = tempWords;
                    iter_in->next->content.totalTimes = tempTimes;
                    iter_in->next->content.weight = tempWeight;
                }
            }
        }

        //按顺序打印answers即可，注意屏蔽输入的文章
        int count = 0;
        for (QueueNode<relatedFileNode> *iter = answers._front; iter && count < 5; iter = iter->next) {
            ifstream info("./output/" + to_string(iter->content.docID) + ".info");
            if (!info || docID == iter->content.docID)
                continue;
            string title;
            getline(info, title);
            result << "(" << iter->content.docID << "," << title << "),";
            ++count;
        }
        result << endl;

        delete highFrequencyWords;
    }
}