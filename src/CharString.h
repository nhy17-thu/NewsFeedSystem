#ifndef NEWSFEEDSYSTEM_CHARSTRING_H
#define NEWSFEEDSYSTEM_CHARSTRING_H

#include <iostream>
#include <string>

class CharStringNode;
class CharStringLink;

class CharString {
    friend CharStringNode;
    friend CharStringLink;

private:
    char *content = nullptr;    //在构造函数中分配空间大小
	unsigned int contentLength = 0;   //长度不包含结尾的'\0'

    CharString() = default;     //使用时注意先分配content空间

public:
    explicit CharString(char *_content);
    explicit CharString(const std::string& _content);
    ~CharString();
    CharString(const CharString& src);  //拷贝构造函数
    CharString(CharString&& src) noexcept;  //移动构造函数一般不会抛出异常

    int indexOf(CharString&& modelStr, int startPos = 0);    //查找子串的位置，返回第一次出现的位置(默认从头开始)；匹配失败则返回-1
    CharString *substring(int length, int startPos);    //截取从给定位置开始给定长度的子串(默认从头开始)
    CharString *remove_htmlSuffix();
    static CharString *concat(const CharString& left, const CharString& right);     //连接左右两个字符串
	unsigned int length();   //返回字符串长度
    std::string getContentString();
	int getContentInt();
    char* getCharPointer();
    bool matchable();
    bool singleTag();   //必定不匹配的标签
    bool filteredTag(); //过滤掉的标签
    bool startWithOmission();
    static bool pairedTags(CharString& start, CharString& end);
    int isNewsSource(int startPos);    //若当前行含有来源信息则返回来源信息开始的位置，不含则返回-1
    int isText(int startPos);
    int isTime(int startPos);
    CharString frontTag();  //截取头部标签
    CharString nextTag(int _startPos);   //返回从当前位置开始的下一个标签

    CharString& operator= (const CharString& right);    //拷贝内容
    CharString& operator= (const std::string& right);
    bool operator== (const CharString& right);
    bool operator== (const std::string& right);
    bool operator!= (const CharString& right);
    friend std::ostream& operator<< (std::ostream& out, const CharString& src);
    char operator[] (int pos);  //下标重载，若数组越界则返回'\0'

};


#endif //NEWSFEEDSYSTEM_CHARSTRING_H
