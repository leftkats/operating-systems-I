#include <stdio.h>
#include<stdlib.h>

//sunolikos arithmoa spo pitses	
#define N_MAXPIZZA 3

//functions daclaration
void  menu();
void check_order(char*);
void send_order(int, char *) ;
void order_arrival(int );



// emfanish menu
void menu()
{
	printf("******************************************\n");
  	printf(" *\t\t Menu  \t\t\t* \n *\t\t 1. Special  \t\t* \n *\t\t 2. Peperoni  \t\t* \n *\t\t 3. Margarita  \t\t* \n *\t\t 4. Distance  \t\t* \n");
	printf("\n");
	printf("******************************************\n");
	printf("\tPlease fill in the number of pizzas \n\tfor every kind and the distance.\n\tThe numbers should be separated by comas \n\tand the distance is 0 for  near 1 for far.\n");

}

// epilogh paragglias
void check_order(char* order)
{
   char str[7];
   int i;
   
   // h eisodos apo to  plhtkrologio ekteleitai ews otou
   //dwthei swsth eisodos opws orizetai apo to menu
   while(1){
		fgets ( str, 8, stdin );
		i = strlen(str); 
		if(i==7)
			{ 
			if(((atoi(&str[0])+atoi(&str[2])+atoi(&str[4]))<=N_MAXPIZZA ) &&(atoi(&str[6])==0 || atoi(&str[6])==1))
			{
				   strcpy(order,str);
				   return ; 
				}                      
	  
		  }
		else
			printf("Enter a valid order\n") ; 
   }//end of while
}


//apostolh paraggelias
void send_order(int fd , char * str) 		
{    //prwth eggrafh sto socket gia thn apostolh ths paraggelias
 	 write( fd, str, sizeof(char)*8 );
}


// afiksh paraggelias
void order_arrival(int fd)
{
   int x;
   // diavasma tou socket gia thn afiksh ths paraggelias 
   // perimenei enan integer apo to server wste gia kathe 
   //apo tis parakatw periptwseis tupwnei to antistoixo mhnuma
   read(fd,&x,sizeof(int));
    if(x==-1){
              printf("Error in socket read at arrival\n");
              exit(0);	
      }
      
    if (x==1)
       printf("Your order has arrived!!\n");
       
    if (x==2)
       printf("Sorry for the delay, here is your free coke\n");
} 

