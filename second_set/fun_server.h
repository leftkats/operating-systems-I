#include <stdio.h>
#include<stdlib.h>
#include <sys/types.h>	
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>


// orismos statherwn opws autes dinontai apo thn 
// ekfwnhsh ths askhshs
#define UNIXSTR_PATH "unix.str"
#define T_MARGARITA 1
#define T_PEPERONI 2
#define T_SPECIAL 3
#define N_PSISTES 10
#define N_DIANOMEIS 10
#define T_MAKRIA 5
#define T_KONTA 3
#define T_VERYLONG 20


//synarthseis pou kalountai apo ta threads
void *create_threads(void*);
void* check_orders (void* );

//arxikopoihsh twn mutexes
pthread_mutex_t mymutex_ps = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mymutex_di = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mymutex_li = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mymutex_or = PTHREAD_MUTEX_INITIALIZER;


int thr_count=0;

int psistes[10];
int dianomeis[10];
int  pos=0;

 //timer start
 struct timeval tvBegin[100], tvEnd[100], tvDiff;
 
void get_order(int ,char *);
void send_order(int ,int);


// afiksh paraggelias apo ton client
void get_order(int fd,char * order)
{
  char str[8];
 //prwto diavasma socket gia epilogh paraggelias
  read( fd, str, sizeof(str) );
  strcpy(order,str);
  return ;
}

//apostolh ston client ths paraggelias
void send_order(int fd,int temp)
{
     //deyterh eggrafh sto socket gia apostolh paraggelias  
       write( fd, &temp, sizeof( int ) );
}

// routina dhmiourgias kai exuphrethshs threads 
void *create_threads(void* fd)
{
                                     
     int epil,k,temp,pos_psisth,pos_dianomea,my_pos;
     int flag=0,cooking_time=0,delivery_time=0,i;
  
  //kanoume cast sto fd dioti einai kenos deikths
    int connfd=(int)fd;
         
    char str[8];
    			
    // upodoxh paraggelias 
    get_order(connfd,str); 
  
    //kleidoma tou mutex gia thn paraggelia
    pthread_mutex_lock(&mymutex_or);		
    order[pos].sender = pos+1;
    order[pos].num_special = atoi(&str[0]);
    order[pos].num_peperoni = atoi(&str[2]);
    order[pos].num_margarita = atoi(&str[4]);
    order[pos].distance = atoi(&str[6]);
    my_pos=pos;
    pos++;	  			
    //xekleidoma tou mutex 
    pthread_mutex_unlock(&mymutex_or);
    //kleidoma tou mutex 
    pthread_mutex_lock(&mymutex_li);		
    // upologismos sunolikou xronou ekteleshs mias paraggelias
    cooking_time = order[my_pos].num_special*T_SPECIAL+order[my_pos].num_peperoni*T_PEPERONI+order[my_pos].num_margarita*T_MARGARITA;
    
   	//kleidoma tou mutex 
     pthread_mutex_unlock(&mymutex_li);		

    int delay;
    
    // begin
    gettimeofday(&tvBegin[my_pos], NULL);
      			
    // shmeio elegxou an uparxei diathesimos psisths 
    //pou tha analavei thn paraggelia
    while(flag==0)
    	{
   	          pthread_mutex_lock(&mymutex_ps);	
  				
    		   for (i=0;i<N_PSISTES;i++)
    				{
    					if (psistes[i]==0)
    					{
    						psistes[i]=1;
    						pos_psisth = i;
    						flag=1;
   						    break;
    					}	
                    }
    		  pthread_mutex_unlock(&mymutex_ps);	
          }
    			
    // kathisterhsh gia thn oloklhrwsh ths paraggelias
    // apo thn meria tou psisth
    sleep(cooking_time);
    			
    pthread_mutex_lock(&mymutex_ps);	
    		
    psistes[pos_psisth]=0;
    			
    pthread_mutex_unlock(&mymutex_ps);;
    		
    // kleidoma tou mutex ths listas paraggeliwn
    pthread_mutex_lock(&mymutex_or);	
    			
    // molis etoimasthke paraggelia			
    order[my_pos].order_status = 1;	
    pthread_mutex_unlock(&mymutex_or);	
     			
    			
    flag = 0;
    // kathorimsmos xronou kathusthrhshs tou dianomea
    // analoga an o pelaths einai konta h makria 
    if (order[my_pos].distance==0)
    	delivery_time = T_KONTA;
    else
    	delivery_time = T_MAKRIA;
    				
    // shmeio elegxou an uparxei diathesimos dianomeas
    //pou tha analavei thn paraggelia	
    while(flag==0)
    	{             
    	   pthread_mutex_lock(&mymutex_di);	
    			
           for (i=0;i<N_DIANOMEIS;i++)
    	  	  if (dianomeis[i]==0)
    		    {
    				dianomeis[i]=1;
    				pos_dianomea = i;
    				flag=1;
    				break;
    			}	
      	    pthread_mutex_unlock(&mymutex_di);	
   		}
    	// kathisterhsh gia thn oloklhrwsh ths paraggelias
    	// apo thn meria tou dianomea
    	sleep(delivery_time);
    
    			
    	// apeleutherwsh tou dianomea wste na 
        // xrhsimopoihthei apo eponenes paraggelies    
    			
    	pthread_mutex_lock(&mymutex_di);	
    	
        dianomeis[pos_dianomea]=0;
        	
    	pthread_mutex_unlock(&mymutex_di);	
    		
     	//timer stop
    	gettimeofday(&tvEnd[my_pos], NULL);  			
        delay = tvEnd[my_pos].tv_sec-tvBegin[my_pos].tv_sec;
      
      	// shmeio elegxou an exoume kathusterhsh megaluterh tou 
    	// epitreptou oriou 
    			
         //periptvsh opou den exoume kathusrerhsh
    	if (delay<T_VERYLONG)
    		{   
               // apostolh paraggelias mesa sta oria
    	    	send_order(connfd,1);
    				
    			pthread_mutex_lock(&mymutex_or);
				    
      		   // h paraggelia exei stalei sthn wra ths			
    			order[my_pos].order_status = 2;	
    			pthread_mutex_unlock(&mymutex_or);
    		}
    	else
    		{
                // apostolh paraggelias ektos oriwn
    			send_order(connfd,2);
    			pthread_mutex_lock(&mymutex_or);
    			
               // h paraggelia exei stalei me kathusterhsh		
    	    	order[my_pos].order_status = 3;
                    	
    			pthread_mutex_unlock(&mymutex_or);
    		}
    			  
        close(connfd);
    
   //kleisimo thread 
    pthread_exit(NULL);
    
      
}//end function

// synarthsh poy elegxei thn lista paraggeliwn 
// me vash thn xronikh periodo pou dinetai sthn ekfwnhsh 
// kai tupwnei to sunolo twn paraggeliwn kai aytwn poy
// exoun kathisterhsei
void* check_orders (void* tmp )
{
      int i,late=0,total=0;
      
      do{    
          late=0,total=0;
          sleep(T_VERYLONG);  
          for (i=0;i<MAX_ORDER_LIST_SIZE;i++)
		    if (order[i].sender != 0)			
		      {
		     	total++;
			    if (order[i].order_status==3)
			    	late++;
		      }
            printf("\nTotal orders:%d\nLate orders:%d\n", total,late);
                  
      }while(1);         
                             
}







