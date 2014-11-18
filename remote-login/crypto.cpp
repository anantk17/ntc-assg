#include <iostream>
#include <sstream>
#include <string>
#include "polarssl/sha1.h"

using namespace std;
namespace custom{
string cppsha1(string text) {
	unsigned char obuf[20];
	sha1((unsigned char*)text.c_str(), text.size(), obuf);
	string s="";
	
	int i;
	
 	for (i = 0; i < 20; i++) {
 		s+=obuf[i];
        }
        
	stringstream ss;
	for(int i=0; i<20; ++i)
	    ss << std::hex << (int)obuf[i];
	string mystr = ss.str();
    return mystr;
    }
}
