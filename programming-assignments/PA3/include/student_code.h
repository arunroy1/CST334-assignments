#ifndef __student_code_h__
#define __student_code_h__

#include <stdbool.h>
#include <stddef.h>

// A sample node structure. The actual structure is defined by the assignment's framework,
// but it will contain these essential fields.
typedef struct node {
    size_t size;      // The size of the data block following this header
    bool is_free;     // Flag to indicate if the block is free
    struct node* next;  // Pointer to the next node in the list
    struct node* prev;  // Pointer to the previous node
} node_t;

// Error codes
enum {
    E_NO_SPACE = 1,
    E_CORRUPT_ARENA,
    E_BAD_POINTER,
    E_NOT_FREED,
};
extern int mem_errno;

// Global variable for the start of the memory arena
extern node_t* g_head;

// Functions you are writing
node_t* find_first_free_chunk(size_t size, node_t* starting_node);
void split_node(node_t* node, size_t size);
node_t* get_header(void* ptr);
void coalesce_nodes(node_t* front, node_t* back);
void* mem_alloc(size_t size);
void mem_free(void* ptr);

#endif