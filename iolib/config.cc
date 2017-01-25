#include <sstream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include "config.h"

/*
配置文件以行为单位
每一行只能包括两项:配置名和值
配置名和配置值中不能包含空格字符，且两者之间必须用空格间隔。例如

serverip 127.0.0.1
serverport 14000
tasklen 1000
*/
template <class T>  
T Str2Num(const string& str)  
{  
	istringstream iss(str);  
	T num;  
	iss >> num;  
	return num; 	 
}  

string Num2Str(int i)
{
        stringstream ss;
        ss<<i;
        return ss.str();
}


void Config::InitConfig(string fileName){
	ifstream configFile;
    configFile.open(fileName.c_str());
    string line;
    string key;
	string value;
    while(getline(configFile, line)){
        istringstream ss(line);
		ss >> key;
		ss >> value;
		mConfigMap[key] = value;
	}
}


string Config::GetValue(string key, string def) 
{
    map<string, string>::iterator  i = mConfigMap.find(key);
    return (i == mConfigMap.end() ? def : i->second);
}
const char* Config::GetValue(string key, const char* def) 
{
    map<string , string>::iterator  i = mConfigMap.find(key);
    return (i == mConfigMap.end() ? def : i->second.c_str());
}

short Config::GetValue(string key, short def){
	map<string , string>::iterator  i = mConfigMap.find(key);
	return (i == mConfigMap.end() ? def : Str2Num<short>(i->second.c_str()));
	
}
int Config::GetValue(string key, int def) 
{
    map<string , string>::iterator const i = mConfigMap.find(key);
    return (i == mConfigMap.end() ? def : atoi(i->second.c_str()));
}

unsigned int Config::GetValue(string key, unsigned int def) 
{
    map<string , string>::iterator i = mConfigMap.find(key);
    return (i == mConfigMap.end() ? def : atoi(i->second.c_str()));
}

long Config::GetValue(string key, long def) 
{
    map<string , string>::iterator i = mConfigMap.find(key);
    return (i == mConfigMap.end() ? def : atol(i->second.c_str()));
}

unsigned long Config::GetValue(string key, unsigned long def) 
{
    map<string , string>::iterator i = mConfigMap.find(key);
    return (i == mConfigMap.end() ? def : atol(i->second.c_str()));
}

long long Config::GetValue(string key, long long def) 
{
    map<string , string>::iterator i = mConfigMap.find(key);
    return (i == mConfigMap.end() ? def : atoll(i->second.c_str()));
}

unsigned long long Config::GetValue(string key, unsigned long long def) 
{
    map<string , string>::iterator i = mConfigMap.find(key);
    return (i == mConfigMap.end() ? def : atoll(i->second.c_str()));
}

double Config::GetValue(string key, double def) 
{
    map<string , string>::iterator i = mConfigMap.find(key);
    return (i == mConfigMap.end() ? def : atof(i->second.c_str()));
}

