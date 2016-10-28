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
#include "fun_server.h"

// orismos statherwn opws autes dinontai apo thn 
// ekfwnhsh ths askhshs
#define UNIXSTR_PATH "unix.str"
#define T_MARGARITA 10
#define T_PEPERONI 12
#define T_SPECIAL 15
#define N_PSISTES 10
#define N_DIANOMEIS 10
#define T_MAKRIA 10
#define T_KONTA 5
#define T_VERYLONG 50
#define LISTENQ 20

char *data;
int shm_id;
int ticks;
char *ptr_or;
void clean_memory();


// aparaithth sunarthsh gia ton upologismo tou xronou 
// ekteleshs mias paraggelias afou mpei sthn lista twn paraggeliwn
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}

// aparaithth sunarthsh gia ton upologismo tou xronou 
// ekteleshs mias paraggelias afou mpei sthn lista twn paraggeliwn
void timeval_print(struct timeval *tv)
{
    char buffer[30];
    time_t curtime;
    curtime = tv->tv_sec;
    strftime(buffer, 30, "%m-%d-%Y  %T", localtime(&curtime));
}

void tick(int sig) {
	int i,late=0,total=0;
	ticks++;
	memcpy(&order,ptr_or,sizeof(order));
	for (i=0;i<MAX_ORDER_LIST_SIZE;i++)
		if (order[i].sender != 0)			
		{
			
			total++;
			if (order[i].order_status==3)
				late++;
		}
	if (ticks%100 == 0)
		printf("\nTotal orders:%d\nLate orders:%d\n", total,late);
}

