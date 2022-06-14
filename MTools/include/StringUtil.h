//
// Created by NotAdmin on 2021/9/8.
//
#include <string>

#ifndef WZ_STEING_UTIL
#define WZ_STEING_UTIL

using namespace std;

class StringUtil {
public:
    static string &replace_all(string &str, const string &old_value, const string &new_value);
};

#endif