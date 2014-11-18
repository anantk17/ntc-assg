#include<iostream>
#include<vector>
#include<string>
#include<fstream>

#define filename "users"

using namespace std;

struct entry {
	string username, password;
};

vector<entry> make() {	//making the vector for username, password
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

void add_user(string username, string password, vector<entry> *e) {	//adding username, password to vector and file
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

bool compare(entry en, vector<entry> e) {	//checking if it is valid user
	for(int i=0; i<(int)e.size(); ++i)
		if(en.username==e[i].username && en.password==e[i].password)
			return TRUE;
	return FALSE;
}
