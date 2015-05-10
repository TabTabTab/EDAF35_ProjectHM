#define _BSD_SOURCE

#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>



typedef struct node_t node_t;

struct node_t{
	size_t size;
	node_t* next;
	bool is_free;
};

void* realloc(void *ptr, size_t size);
void* malloc(size_t size); 
void* calloc(size_t nitems, size_t size); 
void free(void* ptr); 


node_t* get_avail(size_t size);
node_t* create_block(size_t size,node_t** last);

node_t* get_node(void* ptr);

static const size_t block_node_size=sizeof(node_t);

static const void* SBRK_FAIL=(void*) -1;
static node_t* block_head=NULL;

void* calloc(size_t nitems, size_t elem_size)
{
	printf("calling custom calloc\n");
	size_t size=nitems*elem_size;
	void* ptr=malloc(size);
	if(ptr==NULL){
		return NULL;
	}
	memset(ptr,0,size);
	return ptr;
}

void* malloc(size_t size)
{
	printf("calling custom malloc\n");
	if(size<=0){
		return NULL;	
	}

	node_t* node=get_avail(size);

	if(node==NULL){
		return NULL;
	}
	node->is_free=false;	
	//node+1 in order to evade returning the node itself, node+1 will return a ptr to the actual data block
	return node+1;
}

void *realloc(void *ptr, size_t size) {
	printf("calling custom realloc\n");
  if (!ptr) {
    // NULL ptr. realloc should act like malloc.
    return malloc(size);
  }

  struct node_t* block_ptr = get_node(ptr);
  if (block_ptr->size >= size) {
    // We have enough space. Could free some once we implement split.
    return ptr;
  }

  // Need to really realloc. Malloc new space and free old space.
  // Then copy old data to new space.
  void *new_ptr;
  new_ptr = malloc(size);
  if (!new_ptr) {
    return NULL; // TODO: set errno on failure.
  }
  memcpy(new_ptr, ptr, block_ptr->size);
  free(ptr);
  return new_ptr;
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
		last=&(temp->next);
		temp=temp->next;
	}

	node_t* new_block=create_block(size,last);
	return new_block;
}

node_t* create_block(size_t size,node_t** last)
{	
	//varför måste man göra detta?
	node_t* new_node=sbrk(0);
	void* request = sbrk(block_node_size+size);
	if (request==SBRK_FAIL){
		return NULL;
	}
	new_node->next=NULL;
	new_node->size=size;
	new_node->is_free=true;
	*last=new_node;
	return new_node;
}



void free(void* ptr)
{
	printf("calling custom free\n");
	if(ptr==NULL){
		return;
	}
	node_t* node=get_node(ptr);
	node->is_free=true;
}

node_t* get_node(void* ptr)
{
	node_t* node=(node_t*)ptr;
	node=node-1;
	return node;
}

/*
int main()
{
	printf("\n==================\nwe are running..\n==================\n\n");
	void* mem=malloc(10);
	void* mem2=malloc(10);
	printf("mem: %d\n",mem);
	printf("mem2: %d\n",mem2);
	free(mem2);
}
*/
