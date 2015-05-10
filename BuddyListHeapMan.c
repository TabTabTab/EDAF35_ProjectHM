#define _BSD_SOURCE

#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define _BSD_SOURCE

#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct block_t block_t;

struct block_t{
	bool reserved; /* one if reserved. */
	char kval; /* current value of K. */
	list_t* succ; /* successor block in list. */
	list_t* pred; /* predecessor block in list. */
};

void* realloc(void *ptr, size_t size);
void* malloc(size_t size); 
void* calloc(size_t nitems, size_t size); 
void free(void* ptr);


