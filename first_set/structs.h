

#define UNIXSTR_PATH "unix.str"
#define MAX_ORDER_LIST_SIZE 2000      
 
typedef struct orderList { 
	int sender;				// id paraggelias
	int num_special;		// arithos apo special pizzes
	int num_peperoni;		// arithos apo peperoni pizzes
	int num_margarita;		// arithos apo margarita pizzes
	int distance;			// apostash 0 gia knonta 1 gia makria
	int order_status;		// se poia fash vrisketai h paraggelia
 } orderList;    

 
orderList order[MAX_ORDER_LIST_SIZE];
orderList the_order;

////////////////////////////
//arxikopoihseis










