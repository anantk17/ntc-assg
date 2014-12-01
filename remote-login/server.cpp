#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <polarssl/dhm.h>
#include <polarssl/entropy.h>
#include <polarssl/ctr_drbg.h>

#include "user.cpp"


char *socket_path = "\0hidden";
//unsigned char key[32] = {54, 207, 140, 177, 126, 224, 252, 171, 112, 224, 200, 60, 78, 13, 191, 219, 84, 13, 121, 195, 161, 97, 151, 243, 207, 35, 194, 227, 65, 74, 70, 35};
using namespace std;

int main(int argc, char *argv[]) {

  struct sockaddr_in serv_addr, cli_addr;

  int fd,cl,rc;
  unsigned int portno;
  
  
  vector<User> user_list = custom::get_users_from_file();
  cout << user_list[0].username<<endl;
  cout << user_list[1].username<<endl;
  
  const char  *pers = "dh_server";
  entropy_context entropy;
  ctr_drbg_context ctr_drbg;
  dhm_context dhm;
  dhm_init( &dhm );
  
  entropy_init( &entropy );
  
  //Read prime number and generator from file
  
  int ret = ctr_drbg_init( &ctr_drbg, entropy_func, &entropy,(const unsigned char *) pers,strlen( pers ) ) ;
        FILE* f;
        size_t n, buflen;
      if( ( f = fopen( "dh_prime.txt", "rb" ) ) == NULL )
    {
        ret = 1;
        printf( " failed\n  ! Could not open dh_prime.txt\n" \
                "  ! Please run dh_genprime first\n\n" );
        return 0;
    }

    if( mpi_read_file( &dhm.P, 16, f ) != 0 ||
        mpi_read_file( &dhm.G, 16, f ) != 0 )
    {
        printf( " failed\n  ! Invalid DH parameter file\n\n" );
        return  0;
    }

    fclose( f );
                                   
  if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = 5001;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  if (bind(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
    perror("bind error");
    exit(-1);
  }

  if (listen(fd, 5) == -1) {
    perror("listen error");
    exit(-1);
  }
  unsigned int clilen = sizeof(cli_addr);
  while (1) {
    if ( ( cl = accept(fd, (struct sockaddr*)&cli_addr, &clilen)) == -1) {
      perror("accept error");
      continue;
    }
    if(fork()== 0){
    close(fd);
    
    unsigned char buf[2048];
    unsigned char buf2[2];
    
    //Create server's public key
    ret = dhm_make_params( &dhm, (int) mpi_size( &dhm.P ), buf, &n,  ctr_drbg_random, &ctr_drbg );
    
    buflen = n + 2;
    buf2[0] = (unsigned char)(buflen >> 8 );
    buf2[1] = (unsigned char)(buflen);
    
    //Send server's public ket
    ret = write( cl, buf2, 2);
    ret = write( cl, buf, buflen);
    
    //Read client's public key
    ret = read ( cl, buf, n);
    ret = dhm_read_public( &dhm, buf, dhm.len );
    
    //Calculate shared secret
    ret = dhm_calc_secret( &dhm, buf, &n,
                                 ctr_drbg_random, &ctr_drbg );
    
    cout << "Shared secret" << endl;
    for( n = 0; n < 16; n++ )
        printf( "%02x", buf[n] );
    
    cout << endl;    
    string username, password,buffer;
    rc = custom::cpp_dec_read(cl, buffer , buf);
    User u = custom::parseauthstring(buffer);
    if(custom::exists(u,user_list))
    {
        buffer.assign("1");
        rc = custom::cpp_enc_write(cl,buffer,buf);
        cout << "User "<< u.username << "logged in" <<endl;
    }
    else
    {
        buffer.assign("0");
        rc = custom::cpp_enc_write(cl,buffer,buf);
        cout << "Failed login attempt" << endl;
        close(cl);
        return 0;
    }

    buffer.clear();
    
    while(1)
    {
        rc = custom::cpp_dec_read(cl,buffer,buf);
        if(rc >= 0)
        {
            if(buffer == "exit")
            {
                cout << "User "<<u.username<<" logged out"<<endl;
                close(cl);
                return 0;
            }
            cout << "Read : " << buffer <<  endl;
            rc = custom::cpp_enc_write(cl,buffer,buf);
            if(rc >= 0)
                cout << "Returned : "<< buffer << endl;
        }
        
        buffer.clear();

        if(rc == -1)
        {
            perror("socket i/o error");
            close(2);
        }
    }
    }
  }
  close(fd);
  return 0;
}
