#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//char *socket_path = "./socket";
char *socket_path = "\0hidden";

int main(int argc, char *argv[]) {
  char username[100];
  char* password;
  
  //Take username and password from STDIN
  //Password is taken using getpass to prevent password from being displayed on screen

  printf("Enter username: ");
  scanf("%s",username);
  printf("%s\n",username);
  password = getpass("Enter password: ");
  printf("\n");
  printf("%s", password);
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
  
  //Connect to server
  
  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    printf("Error");
    exit(-1);
  }
  
  //Create authentication string
  //Format: auth$username$password
  //username and password should not contain $

    char auth[] = "auth$";
//    char user[100];
//    strcpy(user,"anant");
//    char pass[100];
//    strcpy(pass,"pass");
    
    char auth_msg[256];
//    strncpy(auth_msg,auth,sizeof(auth)-1);
//    strcat(username,"$");//
//    printf("%s",username);
//    strcat(auth_msg,username);
//    printf("%s- auth_msg", auth_msg);
//    strcat(auth_msg,password);
    sprintf(auth_msg, "auth$%s$%s", username, password);   
    printf("auth-%s", auth_msg);

    //Write the authentication string on socket
    //Wait for 1 byte return value

    rc = write(fd,auth_msg,sizeof(auth_msg) - 1); 
    int logged_in = 0;
    while(rc=read(fd,buf,1) > 0){
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
 
  if(logged_in){
    printf("Connected to ECHO Server.Enter exit to exit\n ");
    fflush(stdout);
    
    //Read input from user
    //Write input to socket
    //Read returned data from socket
    //Print returned data on screen

    while(1)
        {
        char temp_buf[10];
        printf("Enter Message: ");
        fflush(stdout);
        rc = read(STDIN_FILENO,buf,sizeof(buf));
        sscanf(buf,"%s",temp_buf);
        write(fd,buf,rc);
        //If entered text is exit, close connection
        if(!strcmp(temp_buf,"exit"))
        {
            
            int ret = shutdown(fd,0);
            return 0;
        }
        rc = read(fd, buf,sizeof(buf));
        printf("Server returned : %s", buf);
        bzero(buf,sizeof(buf));

        printf("\n");

    }
  }
  return 0;
}

