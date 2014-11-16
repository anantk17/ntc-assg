#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//char *socket_path = "./socket";
char *socket_path = "\0hidden";

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  char buf[100];
  int fd,rc;

  if (argc > 1) socket_path=argv[1];

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    printf("Error");
    exit(-1);
  }

    char auth[] = "auth$";
    char user[100];
    //strncpy(user,"anant",sizeof("anant")-1);
    strcpy(user,"anant");
    char pass[100];
    strcpy(pass,"pass");
    //strncpy(user,"pass",sizeof("anant")-1);
    
    char auth_msg[256];
    strncpy(auth_msg,auth,sizeof(auth)-1);
    strcat(user,"$");
    //printf("%s",user);
    strcat(auth_msg,user);
    strcat(auth_msg,pass);
    //printf("%s",auth_msg);
    //rc = write(fd,user,sizeof(user) - 1);
    rc = write(fd,auth_msg,sizeof(auth_msg) - 1); 
 // }i
    int logged_in = 0;
    while(rc=read(fd,buf,1) > 0){
        //printf("%c",buf[0]);
        //sscanf(buf,"%s%d",auth_status,&status); 
        //printf("%d",status);
        if(buf[0] == '1'){
            printf("Login Successful\n");
            fflush(stdout);
            logged_in = 1;
            bzero(buf,sizeof(buf));
            break;
        }
        else{
            printf("Login Failed");
            exit(-1);
        }
    }

  printf("Connected to ECHO Server.\n ");
  fflush(stdout);
  if(logged_in){
  /*while( (rc=read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
    if (write(fd, buf, rc) != rc) {
      if (rc > 0) fprintf(stderr,"partial write");
      else {
        perror("write error");
        exit(-1);
      }
    }
    rc = read(fd,buf,sizeof(buf));
    printf("Server replied : %s",buf);
    printf("Write Message: ");
  }*/
    while(1)
    { 
        printf("Enter Message: ");
        fflush(stdout);
        rc = read(STDIN_FILENO,buf,sizeof(buf));
        printf("Server returned : %s", buf);
        write(fd,buf,rc);
        printf("\n");

    }
  }
  return 0;
}

