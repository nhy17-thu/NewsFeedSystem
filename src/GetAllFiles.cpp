#include "GetAllFiles.h"
using namespace std;

string ExePath() {
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    string::size_type pos = string(buffer).find_last_of("\\/");
    return string(buffer).substr(0, pos);
}

void GetAllFiles(CharStringLink& files) {
    intptr_t hFile = 0; //�ļ����
    struct _finddata_t fileinfo;
    string p;
	cout << "Current Path: " << ExePath() << endl;
    if ((hFile = _findfirst(p.assign(ExePath()).append("\\input\\*.html").c_str(), &fileinfo)) != -1) {	//�ļ�����

        do {
            cout << fileinfo.name << endl;
            if (!(fileinfo.attrib & _A_SUBDIR)) {    //�����ļ���
                if (fileinfo.name[0] != '.')
					files.push_back(CharString(p.assign(fileinfo.name)));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}