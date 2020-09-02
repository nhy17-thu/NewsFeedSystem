#include "CharString.h"

CharString::CharString(char *_content) {
    for (int i = 0; _content[i] != '\0'; ++i) {
        ++contentLength;
    }
    content = new char[contentLength + 1];
    for (int i = 0; i < contentLength; ++i) {
        content[i] = _content[i];
    }
    content[contentLength] = '\0';
}

CharString::CharString(const std::string& _content) {
    contentLength = _content.length();
    content = new char[contentLength + 1];
    for (int i = 0; i < contentLength; ++i) {
        content[i] = _content[i];
    }
    content[contentLength] = '\0';
}

CharString::~CharString() {
	delete[] content;
    content = nullptr;
    contentLength = 0;
}

CharString::CharString(const CharString& src)
    : contentLength(src.contentLength) {
    content = new char[contentLength + 1];
    for (int i = 0; i < contentLength; ++i) {
        content[i] = src.content[i];
    }
    content[contentLength] = '\0';
}

CharString::CharString(CharString&& src) noexcept {
    contentLength = src.contentLength;
    content = new char[contentLength + 1];
    for (int i = 0; i < contentLength; ++i) {
        content[i] = src.content[i];
    }
    content[contentLength] = '\0';
    src.content = nullptr;
    src.contentLength = 0;
}

//构造模式串p的next表
int *buildNextTable(CharString& p) {
	unsigned int m = p.length(), j = 0;  //主串指针
    int *next = new int[m];
    int t = next[0] = -1;   //模式串指针
    while (j < m - 1) {
        if (t < 0 || p[j] == p[t]) {  //匹配
            ++j;
            ++t;
            next[j] = (p[j] == p[t] ? next[t] : t);
        } else {
            t = next[t];
        }
    }
    return next;
}

//KMP算法
int CharString::indexOf(CharString&& modelStr, int startPos) {
    int *next = buildNextTable(modelStr);
    int n = (int)contentLength;
    int m = (int)modelStr.contentLength;
    int i = startPos;   //文本串指针
    int j = 0;  //模式串指针
    while (j < m && i < n) {    //从左向右逐个比较
        if (j < 0 || content[i] == modelStr.content[j]) {
            ++i;
            ++j;
        } else {
            j = next[j];    //模式串右移
        }
    }
    delete[] next;
    if (j < m) {    //如果匹配失败
        return -1;
    }
    return (i - j);
}

CharString *CharString::substring(int length = SIZE_MAX, const int startPos = 0) {
	if (startPos >= contentLength)
		return nullptr;
    if (length > contentLength - startPos)
        length = contentLength - startPos;
    char *tempContent = new char[length + 1];
    for (int i = 0; i < length; ++i) {
        tempContent[i] = content[startPos + i];
    }
    tempContent[length] = '\0';
    return new CharString(tempContent);
}

CharString *CharString::concat(const CharString& left, const CharString& right) {
    auto *tempString = new CharString;
	unsigned int lLength(left.contentLength), rLength(right.contentLength);
    tempString->contentLength = lLength + rLength;
    tempString->content = new char[tempString->contentLength + 1];
    for (int i = 0; i < lLength; ++i) {
        tempString->content[i] = left.content[i];
    }
    for (int i = 0; i < rLength; ++i) {
        tempString->content[lLength + i] = right.content[i];
    }
    tempString->content[tempString->contentLength] = '\0';
    return tempString;
}

CharString& CharString::operator=(const CharString &right) {
    if (this != &right) {
        delete[] content;
        contentLength = right.contentLength;
        content = new char[contentLength + 1];
        for (int i = 0; i <= contentLength; ++i) {
            content[i] = right.content[i];
        }
    }
    return *this;
}

std::ostream& operator<<(std::ostream &out, const CharString& src) {
    for (unsigned int i = 0; src.content[i] != '\0'; ++i) {
        out << src.content[i];
    }
    return out;
}

char CharString::operator[](int pos) {
    if (pos < contentLength)
        return content[pos];
    return '\0';
}

unsigned int CharString::length() {
    return contentLength;
}

bool CharString::operator==(const CharString &right) {
    if (contentLength != right.contentLength)
        return false;
    for (int i = 0; i < contentLength; ++i) {
        if (content[i] != right.content[i]) {
            return false;
        }
    }
    return true;
}

bool CharString::operator!=(const CharString &right) {
    if (*this == right)
        return false;
    else
        return true;
}

CharString& CharString::operator=(const std::string &right) {
    contentLength = right.length();
    delete content;
    content = new char[contentLength + 1];
    for (int i = 0; i < contentLength; ++i) {
        content[i] = right[i];
    }
    content[contentLength] = '\0';
	return *this;
}

