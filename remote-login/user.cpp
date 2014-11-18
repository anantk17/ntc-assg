#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include <openssl/sha.h>

#define filename "users"

using namespace std;

struct User {
	string username, password;
};

namespace custom
{
vector<User> get_users_from_file() {	//making the vector for username, password
	vector<User> e;
	
	ifstream file;
	file.open(filename);
	
	string user, pass;
	
	while(!file.eof()) {
		User u;
		file>>user>>pass;
		u.username = user;
		u.password = pass;
		e.push_back(u);
	}
	
	file.close();
	
	return e;
}

void add_user(string username, string password, vector<User> *e) {	//adding username, password to vector and file
	ofstream file;
	file.open(filename, ios::app);
	file<<username<<endl;
	file<<password<<endl;
	file.close();
	
	User u;
	u.username = username;
	u.password = password;
	(*e).push_back(u);
}

bool exists(User u, vector<User> e) {	//checking if it is valid user
	for(int i=0; i<(int)e.size(); ++i)
		if(u.username==e[i].username && u.password==e[i].password)
			return true;
	return false;
}

User parseauthstring(string auth_string)
    {
        User u;
        size_t pos;
        string delimiter = "$";
        pos = auth_string.find(delimiter);
        if(pos != string::npos)
        {
            auth_string.erase(0,pos+delimiter.length());
            pos = auth_string.find(delimiter);
            if(pos != string::npos)
            {
                u.username = auth_string.substr(0,pos);
                auth_string.erase(0, pos+delimiter.length());
                u.password = auth_string;
            }
            else
            {
                u.username = "";
                u.password = "";
            }
        }
        else
        {
            u.username = "";
            u.password = "";
        }

        return u;
    }
    
}

string sha1(string password) {
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
    return mystr;
}

