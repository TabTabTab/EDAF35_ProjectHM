#define _BSD_SOURCE


#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


typedef struct node_t node_t;

struct node_t{
	size_t size;
	node_t* next;
	bool is_free;
};

void* malloc(size_t size); 
void free(void* ptr); 


node_t* get_avail(size_t size);
node_t* create_block(size_t size,node_t** last);

node_t* get_prt_node(void* ptr);

static const size_t block_node_size=512;

static const void* SBRK_FAIL=(void*) -1;
static node_t* block_head=NULL;


void* malloc(size_t size)
{
	printf("calling custom malloc\n");
	node_t* node=get_avail(size);

	if(node==NULL){
		return NULL;
	}
	node->is_free=false;	
	//node+1 in order to evade returning the node itself, node+1 will return a ptr to the actual data block
	return node+1;
}

/*
Returns null if failure
*/
node_t* get_avail(size_t size)
{
	//first try to find a usable block
	node_t* temp=block_head;

	//we need to keep track of last so that we can add to the last element later on
	node_t** last=&block_head;
	while(temp!=NULL){
		if(temp->is_free && (temp->size) >= size){
			return temp;
		}
		node_t** last=&(temp->next);
		temp=temp->next;
	}

	node_t* new_block=create_block(size,last);
	return new_block;
}

node_t* create_block(size_t size,node_t** last)
{
	node_t* new_node=sbrk(0);
	void* test=sbrk(0);
	void* request = sbrk(block_node_size+size);
	if (request==SBRK_FAIL){
		return NULL;
	}
	memset(request,0,block_node_size+size);
	*last=new_node;
	return new_node;
}



void free(void* ptr)
{
	printf("calling custom free\n");
	node_t* node=get_prt_node(ptr);
	if(node == NULL){
		fprintf(stderr,"trying to free unallocated memory\n");
		exit(1);
	}
	else{
		node->is_free=true;
	}
}

node_t* get_prt_node(void* ptr)
{
	node_t* temp=block_head;
	while(temp!=NULL){
		void* potential=(void*)temp+1;
		if(potential==ptr){
			return temp;
		}
	}
	return NULL;
}



int main()
{
	printf("\n==================\nwe are running..\n==================\n\n");
	void* mem=malloc(10);
	void* mem2=malloc(10);
	free(mem2);
}