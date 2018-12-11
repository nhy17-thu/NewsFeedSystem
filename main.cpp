#include <iostream>
#include <fstream>
#include "Stack.h"
#include "CharString.h"
#include "CharStringLink.h"
#include "HashTable.h"
using namespace std;

void printHTML2txt(CharString& fileName);
void initDictionary(hashTable& dic);
CharStringLink *divideWords(CharString& fileName, hashTable& dic);
#define maxWordLength 20

int main() {
    //todo: ��ȡ��ִ�г���ͬ��Ŀ¼�µ������ļ�
/*
    CharStringLink files;
    //�˴�ɨ���õ�ǰ�ļ����������ļ���
    size_t length = files.length();
    for (size_t i = 0; i < length; ++i) {
        printHTML2txt(files[i].content);
    }
     */
    hashTable hash_dictionary;
    initDictionary(hash_dictionary);
    for (int i = 0; i < 20; ++i) {
        //ɨ��ȫ��
        CharString temp(to_string(i) + ".html");
        printHTML2txt(temp);
        //�ִ�
        auto wordsList = divideWords(temp, hash_dictionary);
        ofstream info("./output/" + to_string(i) + ".txt");
        CharString empty("");
        while (wordsList->length()) {
            CharString temp = wordsList->front->content;
            if (temp != empty)
                info << temp << endl;
            wordsList->pop_front();
        }
    }
    return 0;
}

void initDictionary(hashTable& dic) {
    ifstream dictionary("dictionary.dic");
    if (!dictionary)
        return;
    string _line;
    int countLine = 0;
    while (dictionary >> _line) {
        ++countLine;
        dic.insert(_line);  //����Ϊ�ֵ�
    }
}

void printHTML2txt(CharString& fileName) {
    ifstream html("./input/" + fileName.getContentString());    //�򿪶�Ӧ�ĵ�
    if (!html)
        return;
    ofstream info("./output/" + fileName.remove_htmlSuffix()->getContentString() + ".info");
    if (!info)
        return;

    string _line;
    bool inBody(false), inScript(false), inText(false), inTextArea(false);
    CharString startBody("<body"), endBody("</body");
    Stack<CharString> tags;
    Stack<CharString> contentsStack;
    int countLine = 0;

    while (getline(html, _line)) { //ÿ�ζ���һ��
        ++countLine;

        if (countLine == 164) {
            ;
        }

        CharString line(_line); //���̱���Ϊ�Զ����ַ�������
        int currentPos = -1;
        if (inBody && !inScript) {
            if (line.frontTag() == endBody) {
                return;
            } else {
                CharString frontTag = line.frontTag();
                if (frontTag.singleTag() || !line.frontTag().length()) {
                    continue;
                }
                if (line.frontTag() == "<script") {
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
                            if (*temp == "<script") {
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
                        CharString top(tags.top());
                        //��������<h1...>XXX</h1>
                        //����<a...>XXX</a>�����м��XXX������
                        //������ҵ���Ӧ��id�����ұ�ǩ��Ӧ����pairedTags��������������">"��"<"λ�ü������
                        //�ҵ�id���룺��ջ��λ�ã�lastPos����ʼ������һ��'>'֮ǰ���ҵ�id="tagsWeWant"
                        if (top == "<h1") { //����Ǳ���
                            int headStart = line.indexOf(CharString(">")) + 1;
                            int headEnd = line.indexOf(CharString("</h1"));
                            info << *(line.substring(headEnd - headStart, headStart)) << endl;
                        } else if (line.isNewsSource(lastPos) != -1) { //�������Դ
                            int sourceStart = line.isNewsSource(lastPos);
                            int sourceEnd = line.indexOf(CharString("</"), sourceStart);
                            info << *(line.substring(sourceEnd - sourceStart, sourceStart)) << endl;
                        }
                        if (line.isTime(lastPos) != -1) {   //�����ʱ��
                            info << *(line.substring(19, line.isTime(lastPos))) << endl;
                        }
                        tags.pop_back();
                    }
                }
                int lastPos = 0;
                if (line.frontTag().filteredTag())
                    continue;
                while (inText) {    //��������
                    if (line.frontTag() == "<div" && !lastPos)
                        contentsStack.push_back(CharString("<div"));
                    else if (line.nextTag(lastPos) == "</div") {
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
                        if (nextTag == "</p")
                            info << endl;
                    }
                    if (nextTag == "<div")
                        contentsStack.push_back(CharString("<div"));
                    else if (nextTag == "</div")
                        contentsStack.pop_back();
                    else if (nextTag == "")
                        break;
                    if (contentsStack.empty()) {
                        inText = false;
                    }
                }
                while (inTextArea) {    //����ͼ��
                    if (line.frontTag() == "<textarea" && !lastPos)
                        contentsStack.push_back(CharString("<textarea"));
                    else if (line.nextTag(lastPos) == "</textarea") {
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
                        if (nextTag == "</p")
                            info << endl;
                    }
                    if (nextTag == "<textarea")
                        contentsStack.push_back(CharString("<textarea"));
                    else if (nextTag == "</textarea")
                        contentsStack.pop_back();
                    else if (nextTag == "")
                        break;
                    if (contentsStack.empty()) {
                        inTextArea = false;
                    }
                }
            }
        } else if (line.frontTag() == startBody) {
            inBody = true;
        } else if (line.frontTag() == "</script") {
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
    while (getline(info, contentLine)) {
        CharString line(contentLine);   //��������Ϊ�Լ��ṹ
        if (!start && ((line[0] == '2' && line[1] == '0') || (line[0] == '1' && line[2] == '9'))) {
            start = true;
            continue;
        }
        if (!start)
            continue;
        size_t startLength(maxWordLength + 2);
        while (line.length() > 3) {    //��ʣ�µ��Ӵ�Ϊ���ֻ�ȫ��
            startLength -= 2;       //����С����
            if (startLength > line.length())
                startLength = line.length();
            else if (startLength < 4) {   //���ʣ�·Ǵʵ���
                line = *line.substring(SIZE_MAX, (int)startLength); //���µ�ǰ�ַ���
                startLength = maxWordLength;
            }
            auto part = line.substring(startLength, 0);    //��ͷ����ʼƥ��
            if (dic.contained(part->getContentString())) {  //����ִʳɹ�
                wordsList->push_back(*part);
                line = *line.substring(SIZE_MAX, (int)startLength); //���µ�ǰ�ַ���
                startLength = maxWordLength + 2;
            }
        }
    }
    return wordsList;
}
