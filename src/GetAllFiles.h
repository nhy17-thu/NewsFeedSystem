#ifndef NEWSFEEDSYSTEM_GETALLFILES_H
#define NEWSFEEDSYSTEM_GETALLFILES_H

//MacOS下包含<sys/uio.h>，Windows下包含<io.h>
#include <iostream>
#include <vector>
#include <string>
#include "CharStringLink.h"
#ifdef __APPLE__
#include <sys/uio.h>
#define MAX_PATH 100
#elif defined _WIN32 || defined _WIN64
	#include <io.h>
	#include <Windows.h>
#endif

std::string ExePath();
void GetAllFiles(CharStringLink& files);

#endif //NEWSFEEDSYSTEM_GETALLFILES_H
