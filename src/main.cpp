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

void extractInfo(CharString& fileName, docHash& list);          //���ڹ�ϣ���������ĵ�
void extractInfo(CharString& fileName, BalTree& AVLtree);       //����AVL�����������ĵ�
void respondQuery(BalTree &reversedDocs);
void respondRecommend(BalTree &reversedDocs, CharStringLink& files);

Queue<int> timeNotFound;

int main() {
	hashTable hash_dictionary;
	initDictionary(hash_dictionary);

    CharStringLink files;
    //�˴�ɨ���õ�ǰ�ļ����������ļ���
	GetAllFiles(files);
    size_t length = files.length();

    /*
     * �ⲿ��ע�͵���Ϊʵ��һ����
    for (size_t i = 0; i < length; ++i) {
		//ɨ��ȫ��
        printHTML2txt(files[i].content);
    }
	
	for (size_t i = 0; i < length; ++i) {
		//�ִ�
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

    BalTree reversedDocs;   //����AVL�����������ĵ�
    for (size_t i = 0; i < length; ++i) {
        extractInfo(files[i].content, reversedDocs);
    }

    //ʵ��������ѯ����
    respondQuery(reversedDocs);

    //ʵ�������Ƽ�����
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
        dic.insert(_line);  //����Ϊ�ֵ�
    }
}

void printHTML2txt(CharString &fileName) {
    ifstream html("./input/" + fileName.getContentString());    //�򿪶�Ӧ�ĵ�
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

    while (getline(html, _line)) { //ÿ�ζ���һ��
        ++countLine;
        CharString line(_line); //���̱���Ϊ�Զ����ַ�������
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
                while (!inText && !inTextArea) {  //����ǰ�����б�ǩ,ע��indexOf�Ҳ�����ʱ�򷵻�-1
                    int nextStart = line.indexOf(CharString("<"), currentPos + 1);
                    if (nextStart == -1)    //���ʣ��û�б�ǩ��
                        break;
                    int nextEnd = line.indexOf(CharString("</"), currentPos + 1);
                    if (nextStart < nextEnd || nextEnd == -1) {  //�����ʼ���±�ǩ��ѹջ
                        currentPos = nextStart;
                        if (line.isText(currentPos) != -1) {    //�����ǰ�±�ǩ��־����
                            inText = true;
                            break;
                        } else if (line.indexOf(CharString("<textarea")) != -1) {   //�����־ͼ��
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
                                tags.push_back(*temp);  //ѹ���tags����<div, <p
                            }
                        } else {
                            continue;
                        }
                    } else {    //���������ԭ���ı�ǩ���ж��Ƿ���Ҫ��ȡ����λ�ü����ݣ�Ȼ���ٳ�ջ����û���±�ǩ�ˣ�
                        int lastPos(currentPos);
                        currentPos = nextEnd;
						if (!tags.empty()) {
							CharString top(tags.top());
							//��������<h1...>XXX</h1>
							//����<a...>XXX</a>�����м��XXX������
							//������ҵ���Ӧ��id�����ұ�ǩ��Ӧ����pairedTags��������������">"��"<"λ�ü������
							//�ҵ�id���룺��ջ��λ�ã�lastPos����ʼ������һ��'>'֮ǰ���ҵ�id="tagsWeWant"
							if (top == "<h1") { //����Ǳ���
								int headStart = line.indexOf(CharString(">")) + 1;
								int headEnd = line.indexOf(CharString("</h1"));
								info << *(line.substring(headEnd - headStart, headStart)) << endl;
							}
							else if (line.isNewsSource(lastPos) != -1) { //�������Դ
								int sourceStart = line.isNewsSource(lastPos);
								int sourceEnd = line.indexOf(CharString("</"), sourceStart);
								info << *(line.substring(sourceEnd - sourceStart, sourceStart)) << endl;
							}
							if (line.isTime(lastPos) != -1) {   //�����ʱ��
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
                while (inText) {    //��������
                    if (line.nextTag(lastPos) == CharString("<div") && !lastPos)
                        contentsStack.push_back(CharString("<div"));
                    else if (line.nextTag(lastPos) == CharString("</div") && !lastPos) {
                        contentsStack.pop_back();
                        info << endl;
                    }
                    int textStart = line.indexOf(CharString(">"), lastPos) + 1;
                    if (textStart < lastPos)    //��ֹ������ȡһ��
                        break;
                    int textEnd = line.indexOf(CharString("<"), textStart);
                    if (textEnd == -1 && textStart < line.length()) //�����β��û�н�������
                        textEnd = line.length();
					
                    if (contentsStack.empty()) {
                        inText = false;
						break;
                    }
					
                    if (textEnd == -1)  //����ǰ�ж��꣬��ʼ��һ��
                        break;
                    lastPos = textEnd;
                    CharString nextTag(line.nextTag(lastPos++));
                    if (textEnd != textStart) {   //�����ݲ����
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
                while (inTextArea) {    //����ͼ��
                    if (line.frontTag() == CharString("<textarea") && !lastPos)
                        contentsStack.push_back(CharString("<textarea"));
                    else if (line.nextTag(lastPos) == CharString("</textarea")) {
                        contentsStack.pop_back();
                        info << endl;
                    }
                    int textStart = line.indexOf(CharString(">"), lastPos) + 1;
                    int textEnd = line.indexOf(CharString("<"), textStart);
                    if (textEnd == -1)  //����ǰ�ж��꣬��ʼ��һ��
                        break;
                    lastPos = textEnd;
                    CharString nextTag(line.nextTag(lastPos++));
                    if (textEnd != textStart) {   //�����ݲ����
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

//�������ƥ��ִ��㷨
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
        CharString line(contentLine);   //��������Ϊ�Լ��ṹ
        if (!start && ((line[0] == '2' && line[1] == '0') || (line[0] == '1' && line[2] == '9'))) {
            start = true;
            continue;
        }
        if (!start)
            continue;
		unsigned int startLength(maxWordLength + 2);
        while (line.length() > 3) {    //��ʣ�µ��Ӵ�Ϊ���ֻ�ȫ��
            startLength -= 2;       //����С����
            if (startLength > line.length())
                startLength = line.length();
            else if (startLength < 4) {   //���ʣ�·Ǵʵ���
				auto temp = line.substring(SIZE_MAX, (int)startLength); //���µ�ǰ�ַ���
				if (!temp)
					break;
				line = *temp;
                startLength = maxWordLength;
            }
            auto part = line.substring(startLength, 0);    //��ͷ����ʼƥ��
            if (dic.contained(part->getContentString())) {  //����ִʳɹ�
                wordsList->push_back(*part);
				auto temp = line.substring(SIZE_MAX, (int)startLength); //���µ�ǰ�ַ���
				if (!temp)
					break;
				line = *temp;
				startLength = maxWordLength + 2;
            }
        }
    }
    return wordsList;
}

//���ڹ�ϣ���������ĵ�
void extractInfo(CharString &fileName, docHash &list) {
    list.importFromDoc(fileName);
}

//����AVL�����������ĵ�
void extractInfo(CharString &fileName, BalTree &AVLtree) {
    AVLtree.importFromDoc(fileName);
}

class relatedFileNode{
public:
    int docID = -1;
    int wordsIncluded = 1;  //���ֵĲ�ͬ�ؼ���Խ�࣬���nodeȨ��Խ��
    int totalTimes = 0;

    double weight = 0;     //�ڹ��ܶ���ʹ�ã�ʵʱ�����ĵ�Ȩ��
};

void respondQuery(BalTree &reversedDocs) {
    ifstream query("./query1.txt");
    ofstream result("./result1.txt");
    if (!query)
        return;
    string oneLine;

    while (getline(query, oneLine)) {
        CharString line(oneLine);       //����Ϊ�Զ����ַ���

        //�ִʣ���ÿһ���ؼ��ʽ���ͳ��
        Queue<relatedFileNode> answers;     //�洢��һ�еĴ𰸣���Щ��ҳ�����������ؼ��ʣ��ֱ�����˼����ؼ��ʣ��ܹ������˼��Σ�
        CharStringLink *words = CharStringLink::divideWord(line, 0);
        CharStringNode *word = words->front;
        while (word) {
            CharString target = word->content;
            /*
             * ����ÿһ���ؼ��ʣ���AVL�����ҵ���Ӧ�ڵ��docList
             * ����docList��ÿһƪ���£�docListNode��
             *  �����ƪ�����Ѿ���answers�д��ڣ�docID���ڣ�����wordsIncluded+1��totalTimes�������docListNode��times
             *  �����ƪ�����в����ڣ��½�node����docID�������relatedFileNode��docID��totalTimes����times��������
             */
            docList *info = reversedDocs.searchWord(target);

            for (docListNode *articles = info->_front; articles; articles = articles->next) {
                //�Ȳ�����ƪ�����Ƿ��Ѿ�����
                relatedFileNode *article = nullptr;
                for (QueueNode<relatedFileNode> *iter = answers._front; iter; iter = iter->next) {
                    if (iter->content.docID == articles->docID) {
                        article = &iter->content;
                        break;
                    }
                }

                if (article) {  //ͬһƪ�����г������¹ؼ���
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
         * ��answers��Ȩ�ؽ�������
         * Ȩ�ض���Ϊ��wordsIncluded * totalTimes
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

        //��˳���ӡanswers����
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
     * ������������ű��⣬�ҵ���Ӧ�����¼�ID(ֻ����$id$.info�ļ��ĵ�һ��)
     * ͳ�Ƹ�ƪ�����г��ֵĸ�Ƶ����
     * ����respondQuery�еķ������ҵ���Щ��Ƶ���ʶ�Ӧ������
     * ����Ƽ���Ƶ�ʻ����Ȩ����ߵ�5ƪ����
     *  Ȩ�صĶ������������������еĶ���
     */
    while (getline(query, oneLine)) {   //����ÿ������ѯ�����ű���
        CharString line(oneLine);       //����Ҫ��ѯ�����ű��Ᵽ��Ϊ�Զ����ַ���
        int docID = -1;
        for (size_t i = 0; i < files.length(); ++i) {   //���Ҷ�Ӧ������
            CharString fileName = files[i].content;
            int tempID = fileName.remove_htmlSuffix()->getContentInt();
            ifstream info("./output/" + to_string(tempID) + ".info");
            string title;
            getline(info, title);

            if (line == title) {    //�˴�ʹ�����Զ��������==�����
                docID = tempID;
                break;
            }
        }

        if (docID == -1) {      //���û�ҵ���Ӧ����
            result << "�����Ų������ݿ��У��޷��Ƽ�" << endl;
            continue;
        }

        //���÷ִʽ��ͳ�������и�Ƶ����
        auto highFrequencyWords = new wordList(docID);
        ifstream txt("./output/" + to_string(docID) + ".txt");
        string wordLine;
        while (getline(txt, wordLine)) {
            CharString word(wordLine);
            highFrequencyWords->add(word);
        }
        highFrequencyWords->updateOrder();

        //�ҵ���Ƶ��������Ӧ������
        Queue<relatedFileNode> answers;     //�洢���и�Ƶ���ʵĴ𰸣���Щ��ҳ�����������ؼ��ʣ��ֱ�����˼����ؼ��ʣ��ܹ������˼��Σ�
        wordListNode *word = highFrequencyWords->_front;
        while (word) {  //����ÿһ���ؼ���
            CharString target = word->term;
            /*
             * ����ÿһ���ؼ��ʣ���AVL�����ҵ���Ӧ�ڵ��docList
             * ����docList��ÿһƪ���£�docListNode��
             *  �����ƪ�����Ѿ���answers�д��ڣ�docID���ڣ���������Ȩ��
             *  �����ƪ�����в����ڣ��½�node����docID�������relatedFileNode��docID��totalTimes����times��������
             */
            docList *info = reversedDocs.searchWord(target);

            for (docListNode *articles = info->_front; articles; articles = articles->next) {   //����docList��ÿһƪ����
                //�Ȳ�����ƪ�����Ƿ��Ѿ�����
                relatedFileNode *article = nullptr;
                for (QueueNode<relatedFileNode> *iter = answers._front; iter; iter = iter->next) {
                    if (iter->content.docID == articles->docID) {
                        article = &iter->content;
                        break;
                    }
                }

                if (article) {  //ͬһƪ�����г������¹ؼ��ʣ����¸��ĵ���Ȩ��
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
         * ����Ƽ�Ȩ����ߵ�5ƪ�����ܸ��٣�����
         * ��answers��Ȩ�ؽ�������
         * Ȩ�ض���Ϊ��Sigma(ԭ�ĵ���Ƶ * �����´�Ƶ)���Թؼ������
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

        //��˳���ӡanswers���ɣ�ע���������������
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