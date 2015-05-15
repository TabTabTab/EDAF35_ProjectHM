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
	node_t* prev;
	bool is_free;
};

void* realloc(void *ptr, size_t size);
void* malloc(size_t size); 
void* calloc(size_t nitems, size_t size); 
void free(void* ptr); 
void merge(node_t* node);
bool are_neighbours(node_t* potential_left, node_t* potential_right);

node_t* get_avail(size_t size);
node_t* create_block(size_t size);

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
	if(size==0){
  		free(ptr);
		return;
  }
  if (!ptr) {
    // NULL ptr. realloc should act like malloc.
    return malloc(size);
  }

  struct node_t* block_ptr = get_node(ptr);
  if (block_ptr->size >= size) {
    
    return ptr;
  }


  void *new_ptr;
  new_ptr = malloc(size);
  if (!new_ptr) {
    return NULL; 
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
//	node_t** last=&block_head;
	while(temp!=NULL){
		if(temp->is_free && (temp->size) >= size){
			return temp;
		}
//		last=&(temp->next);
		temp=temp->next;
	}

	node_t* new_block=create_block(size);
	return new_block;
}

node_t* create_block(size_t size)
{	
	//varför måste man göra detta?
	node_t* new_node=sbrk(0);
	void* request = sbrk(block_node_size+size);
	if (request==SBRK_FAIL){
		return NULL;
	}
		new_node->is_free=true;		
		new_node->size=size;
		new_node->prev=NULL;
	if(block_head==NULL){
		new_node->next=NULL;
	}
	else{
		new_node->next=block_head;
		block_head->prev=new_node;
	}
	block_head=new_node;
	
	return new_node;
}
/*
node_t* create_block2(size_t size,node_t** last)
{	
	//varför måste man göra detta?
	node_t* new_node=sbrk(0);
	void* request = sbrk(block_node_size+size);
	if (request==SBRK_FAIL){
		return NULL;
	}
	//new_node->prev =*last;
	new_node->next=NULL;
	new_node->size=size;
	new_node->is_free=true;
	*last=new_node;
	return new_node;
}
*/



void free(void* ptr)
{
	printf("calling custom free\n");
	if(ptr==NULL){
		return;
	}
	node_t* node=get_node(ptr);
	node->is_free=true;
	merge(node);
}

void merge(node_t* node){
	if(node->next!=NULL && node->next->is_free && are_neighbours(node,node->next)){
		node->size = node->size + block_node_size + node->next->size;
		node->next=node->next->next;
	}
	if(node->prev!=NULL && node->prev->is_free && are_neighbours(node->prev,node)){
		node->prev->size = node->prev->size + block_node_size + node->size;
		node->prev->next=node->next;
	}

}

bool are_neighbours(node_t* potential_left, node_t* potential_right)
{
	char* byte_ptr=(char*)potential_left;
	node_t* right=(node_t*)(byte_ptr + block_node_size + potential_left->size);
	if(right==potential_right){
		return true;
	}else{
		return false;
	}
}

node_t* get_node(void* ptr)
{
	node_t* node=(node_t*)ptr;
	node=node-1;
	return node;
}


