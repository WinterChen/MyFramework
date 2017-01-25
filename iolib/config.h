#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <iostream>
#include <string>
#include <map>
using namespace std;


class Config{
public:
	Config(){}
	~Config(){}
	void InitConfig(string fileName);
	string GetValue(std::string key, std::string def) ;
    const char* GetValue(std::string key, const char* def) ;
    int GetValue(std::string key, int def) ;
	short GetValue(std::string key, short def) ;
    unsigned int GetValue(std::string key, unsigned int def) ;
    long GetValue(std::string key, long def) ;
    unsigned long GetValue(std::string key, unsigned long def) ;
    long long GetValue(std::string key, long long def) ;
    unsigned long long GetValue(std::string key, unsigned long long def) ;
    double GetValue(std::string key, double def) ;
	
	
private:
	map<string, string> mConfigMap;
};


#endif

