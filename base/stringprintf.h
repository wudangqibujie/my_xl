//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_STRINGPRINTF_H
#define MY_XL_STRINGPRINTF_H
#include <string>



std::string StringPrintf(const char* format, ...);
void SStringPrintf(std::string* dst, const char* format, ...);
void StringAppendF(std::string* dst, const char* format, ...);

#endif //MY_XL_STRINGPRINTF_H
