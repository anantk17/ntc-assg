#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include "utility.cpp"

char *socket_path = "\0hidden";

using namespace std;

bool exists(User u)
{
    return (u.username== "anant" and u.password=="pass");
}

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  int fd,cl,rc;

  if (argc > 1) socket_path=argv[1];

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

  unlink(socket_path);

  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind error");
    exit(-1);
  }

  if (listen(fd, 5) == -1) {
    perror("listen error");
    exit(-1);
  }

  while (1) {
    if ( (cl = accept(fd, NULL, NULL)) == -1) {
      perror("accept error");
      continue;
    }
    
    string username, password,buffer;
    rc = custom::cppread(cl, buffer);
    User u = custom::parseauthstring(buffer);
    
    if(exists(u))
    {
        buffer.assign("1");
        rc = custom::cppwrite(cl,buffer);
        cout << "User "<< u.username << "logged in" <<endl;
    }
    else
    {
        buffer.assign("0");
        rc = custom::cppwrite(cl,buffer);
        cout << "Failed login attempt" << endl;
        shutdown(cl,2);
        continue;
    }

    buffer.clear();
    
    while(1)
    {
        rc = custom::cppread(cl,buffer);
        if(rc >= 0)
        {
            if(buffer == "exit")
            {
                cout << "User "<<u.username<<" logged out"<<endl;
                shutdown(cl,2);
                break;
            }
            cout << "Read : " << buffer <<  endl;
            rc = custom::cppwrite(cl,buffer);
            if(rc >= 0)
                cout << "Returned : "<< buffer << endl;
        }
        
        buffer.clear();

        if(rc == -1)
        {
            perror("socket i/o error");
            shutdown(cl,2);
        }
    }
  }
  return 0;
}

