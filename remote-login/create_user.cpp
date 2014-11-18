#include <iostream>
#include <fstream>
#include "user.cpp"

using namespace std;

#define file_name "user"

int main()
{
    string username,password;
    cin >> username;
    cin >> password;
    
    ofstream file(file_name, ios::app);
    file << username<<endl; 
    file << custom::sha1(password)<<endl;
    
    return 0;

}
