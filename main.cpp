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
    //todo: 获取可执行程序同级目录下的所有文件
/*
    CharStringLink files;
    //此处扫描获得当前文件夹下所有文件名
    size_t length = files.length();
    for (size_t i = 0; i < length; ++i) {
        printHTML2txt(files[i].content);
    }
     */
    hashTable hash_dictionary;
    initDictionary(hash_dictionary);
    for (int i = 0; i < 20; ++i) {
        //扫描全文
        CharString temp(to_string(i) + ".html");
        printHTML2txt(temp);
        //分词
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
        dic.insert(_line);  //保存为字典
    }
}

void printHTML2txt(CharString& fileName) {
    ifstream html("./input/" + fileName.getContentString());    //打开对应文档
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

    while (getline(html, _line)) { //每次读入一行
        ++countLine;

        if (countLine == 164) {
            ;
        }

        CharString line(_line); //立刻保存为自定义字符串类型
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
                            if (*temp == "<script") {
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
                        CharString top(tags.top());
                        //标题名：<h1...>XXX</h1>
                        //若有<a...>XXX</a>，则中间的XXX不换行
                        //如果能找到对应的id编码且标签对应（用pairedTags函数），就输入">"到"<"位置间的内容
                        //找到id编码：从栈顶位置（lastPos）开始，在下一个'>'之前能找到id="tagsWeWant"
                        if (top == "<h1") { //如果是标题
                            int headStart = line.indexOf(CharString(">")) + 1;
                            int headEnd = line.indexOf(CharString("</h1"));
                            info << *(line.substring(headEnd - headStart, headStart)) << endl;
                        } else if (line.isNewsSource(lastPos) != -1) { //如果是来源
                            int sourceStart = line.isNewsSource(lastPos);
                            int sourceEnd = line.indexOf(CharString("</"), sourceStart);
                            info << *(line.substring(sourceEnd - sourceStart, sourceStart)) << endl;
                        }
                        if (line.isTime(lastPos) != -1) {   //如果是时间
                            info << *(line.substring(19, line.isTime(lastPos))) << endl;
                        }
                        tags.pop_back();
                    }
                }
                int lastPos = 0;
                if (line.frontTag().filteredTag())
                    continue;
                while (inText) {    //处理正文
                    if (line.frontTag() == "<div" && !lastPos)
                        contentsStack.push_back(CharString("<div"));
                    else if (line.nextTag(lastPos) == "</div") {
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
                while (inTextArea) {    //处理图集
                    if (line.frontTag() == "<textarea" && !lastPos)
                        contentsStack.push_back(CharString("<textarea"));
                    else if (line.nextTag(lastPos) == "</textarea") {
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

//正向最大匹配分词算法
CharStringLink *divideWords(CharString& fileName, hashTable& dic) {
    auto wordsList = new CharStringLink;
    ifstream info("./output/" + fileName.remove_htmlSuffix()->getContentString() + ".info");
    if (!info)
        return nullptr;
    string contentLine;
    bool start = false;
    while (getline(info, contentLine)) {
        CharString line(contentLine);   //立即保存为自己结构
        if (!start && ((line[0] == '2' && line[1] == '0') || (line[0] == '1' && line[2] == '9'))) {
            start = true;
            continue;
        }
        if (!start)
            continue;
        size_t startLength(maxWordLength + 2);
        while (line.length() > 3) {    //若剩下的子串为单字或全空
            startLength -= 2;       //逐步缩小长度
            if (startLength > line.length())
                startLength = line.length();
            else if (startLength < 4) {   //如果剩下非词单字
                line = *line.substring(SIZE_MAX, (int)startLength); //更新当前字符串
                startLength = maxWordLength;
            }
            auto part = line.substring(startLength, 0);    //从头部开始匹配
            if (dic.contained(part->getContentString())) {  //如果分词成功
                wordsList->push_back(*part);
                line = *line.substring(SIZE_MAX, (int)startLength); //更新当前字符串
                startLength = maxWordLength + 2;
            }
        }
    }
    return wordsList;
}
