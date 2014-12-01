#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <netinet/in.h>
#include "polarssl/ctr_drbg.h"
#include "polarssl/dhm.h"

//Includes wrapper functions to use c++ style strings with sockets
#include "utility.cpp"

using namespace std;

int main(int argc, char *argv[]) {

  unsigned char *p, *end;
  const char  *pers = "dh_client";
  entropy_context entropy;
  ctr_drbg_context ctr_drbg;
  dhm_context dhm;
  dhm_init( &dhm );
  
  
  //Setup the Random Number Generator
  entropy_init( &entropy );
  int t = ctr_drbg_init( &ctr_drbg, entropy_func, &entropy,(const unsigned char *) pers,strlen( pers ) );
  
  string username;
  
  
  //Take username and password from STDIN
  //Password is taken using getpass to prevent password from being displayed on screen

  cout << "Enter username: "<<endl;
  cin >> username;
  cout << endl;
  int c = getchar();
  string password = custom::getpass("Enter password: ");
  cout << endl;
  
  struct sockaddr_in serv_addr;
  struct hostent *server;

  int fd,rc;

  int portno = atoi(argv[2]);

  if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }
  
  server = gethostbyname(argv[1]);
  if(server == NULL) {
        perror("No such host");
        exit(1);
    }
    
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char*)server->h_addr,(char*)&serv_addr.sin_addr.s_addr,server->h_length);
  serv_addr.sin_port = htons(portno);
  
  //Connect to server
  
  if (connect(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
    perror("connect error");
    printf("Error");
    exit(-1);
  }
  unsigned char buf[2048];
  memset( buf,0,sizeof(buf));
  t = read(fd, buf,2);
  size_t n = (buf[0] << 8) | buf[1] ;
  size_t buflen = n;
  
  //Read server's public key and store into buf
  memset( buf, 0, sizeof(buf));
  t = read(fd, buf,n);
    
  p = buf;
  end = buf + buflen;
  t = dhm_read_params( &dhm, &p ,end);
  
  //Create client's public key  
  n = dhm.len;
  t = dhm_make_public( &dhm, (int) dhm.len, buf, n, ctr_drbg_random, &ctr_drbg);
  
  //Send client's public key to the server
  t = write(fd, buf, n);
  
  //Calculate shared secret key
  t  = dhm_calc_secret( &dhm, buf, &n, ctr_drbg_random, &ctr_drbg);
  
  cout << "Shared secret" << endl;
  for(n = 0;n < 16; n++)
    printf("%02x", buf[n]);
  
  cout << endl;
    
  
  //Create authentication string
  //Format: auth$username$password
  //username and password should not contain $
    
    int logged_in = custom::authenticate(fd, username,password, buf); 
   
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
                rc = custom::cpp_enc_write(fd, readbuffer, buf);
                if(readbuffer == "exit")
                {
                    int ret = shutdown(fd, 0);
                    return 0;
                }
                cout << "Server Returned: " << endl;
                rc = custom::cpp_dec_read(fd, writebuffer, buf);
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
    dhm_free( &dhm );
    ctr_drbg_free( &ctr_drbg );
    entropy_free( &entropy );
  return 0;
}

