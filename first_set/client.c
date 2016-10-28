#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include "structs.h"
#include <errno.h>		
#include <sys/wait.h>	
#include <sys/ipc.h>
#include <stdlib.h>
#include "fun_client.h"

#define UNIXSTR_PATH "unix.str"


int main(int argc, char *argv[])
{
   
      
    //dhlwsh metavlhtvn & arxikopoihsh
    int sockfd,i;   
    struct sockaddr_un servaddr; /* Struct for the server socket address. */
    int pid,result;
    int topic,epil;
    char order[8];
   
   // periptwsh pou trexoume to _run_tests.sh
   	if (argc==5)
	{
		order[0]=argv[1];
		order[1]=",";
		order[2]=argv[2];
		order[3]=",";
		order[5]=argv[3];
		order[6]=",";
		order[7]=argv[4];

	}
   
   // periptwsh pou ekteloume enan client
   // na pernei eisodo apo to plhktologio
	if (argc==1)
	{
	    // emfanish menu
		menu();
    
		// epilogh kai elegxos paraggelias
		check_order(order);
	}
	if ((argc!=1)&&(argc!=5))
		exit(1);
	
    //create socket     
    sockfd = socket( AF_LOCAL, SOCK_STREAM, 0 ); 
    bzero( &servaddr, sizeof( servaddr ) ); /* Zero all fields of servaddr. */
    servaddr.sun_family = AF_LOCAL; /* Socket type is local (Unix Domain). */
    strcpy( servaddr.sun_path, UNIXSTR_PATH ); /* Define the name of this socket. */

    // syndesh sto server
    result = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  
   // elegxos connect 
	if (result == -1)
	{
	   perror("connect failure\n");
	   exit(0);
	}

    // apostolh paraggelias
    send_order(sockfd,order);
    
    // anamonh gia afiksh paraggelias
    order_arrival(sockfd);
  
    return 0;
}
