#ifndef _FIREBASE_H_
#define _FIREBASE_H_

#include <string>
#include <vector>
#include <map>
void firebase_config(std::string url, std::string doc);
void get_configure(std::map<std::string, std::string>& v);

#endif //_FIREBASE_H_