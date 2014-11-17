#include <sys/socket.h>
#include <sys/un.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>

//Includes wrapper functions to use c++ style strings with sockets
#include "utility.cpp"

//char *socket_path = "./socket";
char *socket_path = "\0hidden";

using namespace std;

int main(int argc, char *argv[]) {

  string username;
  
  //Take username and password from STDIN
  //Password is taken using getpass to prevent password from being displayed on screen

  cout << "Enter username: "<<endl;
  cin >> username;
  cout << endl;
  int c = getchar();
  string password = custom::getpass("Enter password: ");
  cout << endl;
  struct sockaddr_un addr;
  int fd,rc;

  if (argc > 1) socket_path=argv[1];

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
  
  //Connect to server
  
  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    printf("Error");
    exit(-1);
  }

  //Create authentication string
  //Format: auth$username$password
  //username and password should not contain $

    //string auth_string = custom::getauthstring(username,password);
    
    int logged_in = custom::authenticate(fd, username, password); 
   
    if(logged_in)
    {
        cout << "Connected to ECHO Server. Enter exit to exit" << endl;

        string readbuffer;
        string writebuffer;
        while(1)
            {
                cout << "Enter text: " << endl;
                getline(cin,readbuffer);
                int rc;
                rc = custom::cppwrite(fd, readbuffer);
                if(readbuffer == "exit")
                {
                    int ret = shutdown(fd, 0);
                    return 0;
                }
                cout << "Server Returned: " << endl;
                rc = custom::cppread(fd, writebuffer);
                cout << writebuffer << endl;
                readbuffer.clear();
                writebuffer.clear();
            }

    }
    else
    {   
        cout << "Incorrect credentials" << endl;
        shutdown(fd, 0);
                
    }
  return 0;
}

