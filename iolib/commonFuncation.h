//Author:Winter Chen
//Created:2013-4-25
//funcations I used
//

#ifndef COMMONFUNCATION_H
#define COMMONFUNCATION_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;
extern bool gIsBigEndian;

class GenerateString{	
public:		
	GenerateString(){}		
	~GenerateString(){}		
	template<class T>		
	GenerateString& operator()(const T &t){			
		mString << t;			
		return *this;		
	}		
	/*		
	GenerateString& operator()(const char *t){
		mString << t;			return *this;		
	}
	*/
	string GetString(){
		return mString.str();
	}
	void Clear(){
		mString.str("");
	}
public:		
	stringstream mString;
};



unsigned IpStringToU32(const char* pAddr);

unsigned IpStringToU32(const string& strAddr);
string IpU32ToString(unsigned ipv4);
bool IsBigEndian();
unsigned GetIPC(unsigned ip);
unsigned GetIPC(string ip);
string GetIPCString(string ip);
template <class T>  
T StringToNum(string& str){  
    istringstream ss(str);  
    T num;  
    ss >> num;  
    return num;      
}

template <class T>  
int ParseStringTolist(string &ips, vector<T> &xlist, char saparator = ','){
	int pos;
	int startPos = 0;
	unsigned ipU32 = 0;
	string subString = "";
	//cout << "ips :" << ips << endl;
	do    
    {    
        pos = ips.find(saparator, startPos);   // 
        if(pos != string::npos)  // 
        {           
            subString = ips.substr(startPos,pos-startPos);
			//cout << subString << " ";
			istringstream ss(subString);
			T tmp;
			ss >> tmp;
			//cout << tmp << endl;
			xlist.push_back(tmp);
            startPos = pos+1;                
        }    
        else    
        {                       
            subString = ips.substr(startPos,pos-startPos);
			istringstream ss(subString);
			
			T tmp;
			ss >> tmp;
			xlist.push_back(tmp);
			//cout << subString << endl;
            break;    
        }    
    }while(1);
	return 0;
}









#endif


