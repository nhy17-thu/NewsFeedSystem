cmake_minimum_required(VERSION 3.11)
project(NewsFeedSystem)

set(CMAKE_CXX_STANDARD 11)

add_executable(NewsFeedSystem main.cpp Stack.h CharString.cpp CharString.h CharStringLink.cpp CharStringLink.h HashTable.h Queue.h HashTable.cpp BalanceTree.cpp BalanceTree.h docListHash.cpp docListHash.h docList.cpp docList.h wordList.cpp wordList.h)
#add_executable(NewsFeedSystem test.cpp)