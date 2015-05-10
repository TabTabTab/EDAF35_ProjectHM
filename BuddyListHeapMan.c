#define _BSD_SOURCE

#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


//K=10 will result in the total size of 1024 bytes
#define K (10)
#define N (1<<K)


typedef struct block_t block_t;

struct block_t{
	bool reserved; /* one if reserved. */
	char kval; /* current value of K. */
	block_t* succ; /* successor block in list. */
	block_t* pred; /* predecessor block in list. */
};




static block_t** free_list;
static char memory[N];



void* realloc(void* ptr, size_t size);
void* malloc(size_t size); 
void* calloc(size_t nitems, size_t size); 
void free(void* ptr);

size_t upper_bound(size_t size);
size_t two_pot(size_t size);


void* malloc(size_t size)
{


}

size_t two_pot(size_t value)
{
	size_t one=0x1;
	
	size_t nbr_bits=0;
	size_t nbr_ones=0;

	while(value>0){
		nbr_bits++;
		if(value & one == one){
			nbr_ones++;
		}
		value=value>>1;
	}

	if(nbr_ones>1){
		return nbr_bits;
	}else{
		return nbr_bits-1;
	}
}


size_t upper_bound(size_t value)
{
	size_t two_res=two_pot(value);
	return 1<<(two_res);
}


int main()
{
	printf("doing nothing..\n");
	size_t n=128;
	printf("%d\n",two_pot(n));
	printf("%d\n",upper_bound(n));
}
