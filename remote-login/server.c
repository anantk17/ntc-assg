#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

char *socket_path = "\0hidden";

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  char buf[1000];
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

    char* username = (char *)malloc(100 * sizeof(char));
    char* password = (char *)malloc(100*sizeof(char));
    char auth[6];
    
    while(rc = read(cl,buf,sizeof(buf)) > 0){
        char auth_status[20];
       //printf("\nbuffer : %s",buf);
        if(rc >= 0){
            sscanf(buf,"%[^$]$%[^$]$%s",auth,username,password);
            //printf("\n%s\t %s\t %s",auth,username,password);
            if(!strcmp(username,"anant") && !strcmp(password,"pass"))
            {
                strcpy(auth_status,"1");
                write(cl,auth_status,1);
                printf("User %s logged in\n",username);
                fflush(stdout);
                break;
            }
            else
            {
                strcpy(auth_status,"0");
                write(cl, auth_status,1);
                printf("User %s failed to login\n",username);
                fflush(stdout);
                exit(-1);
            }
        }
    } 
    bzero(buf,sizeof(buf));
    while ( 1){
      rc=read(cl,buf,sizeof(buf)); {
      printf("Read: %s\n", buf);
      fflush(stdout);
      write(cl,buf,sizeof(buf));
      printf("Returned: %s\n", buf);
      fflush(stdout);
    }
    if (rc == -1) {
      perror("read");
      exit(-1);
    }
    else if (rc == 0) {
      printf("EOF\n");
      close(cl);
    }
    }
  }


  return 0;
}


