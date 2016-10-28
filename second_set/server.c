#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>	
#include <sys/socket.h>	
#include <errno.h>		
#include <sys/wait.h>	
#include <sys/un.h>		
#include "structs.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include "fun_server.h"

#define UNIXSTR_PATH "unix.str"
#define LISTENQ 20



int main(){

    //dhlwsh metavlhtvn & arxikopoihsh      
    int error; 
    key_t  shmkey;
    int listenfd, connfd,i; 
    char line;
    pid_t childpid;
    socklen_t clilen;
    int temp,k;

	int  pos=0;
    struct sockaddr_un cliaddr, servaddr; /* Structs for the client and server socket addresses. */
	

	listenfd = socket( AF_LOCAL, SOCK_STREAM, 0 ); /* Create the server's endpoint */
	unlink( UNIXSTR_PATH ); /* Remove any previous socket with the same filename. */
	bzero( &servaddr, sizeof( servaddr ) ); /* Zero all fields of servaddr. */
	servaddr.sun_family = AF_LOCAL; /* Socket type is local (Unix Domain). */
	strcpy( servaddr.sun_path, UNIXSTR_PATH ); /* Define the name of this socket. */
	/* Create the file for the socket and register it as a socket. */
	bind( listenfd, ( struct sockaddr* ) &servaddr, sizeof( servaddr ) ); 
   
	
	// arxikopoihsh pinakwn psistwn $ dianomewn
	// dhlwnontai oles oi theseis me 0 dhladh oti oloi
    // oi psistes kai oi dianomeis einai eleutheroi 
	for (i=1;i<10;i++)
	{
		psistes[i]=0;
		dianomeis[i]=0;
	}
	

    //arxikopoihsi tou thread
  	pthread_t time_thread;
  		
    int noth,qc;
    
    qc=pthread_create(&time_thread, NULL, check_orders,(void*) noth );

	
    

    /* Create request queue. */
	listen( listenfd, LISTENQ ); 
   

///////////////////////////////////////////////////////////////////////////////////////////////


   for(;;){

                //ofeiloume na prosexoume oti den xrhsimopoioume se ayth thn periptwsh 
                //mesa sto for loop afou ton rolo ton child processes ton paizoun ta 
		//threads .
		//me alla logia gia kathe accept dhmiourgoume ena thread to opoi tha exuphrethsei
		//ton client      
       	        clilen = sizeof( cliaddr );
                //arxikopoihsi tou thread
  	    	pthread_t threads;
        	int rc;
               //apodoxh aithshs apo client
      	       connfd = accept( listenfd, ( struct sockaddr * ) &cliaddr, &clilen );
            
             if(connfd==-1)
                {
                   printf("server accept failure\n");
		   exit(0);
	         }

     	      //dhmiourgia threads kai ektelesh routinas  
               rc=pthread_create(&threads,NULL,create_threads,(void*)connfd);
	   
	     //telos den xrhsimopoioume signals oute gia thn apofygh zombies 
	     //alla oute kai gia katharismo ths shared memory afou me ctrl -c 
             //se ena thread termatizontai ola kai apodesmeuetai otidhpote
	     //exoume sthn mnhmh           
              
    } //end for
        
} //end main	




    	

/******************************************************************************************************/	