int main(){

    //dhlwsh metavlhtvn & arxikopoihsh      
    int error; 
    key_t  shmkey;
    int listenfd, connfd,i,pos_psisth,pos_dianomea; 
    char line;
    pid_t childpid;
    socklen_t clilen;
    int temp,k;
	int psistes[10];
	int dianomeis[10];
	char *ptr_ps,*ptr_li,*ptr_di;
	int  pos=0;
    struct sockaddr_un cliaddr, servaddr; /* Structs for the client and server socket addresses. */
	
	/*signal timer variables*/
	struct itimerval tbuf;               /* interval timer structure */
    struct sigaction action;             /* signal action structure */

	signal( SIGCHLD, sig_chld ); /* Avoid "zombie" process generation. */ 
	listenfd = socket( AF_LOCAL, SOCK_STREAM, 0 ); /* Create the server's endpoint */
	unlink( UNIXSTR_PATH ); /* Remove any previous socket with the same filename. */
	bzero( &servaddr, sizeof( servaddr ) ); /* Zero all fields of servaddr. */
	servaddr.sun_family = AF_LOCAL; /* Socket type is local (Unix Domain). */
	strcpy( servaddr.sun_path, UNIXSTR_PATH ); /* Define the name of this socket. */
	/* Create the file for the socket and register it as a socket. */
	bind( listenfd, ( struct sockaddr* ) &servaddr, sizeof( servaddr ) ); 
   
	

	
	// dhmiourgia koinhs mnhmhs 
	data = shared_memory();
	//pointer pou deixnei sthn prwth thesh ths koinhs mnhmhs
	char *ptr = data;
	
	// arxikopoihsh pinakwn psistwn $ dianomewn
	// dhlwnontai oles oi theseis me 0 dhladh oti oloi
    // oi psistes kai oi dianomeis einai eleutheroi 
	for (i=1;i<10;i++)
	{
		psistes[i]=0;
		dianomeis[i]=0;
	}
	
	// topothetountai ta dedomena tou pinaka psistes
    // dedomena sthn koinh mnhmh 
	memcpy(ptr,&psistes,sizeof(int)*10);
	ptr_ps = ptr;
    // o pointer twra deixnei toses theseis meta apo 
    // thn arxikh oso kai o pinakas twn psistwn
	ptr = ptr+sizeof(int)*10;
	
	// topothetountai ta dedomena tou pinaka dianomeis
    // dedomena sthn koinh mnhmh 
	memcpy(ptr,&dianomeis,sizeof(int)*10);
	ptr_di = ptr;
	
	ptr = ptr+sizeof(int)*10;
	
	// topotheteitai  to megethos ths  listas twn paraggeliwn
    // dedomena sthn koinh mnhmh 
	memcpy(ptr,&pos,sizeof(int));
	ptr_li = ptr;
	ptr = ptr+sizeof(int);

	memcpy(ptr,&order,sizeof(order));
	
    // twra o pointer tha deixnei sthn thesh opou 
	// tha apothikeetai h lista me tis paraggelies
	ptr_or = ptr;

	
	// dhmiourgia shmaforwn
	semaphores();
	
    // dhmiourgia mias diergasias paidi to opoio tha analavei na 
    // paragei to signal gia thn xronometrhsh 50 ms sta opoia tha 
    //elegxontai oi trexouses paraggelies
    childpid = fork();
    if (childpid ==0){

    	signal( SIGCHLD, sig_chld ); 
    	/*Set up the SIGALRM handler */
        action.sa_handler = tick;     /* set tick to be the handler function */
        //sigemptyset(&action.sa_mask); /* clear out masked functions */
        action.sa_flags   = 0;        /* no special handling */
    	
        /*Use the sigaction function to associate the signal action with SIGALRM */
        if (sigaction(SIGALRM, &action, NULL) < 0 ) {
            perror("SIGALRM");
            exit(-1);
        }
        // Define a 50 milliseconds timer
        tbuf.it_interval.tv_sec  = 0;
        tbuf.it_interval.tv_usec = 50000;
        tbuf.it_value.tv_sec  = 0;
        tbuf.it_value.tv_usec = 50000;

           /*Use the setitimer function to start the timer*/
        if ( setitimer(ITIMER_REAL, &tbuf, NULL) == -1 ) 
    	{
            perror("setitimer");
            /* should only fail for serious reasons */
            exit(-1);                   
    	}
    	while(1);
    }

	//signal gia exodo kai katharisma mnhmhs me ctrl-c
    signal(SIGINT,clean_memory);
    printf("Gia termatismo tou server \nkai katharismo mnhmhs : ctrl -c\n"); 

    /* Create request queue. */
	listen( listenfd, LISTENQ ); 
    for(;;){
           
    	clilen = sizeof( cliaddr );
        //o server perimenei na dextei kapoia aithsh 
    	connfd = accept( listenfd, ( struct sockaddr * ) &cliaddr, &clilen );
    	
    	//elegxos sundeshs
    	if(connfd==-1)
    	{
    		printf("server accept failure\n");
    		exit(0);
    	}
    
        // dhmioyrgia paidiou to opoio tha analvei 
        // na diekpairewsh thn aithsh 
    	childpid = fork();
    	if (childpid ==0){   
           
    			data = shmat(shm_id, NULL, 0);
    			if (data == (char *)-1) {
    				printf("Could not attach to shared memory!\n");
    				exit(1);
    			}
    			
    			close( listenfd );/* Close listening socket. */
            		     
    			char str[8];
    			
                // upodoxh paraggelias 
    			get_order(connfd,str);
    			
    			// kleidwnei o shmaforos tou megethous ths listas
    			sem_wait(sem_li);
    			
                // antigrafh timhs tou megethous lista topika
    			memcpy(&pos,ptr_li,sizeof(int));
    			
    			// kleidwnei o shmaforos ths listas paraggeliwn
    			sem_wait(sem_or);
    			
    			// antigrafh lista paraggeliwn apo thn koinh 
   			    // sthn topikh mnhmh
    			memcpy(&order,ptr_or,sizeof(order));
    			
    			order[pos].sender = pos+1;
    			order[pos].num_special = atoi(&str[0]);
    			order[pos].num_peperoni = atoi(&str[2]);
    			order[pos].num_margarita = atoi(&str[4]);
    			order[pos].distance = atoi(&str[6]);
    		    				
    			// apothikeush sthn koinh mnhmh ths listas paraggeliwn
    			memcpy(ptr_or,&order,sizeof(order));
    
                // xekleidwnei o shmaforos ths listas paraggeliwn
    			sem_post(sem_or);
    			
    			pos++;	
    			// apothikeush sthn koinh mnhmh tou megethous ths listas
                memcpy(ptr_li,&pos,sizeof(int));
                // xekleidwnei o shmaforos tou megethous ths listas			
    			sem_post(sem_li);
    			
    			
    			int flag=0,cooking_time=0,delivery_time=0,i;
    			
    			// upologismos sunolikou xronou ekteleshs mias paraggelias
    			cooking_time = order[pos].num_special*T_SPECIAL+order[pos].num_peperoni*T_PEPERONI+order[pos].num_margarita*T_MARGARITA;
    			
    			//timer start
    			struct timeval tvBegin, tvEnd, tvDiff;
    			int delay;
    			// begin
    			gettimeofday(&tvBegin, NULL);
    			timeval_print(&tvBegin);
    			
                // shmeio elegxou an uparxei diathesimos psisths 
                //pou tha analavei thn paraggelia
    			while(flag==0)
    			{
   	               // kleidwnei o shmaforos tou pinaka twn psistwn 
    				sem_wait(sem_ps);
  					// antigrafh topika tou pinaka me tous psistes
    				memcpy(&psistes,ptr_ps,sizeof(int)*10);
    				for (i=0;i<N_PSISTES;i++)
    					if (psistes[i]==0)
    					{
    						psistes[i]=1;
    						pos_psisth = i;
    						flag=1;
    						// desmeush tou psisth oti einai apasxolhmenos
    						memcpy(ptr_ps,&psistes,sizeof(int)*10);
    						break;
    					}	
    				// xekleidwnei o shmaforos gia tous psistes			
    				sem_post(sem_ps);
    			}
    			
    			// kathisterhsh gia thn oloklhrwsh ths paraggelias
    			// apo thn meria tou psisth
    			usleep(cooking_time);
    			
    			// apeleutherwsh tou psisth wste na xrhsimopoihthei apo eponenes paraggelies
    			// kleidwnei o shmaforos tou pinaka twn psistwn 
    			sem_wait(sem_ps);
    			// antigrafh topika tou pinaka me tous psistes
    			memcpy(&psistes,ptr_ps,sizeof(int)*10);
    			psistes[pos_psisth]=0;
    			memcpy(ptr_ps,&psistes,sizeof(int)*10);
    			// xekleidwnei o shmaforos gia tous psistes	
    			sem_post(sem_ps);
    			
    			// kleidwnei o shmaforos ths listas paraggeliwn
    			sem_wait(sem_or);
    			//copy from shared to local
    			memcpy(&order,ptr_or,sizeof(order));
                // molis etoimasthke paraggelia			
    			order[pos].order_status = 1;	
    			//copy from local to shared
    			memcpy(ptr_or,&order,sizeof(order));
    			// xekleidwnei o shmaforos ths listas paraggeliwn
    			sem_post(sem_or);
    			
    			flag = 0;
    			// kathorimsmos xronou kathusthrhshs tou dianomea
                // analoga an o pelaths einai konta h makria 
    			if (order[pos].distance==0)
    				delivery_time = T_KONTA;
    			else
    				delivery_time = T_MAKRIA;
    				
    		    // shmeio elegxou an uparxei diathesimos dianomeas
                //pou tha analavei thn paraggelia	
    			while(flag==0)
    			{
                    // kleidwnei o shmaforos tou pinaka twn dianomewn 
    				sem_wait(sem_di);
    				// antigrafh topika tou pinaka me tous psistes
                    memcpy(&dianomeis,ptr_ps,sizeof(int)*10);
    				for (i=0;i<N_DIANOMEIS;i++)
    					if (dianomeis[i]==0)
    					{
    						dianomeis[i]=1;
    						pos_dianomea = i;
    						flag=1;
    						// desmeush tou dianomea oti einai apasxolhmenos
    						memcpy(ptr_di,&dianomeis,sizeof(int)*10);
    						break;
    					}	
    				// xekleidwnei o shmaforos gia tous dianomeis
    				sem_post(sem_di);
    			}
    			// kathisterhsh gia thn oloklhrwsh ths paraggelias
    			// apo thn meria tou dianomea
    			usleep(delivery_time);
    			
    			// apeleutherwsh tou dianomea wste na 
                // xrhsimopoihthei apo eponenes paraggelies
                 // kleidwnei o shmaforos tou pinaka twn dianomewn 
    			sem_wait(sem_di);
    			memcpy(&dianomeis,ptr_ps,sizeof(int)*10);
    			dianomeis[pos_dianomea]=0;
    			memcpy(ptr_di,&dianomeis,sizeof(int)*10);
    			// xekleidwnei o shmaforos gia tous dianomeis
    			sem_post(sem_di);
    			
    			//timer stop
    			gettimeofday(&tvEnd, NULL);
    			timeval_print(&tvEnd);
    			timeval_subtract(&tvDiff, &tvEnd, &tvBegin);
    			delay = tvDiff.tv_usec/100;
    			printf("%d milliseconds\n",delay);
    			
    			// shmeio elegxou an exoume kathusterhsh megaluterh tou 
    			// epitreptou oriou twn 50 ms
    			
                //periptvsh opou den exoume kathusrerhsh
    			if (delay<T_VERYLONG)
    			{   
                    // apostolh paraggelias mesa sta oria
    				send_order(connfd,1);
    				// kleidwnei o shmaforos ths listas paraggeliwn
    				sem_wait(sem_or);
    				//copy from shared to local
    				memcpy(&order,ptr_or,sizeof(order));
                    // h paraggelia exei stalei sthn wra ths			
    				order[pos].order_status = 2;	
    				//copy from local to shared
    				memcpy(ptr_or,&order,sizeof(order));
    				// xekleidwnei o shmaforos ths listas paraggeliwn
    				sem_post(sem_or);
    				}
    			else
    			{
                    // apostolh paraggelias ektos oriwn
    				send_order(connfd,2);
    				// kleidwnei o shmaforos ths listas paraggeliwn
    				sem_wait(sem_or);
    				//copy from shared to local
    				memcpy(&order,ptr_or,sizeof(order));
                    // h paraggelia exei stalei me kathusterhsh		
    				order[pos].order_status = 3;	
    				//copy from local to shared
    				memcpy(ptr_or,&order,sizeof(order));
    				// xekleidwnei o shmaforos ths listas paraggeliwn
    				sem_post(sem_or);
    			}
    
    			close_semaphores();
    			
    			error = shmdt(data);
    			if (error == -1) {
    				printf("Could not detach from shared memory!\n");
    				exit(1);
    			}
    			return 0;

    	}//end if
              
            close(connfd);
                   
      } //end for
    	

/******************************************************************************************************/	

}

// sunarthsh gia ton apodesmush twn shmaforwn kai 
// ton katharismo ths koinhs mnhmhs
void clean_memory()
 {
    int error;
    close_semaphores();
    unlink_semaphores();
    error = shmdt(data);

    if(error == -1) {
        printf("Could not detach from shared memory!\n");
        exit(1);
        }

    shmctl(shm_id, IPC_RMID, NULL);   
    printf("\nkatharismos mnhmhs : OK\n");
    printf("termatismos : OK\n");
    exit(0);
}


