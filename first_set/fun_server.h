#include <stdio.h>
#include<stdlib.h>
#include <sys/types.h>	
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SEM1 "sem_psistes"
#define SEM2 "sem_dianomeis"
#define SEM3 "sem_lista"
#define SEM4 "sem_order"

// megethos koinhs mnhmhs
#define SHM_SIZE 100000

// dh;lwsh shmaforwn
sem_t *sem_ps;
sem_t *sem_di;
sem_t *sem_li;
sem_t *sem_or;

int shm_id;

void get_order(int ,char *);
void send_order(int ,int);
char * shared_memory(void);
void semaphores(void);
void close_semaphores(void);
void unlink_semaphores(void);
void sig_chld( int );

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

//dhmioyrgia koinhs mnhmhs
char * shared_memory(void)
  {
    char *data;
    key_t  shmkey;
    shm_id= shmget(shmkey, SHM_SIZE, 0600| IPC_CREAT);
     if (shm_id == -1) {
           printf("Could not create shared memory!\n");
           exit(1);
           }
    data = shmat(shm_id, NULL, 0);
        if(data == (char *)-1) {
          printf("Could not attach to shared memory!\n");
           exit(1);
           }
    return data  ;
  }
  
  
//dhmiourgia shmmforwn
void semaphores(void)
{

  sem_ps = sem_open(SEM1, O_CREAT | O_RDWR,S_IRUSR | S_IWUSR, 1);
    if (sem_ps == SEM_FAILED) {
  	 printf("Could not open semaphore!\n");
  	 exit(1);
	}

   sem_di = sem_open(SEM2, O_CREAT | O_RDWR,S_IRUSR | S_IWUSR, 1);
   if (sem_di == SEM_FAILED) {
   	 printf("Could not open semaphore!\n");
  	 exit(1);
	}

   sem_li = sem_open(SEM3, O_CREAT | O_RDWR,S_IRUSR | S_IWUSR, 1);
   if (sem_li == SEM_FAILED) {
   	 printf("Could not open semaphore!\n");
  	 exit(1);
	}
	
   sem_or = sem_open(SEM4, O_CREAT | O_RDWR,S_IRUSR | S_IWUSR, 1);
   if (sem_or == SEM_FAILED) {
   	 printf("Could not open semaphore!\n");
  	 exit(1);
	}

}

//kleisimo shmaforou
void close_semaphores(void)
{
   sem_close(sem_ps);
   sem_close(sem_di);
   sem_close(sem_li);
   sem_close(sem_or);
}

//diagrafh shmaforou
void unlink_semaphores(void)
{	
    sem_unlink(SEM1);
    sem_unlink(SEM2);
	sem_unlink(SEM3);
    sem_unlink(SEM4);
}


//"zombie" processes.
void sig_chld( int signo )
{
	pid_t pid;
	int stat;

	while ( ( pid = waitpid( -1, &stat, WNOHANG ) ) > 0 ) {
		printf( "Child %d terminated.\n", pid );
	}
}