std::string CharString::getContentString() {
	std::string temp(content);
    return temp;
}

int CharString::getContentInt() {
	std::string temp(content);
	int ans = std::stoi(temp);
	return ans;
}

CharString* CharString::remove_htmlSuffix() {
    return substring(contentLength - 5);
}

//形如<div要和</div匹配，<p和</p
bool CharString::pairedTags(CharString &start, CharString &end) {
    if (start.contentLength + 1 != end.contentLength || end[1] != '/') {
        return false;
    } else {
        for (unsigned int i = 1; start[i] != '\0' && end[i + 1] != '\0'; ++i) {
            if (start[i] != end[i + 1]) {
                return false;
            }
        }
        return true;
    }
}

bool CharString::matchable() {
    if (*this == CharString("<h1")
    || *this == CharString("<div")
    || *this == CharString("<a")
    || *this == CharString("<p")) {
        return true;
    } else
        return false;
}

//从startPos开始，在下一个">"之前有id="ne_article_source"或class="p1"或class="p2"
int CharString::isNewsSource(int startPos) {
    int sourcePos = indexOf(CharString("id=\"ne_article_source\""), startPos);
    if (sourcePos == -1) {
        sourcePos = indexOf(CharString("class=\"p1\""), startPos);
    }
    if (sourcePos == -1) {
        sourcePos = indexOf(CharString("class=\"p2\""), startPos);
    }
    /*
    if (sourcePos == -1) {
        sourcePos = indexOf(CharString("本文来源"), 0);
    }
    */
    int nextEnd = indexOf(CharString(">"), startPos);
    /*
    if (sourcePos != -1)
        nextEnd = indexOf(CharString(">"), sourcePos);
    else
        nextEnd = indexOf(CharString(">"), startPos);
    */
    if (sourcePos != -1 && sourcePos < nextEnd) {
        return nextEnd + 1;
    }
    return -1;
}

bool CharString::singleTag() {
    if (*this == CharString("<link")
        || *this == CharString("<base")
        || *this == CharString("<meta")
        || *this == CharString("<input")
        || *this == CharString("<img")
		|| *this == CharString("<!--")
        ) {
        return true;
    } else
        return false;
}

bool CharString::operator==(const std::string &right) {
    if (contentLength != right.length())
        return false;
    for (int i = 0; i < contentLength; ++i) {
        if (content[i] != right[i]) {
            return false;
        }
    }
    return true;
}

int CharString::isText(int startPos) {
    int nextEnd = indexOf(CharString(">"), startPos);
    int sourcePos = indexOf(CharString("id=\"endText\""), startPos);
    if (sourcePos != -1 && sourcePos < nextEnd) {
        return nextEnd + 1;
    }
    return -1;
}

CharString CharString::nextTag(int _startPos) {
    int startPos = indexOf(CharString("<"), _startPos);
    if (startPos == -1) {
        return CharString("");
    }
    int nextSpaceClose = indexOf(CharString(" "), startPos);
    int nextTagClose = indexOf(CharString(">"), startPos);
	int nextClose = 0;
    if (nextSpaceClose != -1 && nextTagClose != -1)
        nextClose = nextSpaceClose > nextTagClose ? nextTagClose : nextSpaceClose;
    if (nextSpaceClose != -1 && nextTagClose == -1)
        nextClose = nextSpaceClose;
    else if (nextSpaceClose == -1 && nextTagClose != -1)
        nextClose = nextTagClose;
    return *substring(nextClose - startPos, startPos);
}

CharString CharString::frontTag() {
    return nextTag(0);
}

bool CharString::filteredTag() {
    if (*this == CharString("<style")
    || *this == CharString("<i")
    || *this == CharString("<h2")
	|| *this == CharString("<w")) {
        return true;
    } else
        return false;
}

int CharString::isTime(int startPos) {
    int nextEnd = indexOf(CharString(">"), startPos);
    int sourcePos = indexOf(CharString("id=\"ne_article_source\""), startPos);
    if (sourcePos == -1) {
        sourcePos = indexOf(CharString("class=\"ptime\""), startPos);
    }
    int time(indexOf(CharString("20")));
    if (time == -1)
        time = indexOf(CharString("19"));
    if (sourcePos != -1 && sourcePos < nextEnd) {
        return time;
    }
    return -1;
}

char* CharString::getCharPointer() {
    char *temp = new char[contentLength + 1];
    for (unsigned int i = 0; i < contentLength; ++i) {
        temp[i] = content[i];
    }
    temp[contentLength] = '\0';
    return temp;
}

bool CharString::startWithOmission() {
	if (this->indexOf(CharString("<!-- 正文 -->")) == 1
		|| this->indexOf(CharString("<!-- 图集结束")) == 0) {
        return true;
    }
    return false;
}
