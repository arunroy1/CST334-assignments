#include "student_code.h"

// The global head of the memory list and error variable
node_t* g_head = NULL;
int mem_errno = 0;

/**
 * Finds the first free chunk of memory that is large enough to hold the requested size.
 */
node_t* find_first_free_chunk(size_t size, node_t* starting_node) {
    node_t* current = starting_node;
    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    // Return NULL if no suitable chunk is found
    return NULL;
}

/**
 * Splits a node if it's larger than the required size plus the space for a new header.
 */
void split_node(node_t* node, size_t size) {
    // Check if there's enough space for the requested block, a new header,
    // and a minimal data block (e.g., 8 bytes).
    if (node->size >= size + sizeof(node_t) + 8) {
        // Calculate the address for the new node's header
        node_t* new_node = (node_t*)((char*)node + sizeof(node_t) + size);
        
        // Configure the new free node
        new_node->is_free = true;
        new_node->size = node->size - size - sizeof(node_t);
        new_node->next = node->next;
        new_node->prev = node;
        
        // Update the original node
        node->size = size;
        if (node->next != NULL) {
            node->next->prev = new_node;
        }
        node->next = new_node;
    }
}

/**
 * Given a pointer to user data, returns a pointer to the block's header.
 */
node_t* get_header(void* ptr) {
    if (ptr == NULL) {
        return NULL;
    }
    // The header is located just before the data pointer
    return (node_t*)((char*)ptr - sizeof(node_t));
}

/**
 * Merges two adjacent free nodes (front and back) into a single larger free node.
 */
void coalesce_nodes(node_t* front, node_t* back) {
    // Coalesce only if both nodes are valid and free
    if (front != NULL && back != NULL && front->is_free && back->is_free) {
        // The new size is the sum of both data sizes plus the size of one header
        front->size += sizeof(node_t) + back->size;
        front->next = back->next;
        if (back->next != NULL) {
            back->next->prev = front;
        }
    }
}

/**
 * Allocates a block of memory of the requested size.
 */
void* mem_alloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    
    // Find a suitable free chunk
    node_t* free_chunk = find_first_free_chunk(size, g_head);

    if (free_chunk == NULL) {
        mem_errno = E_NO_SPACE;
        return NULL;
    }

    // Split the chunk if it's too large
    split_node(free_chunk, size);
    
    // Mark the chunk as allocated
    free_chunk->is_free = false;
    
    // Return a pointer to the data area (after the header)
    return (void*)((char*)free_chunk + sizeof(node_t));
}

/**
 * Frees a previously allocated block of memory.
 */
void mem_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }

    // Get the header for this memory block
    node_t* node_to_free = get_header(ptr);
    
    // Basic validation
    if (node_to_free->is_free) {
        mem_errno = E_BAD_POINTER;
        return;
    }
    
    // Mark the node as free
    node_to_free->is_free = true;

    // Try to coalesce with the next node
    if (node_to_free->next != NULL && node_to_free->next->is_free) {
        coalesce_nodes(node_to_free, node_to_free->next);
    }
    
    // Try to coalesce with the previous node
    if (node_to_free->prev != NULL && node_to_free->prev->is_free) {
        coalesce_nodes(node_to_free->prev, node_to_free);
    }
}