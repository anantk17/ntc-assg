#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<iomanip>
#include<sstream>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

#define filename "users"
using namespace std;

struct entry {
	string username, password;
};

vector<entry> make() {
	vector<entry> e;
	
	ifstream file;
	file.open(filename);
	
	string user, pass;
	
	while(!file.eof()) {
		entry en;
		file>>user>>pass;
		en.username = user;
		en.password = pass;
		e.push_back(en);
	}
	
	file.close();
	
	return e;
}

void add_user(string username, string password, vector<entry> *e) {	
	ofstream file;
	file.open(filename, ios::app);
	file<<username<<endl;
	file<<password<<endl;
	file.close();
	
	entry en;
	en.username = username;
	en.password = password;
	(*e).push_back(en);
}

int compare(string username, string password, vector<entry> e) {
	for(int i=0; i<(int)e.size(); ++i)
		if(username==e[i].username && password==e[i].password)
			return 1;
	return 0;
}

void sha1(string password) {
	unsigned char obuf[20];
	SHA1((unsigned char*)password.c_str(), password.size(), obuf);
	string s="";
	
	int i;
	
 	for (i = 0; i < 20; i++) {
 		s+=obuf[i];
        }
        
	stringstream ss;
	for(int i=0; i<20; ++i)
	    ss << std::hex << (int)obuf[i];
	string mystr = ss.str();
	cout<<mystr<<endl;
    	printf("\n");
}
