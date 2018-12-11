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

//����ģʽ��p��next��
int *buildNextTable(CharString& p) {
    size_t m = p.length(), j = 0;  //����ָ��
    int *next = new int[m];
    int t = next[0] = -1;   //ģʽ��ָ��
    while (j < m - 1) {
        if (t < 0 || p[j] == p[t]) {  //ƥ��
            ++j;
            ++t;
            next[j] = (p[j] == p[t] ? next[t] : t);
        } else {
            t = next[t];
        }
    }
    return next;
}

//KMP�㷨
int CharString::indexOf(CharString&& modelStr, int startPos) {
    int *next = buildNextTable(modelStr);
    int n = (int)contentLength;
    int m = (int)modelStr.contentLength;
    int i = startPos;   //�ı���ָ��
    int j = 0;  //ģʽ��ָ��
    while (j < m && i < n) {    //������������Ƚ�
        if (j < 0 || content[i] == modelStr.content[j]) {
            ++i;
            ++j;
        } else {
            j = next[j];    //ģʽ������
        }
    }
    delete[] next;
    if (j < m) {    //���ƥ��ʧ��
        return -1;
    }
    return (i - j);
}

CharString *CharString::substring(size_t length = SIZE_MAX, const int startPos = 0) {
    if (length > contentLength)
        length = contentLength;
    char *tempContent = new char[length + 1];
    for (int i = 0; i < length; ++i) {
        tempContent[i] = content[startPos + i];
    }
    tempContent[length] = '\0';
    return new CharString(tempContent);
}

CharString *CharString::concat(const CharString& left, const CharString& right) {
    auto *tempString = new CharString;
    size_t lLength(left.contentLength), rLength(right.contentLength);
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
    for (size_t i = 0; src.content[i] != '\0'; ++i) {
        out << src.content[i];
    }
    return out;
}

char CharString::operator[](int pos) {
    if (pos < contentLength)
        return content[pos];
    return '\0';
}

size_t CharString::length() {
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
    std::string temp = content;
    return temp;
}

CharString* CharString::remove_htmlSuffix() {
    return substring(contentLength - 5);
}

//����<divҪ��</divƥ�䣬<p��</p
bool CharString::pairedTags(CharString &start, CharString &end) {
    if (start.contentLength + 1 != end.contentLength || end[1] != '/') {
        return false;
    } else {
        for (size_t i = 1; start[i] != '\0' && end[i + 1] != '\0'; ++i) {
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

//��startPos��ʼ������һ��">"֮ǰ��id="ne_article_source"��class="p1"��class="p2"
int CharString::isNewsSource(int startPos) {
    int nextEnd = indexOf(CharString(">"), startPos);
    int sourcePos = indexOf(CharString("id=\"ne_article_source\""), startPos);
    if (sourcePos == -1) {
        sourcePos = indexOf(CharString("class=\"p1\""), startPos);
    }
    if (sourcePos == -1) {
        sourcePos = indexOf(CharString("class=\"p2\""), startPos);
    }
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
    int nextClose;
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
    || *this == CharString("<h2")) {
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
    for (size_t i = 0; i < contentLength; ++i) {
        temp[i] = content[i];
    }
    temp[contentLength] = '\0';
    return temp;
}