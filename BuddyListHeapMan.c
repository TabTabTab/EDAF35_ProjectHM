#define _BSD_SOURCE

#define N 22
#define MEMORY_SIZE 4194304

#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>





typedef struct block_t block_t;

struct block_t{
	bool reserved; /* one if reserved. */
	char kval; /* current value of K. */
	block_t* succ; /* successor block in list. */
	block_t* pred; /* predecessor block in list. */
};
void* malloc(size_t size); 
void* realloc(void* ptr, size_t size);
void* calloc(size_t nitems, size_t size); 
void free(void* ptr);

block_t* get_node(void* ptr);
block_t* merge(block_t* p);
void init_memory();
size_t upper_bound(size_t size);
size_t closest_bigger_power_of_two(size_t size);
size_t power_of_two(char k);



static const size_t block_node_size=sizeof(block_t);
static block_t* free_list[N] = {NULL};
static block_t* memory = NULL;




void* calloc(size_t n_items, size_t item_size)
{

	size_t total_size = n_items * item_size;

	void* ptr = malloc(total_size);
	memset(ptr,0,total_size);
	return ptr;

}


void* realloc(void* ptr, size_t size)
{		
	
  if(size==0){
  		free(ptr);
		return;
  }
  if (!ptr) {
    // NULL ptr. realloc should act like malloc.
    return malloc(size);
  }

  struct block_t* block_ptr = get_node(ptr);
  size_t available_size = power_of_two(block_ptr->kval)-block_node_size;
  if (available_size >= size) {
    
    return ptr;
  }


  void *new_ptr;
  new_ptr = malloc(size);
  if (!new_ptr) {
    return NULL; 
  }
  memcpy(new_ptr, ptr, available_size);
  free(ptr);
  return new_ptr;




	
}




void* malloc(size_t size)
{
	block_t*	buddy1;
	if(memory==NULL){
		init_memory();	
	}

	if(size<=0){
		return NULL;	
	}

	size_t total_size = size + block_node_size;

	if(total_size > MEMORY_SIZE) {
		return NULL;	
	}
	
	size_t k_val = closest_bigger_power_of_two(total_size);

	size_t i;
	block_t* I;
	  for(i=k_val -1; i<N; i++){
		if(free_list[i]!=NULL){
			I=free_list[i];
			break;
		}
	}
	
	  if(I==NULL){
		return NULL;	
	}
	
	


	//block_t* I = free_list[i];
	//For each I : K < I < J split the block in two pieces (vi måste väl splittra det "översta" blocket också? dvs block j?)
    while(i > k_val -1){
		if(I->succ == I){

			free_list[i] = NULL;		
		}
		else{

			I->pred->succ=I->succ;
			I->succ->pred =I->pred;
			free_list[i]=I->succ;		
		}
		I->kval = I->kval -1; //eller egentligen i
	


		
		//inte helt säker på det här blir 1 << i 2^i?
		block_t* buddy = (block_t*) ((char*) I + (1 << i));
		buddy->kval = I->kval;
		buddy->reserved = 0;
	
		//de finns inga block av den här storleken så de ska vara sina egna grannar

		buddy->succ = I;
		buddy->pred = I;
		I->succ = buddy;
		I->pred = buddy;
	
		//stoppa in de i listan
		i--;	
		
		//behövs det här? kan någon t.ex kalla på free medans vi är i malloc så det kommer tillbaka minne?
		block_t* temp = free_list[i];
		if(temp!=NULL){
			buddy->succ = temp;
			I->pred = temp->pred;
			temp->pred->succ = I;
			temp->pred = buddy;	
		
	
		}

		free_list[i]=I;

	}
	
	//returnera vårt block
	block_t* block_to_return =free_list[i];
	
	if (block_to_return->succ == block_to_return) {
		free_list[i] = NULL;
	} 
	else {
		block_to_return->pred->succ = block_to_return->succ;
		block_to_return->succ->pred = block_to_return->pred;
		free_list[i] = block_to_return->succ;
	}

	block_to_return->reserved = 1;
	block_to_return->pred = block_to_return;
	block_to_return->succ = block_to_return;

	return (void*) (block_to_return+1);

	

}




void free(void* ptr)
{
	if(ptr == NULL){
		return;
	}
	//hämta själva noden
	block_t* node = get_node(ptr);
	//merga den
	node = merge(node);
	node->reserved = 0;
	size_t i = node->kval -1;

	//stoppa in i listan
	block_t* temp = free_list[i];
	if(temp!=NULL){
		node->pred = temp->pred;
		node->succ = temp;
		temp->pred->succ = node;
		temp->pred = node;
	}
	free_list[i] = node;

}




block_t* merge(block_t* node_to_merge) 
{


	//testa utan char grejerna
	block_t* buddy = (block_t*) ((char*) memory + (((char*) node_to_merge - (char*) memory) ^ (1 << node_to_merge->kval)));
	
	
	if (node_to_merge->kval < N && !buddy->reserved && buddy->kval == node_to_merge->kval) {
		//vi ska merge:a
		size_t i = node_to_merge->kval - 1;

		// ta bort buddyn
		if (buddy->succ == buddy) {
			free_list[i] = NULL;
		} 
		else {
			buddy->pred->succ = buddy->succ;
			buddy->succ->pred = buddy->pred;
			free_list[i] = buddy->succ;
		}
	
		
		//node_to_merge = p < q ? p : q;
		//hämta första blocket
		if(node_to_merge > buddy){
			node_to_merge = buddy;		
		}
		

		node_to_merge->kval++;
		node_to_merge->pred = node_to_merge;
		node_to_merge->succ = node_to_merge;
		
		//fortsätt tills vi är ensamma
		node_to_merge = merge(node_to_merge);
	}

	return node_to_merge;
}




void init_memory()
{
	memory = sbrk(MEMORY_SIZE);
	memory->reserved = 0;
	memory->kval = N;
	memory->succ = memory;
	memory->pred = memory;
	free_list[N - 1] = memory;

}

block_t* get_node(void* ptr)
{
	block_t* node=(block_t*)ptr;
	node=node-1;
	return node;
}


size_t closest_bigger_power_of_two(size_t value)
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
	size_t two_res=closest_bigger_power_of_two(value);
	return 1<<(two_res);
}
size_t power_of_two(char k)
{
	size_t	pow2_sz;
	char	kval;
	
	pow2_sz = 1;
	kval = 0;
	while (kval < k) {
		pow2_sz <<= 1;
		kval++;
	}

	return pow2_sz;
}
//int main(){}

