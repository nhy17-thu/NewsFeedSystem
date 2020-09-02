#include "HashTable.h"

void __hashTable_node::push(const std::string &strIn) {
    val.push_back(CharString(strIn));
}

bool __hashTable_node::has(const CharString &target) {
    if (val.empty())
        return false;
    auto iter = &val.front();
    while (iter) {
        if (iter->content == target)
            return true;
        else
            iter = iter->next;
    }
    return false;
}

int hashIndex(const std::string &GBword) {
    auto temp = (((unsigned char)GBword.at(0) - 176) * 94 + (unsigned char)GBword.at(1) - 161) % hashSize;
    if (temp < 0)
        temp = -temp;
    return temp;
}

bool hashTable::contained(const std::string &strIn) {
    if (!num_elements)
        return false;
    return buckets[hashIndex(strIn)].has(CharString(strIn));
}

void hashTable::insert(const std::string &strIn) {
    ++num_elements;
    buckets[hashIndex(strIn)].push(strIn);
}

unsigned int hashTable::size() {
    return num_elements;
}

