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
    char *content = nullptr;    //�ڹ��캯���з���ռ��С
	unsigned int contentLength = 0;   //���Ȳ�������β��'\0'

    CharString() = default;     //ʹ��ʱע���ȷ���content�ռ�

public:
    explicit CharString(char *_content);
    explicit CharString(const std::string& _content);
    ~CharString();
    CharString(const CharString& src);  //�������캯��
    CharString(CharString&& src) noexcept;  //�ƶ����캯��һ�㲻���׳��쳣

    int indexOf(CharString&& modelStr, int startPos = 0);    //�����Ӵ���λ�ã����ص�һ�γ��ֵ�λ��(Ĭ�ϴ�ͷ��ʼ)��ƥ��ʧ���򷵻�-1
    CharString *substring(int length, int startPos);    //��ȡ�Ӹ���λ�ÿ�ʼ�������ȵ��Ӵ�(Ĭ�ϴ�ͷ��ʼ)
    CharString *remove_htmlSuffix();
    static CharString *concat(const CharString& left, const CharString& right);     //�������������ַ���
	unsigned int length();   //�����ַ�������
    std::string getContentString();
	int getContentInt();
    char* getCharPointer();
    bool matchable();
    bool singleTag();   //�ض���ƥ��ı�ǩ
    bool filteredTag(); //���˵��ı�ǩ
    bool startWithOmission();
    static bool pairedTags(CharString& start, CharString& end);
    int isNewsSource(int startPos);    //����ǰ�к�����Դ��Ϣ�򷵻���Դ��Ϣ��ʼ��λ�ã������򷵻�-1
    int isText(int startPos);
    int isTime(int startPos);
    CharString frontTag();  //��ȡͷ����ǩ
    CharString nextTag(int _startPos);   //���شӵ�ǰλ�ÿ�ʼ����һ����ǩ

    CharString& operator= (const CharString& right);    //��������
    CharString& operator= (const std::string& right);
    bool operator== (const CharString& right);
    bool operator== (const std::string& right);
    bool operator!= (const CharString& right);
    friend std::ostream& operator<< (std::ostream& out, const CharString& src);
    char operator[] (int pos);  //�±����أ�������Խ���򷵻�'\0'

};


#endif //NEWSFEEDSYSTEM_CHARSTRING_H
