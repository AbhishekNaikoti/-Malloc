/**
 * Assignment 1: ++Malloc (mymalloc.c)
 * 
 * @author Taran Suresh ts875
 * @author Abhishek Naikoti an643
 */

#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"

static char myblock[MYBLOCK_SIZE]; // array simluating main memory

/**
 * MetaData is the node container which provides information about its respective user data block.
 */ 
typedef struct MetaData {
    char blockstatus; // used = 'U' / free = 'F'
    unsigned int blocklength; // user defined data memory block size
    struct MetaData *next; // pointer to next MetaData node
} MetaData;

/**
 * Linked list function to coalesce adjacent free user data blocks. Doing so
 * provides the user with the most amount of data to allocate. 
 */ 
void coalesce () {
    MetaData *head = NULL; // head pointer to linked list in myblock
    MetaData *curr = NULL; // curr pointer to traverse linked list

    // printf("Coalescing...\n");

    head = (MetaData*) myblock;
    curr = head;
    while (curr != NULL) {
        if (curr->next == NULL) { // no potential adjacent user data block to coalesce with current
            return;
        } else {
            if (curr->blockstatus == 'F' && (curr->next)->blockstatus == 'F') { // adjacent blocks are free, so coalesce them
                curr->blocklength = curr->blocklength + (curr->next)->blocklength + METADATA_SIZE; // compute total size of current user data block size + next userdata block size + extra MetaData size
                curr->next = (curr->next)->next; // fix linking after deleting next user data block
            } else {
                curr = curr->next;
            }
        }
    }

    return;
}

/**
 * mymalloc() is a better version of malloc() that does not allow the user to do Bad Things. Allows the user
 * to request sizes of data to use for their own needs, but also provides errors if invalid requests are made.
 * @param[in] user requested size
 * @param[in] file wherein user called malloc, to report errors if an invalid call to malloc occurred
 * @param[in] line number from file wherein user called malloc, to report errors if an invalid call to malloc occurred
 * @param[out] void* pointer to start address of user data block
 */ 
void* mymalloc (size_t size, char* file, int line) {
    MetaData *head = NULL; // head pointer to linked list in myblock
    MetaData *curr = NULL; // curr pointer to traverse linked list
    MetaData *new_node = NULL; // new MetaData node to be added when new memory is allocated by user

    /* edge cases where invalid amount of bytes was requested by user */
    if (size == 0) {
        printf("Malloc Error: User attempted to allocate 0 bytes of memory in file: %s on line %d\n", file, line);
        return NULL;
    } else if (size < 0) {
        printf("Malloc Error: User attempted to allocate a negative number of bytes of memory in file: %s on line %d\n", file, line);
        return NULL;
    } else if (size >= MYBLOCK_SIZE) {
        printf("Malloc Error: User attempted to allocate more than available number of bytes of memory in file: %s on line: %d\n", file, line); 
        return NULL;        
    }

    /* initialization of MetaData linked list on first pass */    
    if (myblock[0] == 0) {
        head = (MetaData*) myblock; // pointing head pointer of MetaData linked list to 0th index of myblock on first pass
        head->blockstatus = 'F';
        head->blocklength = MYBLOCK_SIZE - METADATA_SIZE;
        head->next = NULL;
    }

    /**
     * Coalescing the free memory blocks (if any) before checking to malloc. This
     * could potentially provide the user with the space they are asking for
     * rather than just telling them that there is not enough space.
     */
    coalesce(); 

    head = (MetaData*) myblock; // pointing head pointer of MetaData linked list to 0th index of myblock for subsequent passes after first
    curr = head;
    while (curr != NULL) {
        if (curr->blockstatus == 'U') { // current user data block is occupied
            curr = curr->next;
        } else {
            if (curr->blocklength == size) { // user requested data fits exactly within current MetaData block
                curr->blockstatus = 'U'; // current MetaData block is occupied now
                
                curr++; // increment curr by sizeof(MetaData) bytes to point to user data block
                return (void*)(curr);
            } else if (curr->blocklength < (size + METADATA_SIZE)) { // free MetaData block, but not large enough
                curr = curr->next;
            } else if (curr->blocklength >= (size + METADATA_SIZE)) {
                // new MetaData node is sitting at location: current MetaData address + user requested size + METADATA_SIZE
                // example: current MetaData at address 0
                // user requested size is 100 bytes
                // METADATA_SIZE is 16 bytes
                // new MetaData node is sitting at 0 + 100 + 16 = address 116
                new_node = (MetaData*) (void*) ((void*)curr + size + METADATA_SIZE);
                new_node->next = curr->next;
                new_node->blockstatus = 'F';
                new_node->blocklength = curr->blocklength - size - METADATA_SIZE;

                curr->next = new_node;
                curr->blocklength = size;
                curr->blockstatus = 'U';

                curr++; // increment curr by sizeof(MetaData) bytes to point to user data block
                return (void*)(curr);
            }
        }
    }

    printf("Malloc Error: User attempted to allocate more than available number of bytes of memory in file: %s on line: %d\n", file, line); 
    return NULL;
}

/**
 * myFree() is a better version of free() that does not allow the user to do Bad Things. Allows the 
 * user to free valid pointers to data that had been previously allocated with mymalloc(), 
 * but also provides errors if invalid requests are made.
 * @param[in] pointer to user data block to be freed
 * @param[in] file wherein user called malloc, to report errors if an invalid call to malloc occurred
 * @param[in] line number from file wherein user called malloc, to report errors if an invalid call to malloc occurred
 */ 
void myfree (void* ptr, char* file, int line) {
    MetaData *head = NULL; // head pointer to linked list in myblock
    MetaData *curr = NULL; // curr pointer to traverse linked list
    MetaData *addressZero = NULL; // pointer to first address, equivalent to index 0 in myblock

    /* edge case where memory has not yet been initialized */
    if (myblock[0] == 0) {
        printf("Free Error: User attempted to free invalid pointer in file: %s on line: %d\n", file, line);
        return;
    }

    addressZero = (MetaData*) myblock;

    /* edge case where user inputted pointer is outside of memory bounds */
    if (((unsigned long) ptr <= (unsigned long) addressZero) || ((unsigned long) ptr >= ((unsigned long) addressZero + (unsigned long) MYBLOCK_SIZE))) {
        printf("Free Error: User attempted to free pointer outside of memory block in file: %s on line: %d\n", file, line);
        return;
    }

    head = (MetaData*) myblock; // pointing head pointer of MetaData linked list to 0th index of myblock
    curr = head;
    while (curr != NULL) {
        if ((curr + 1) == ptr) { // valid pointer found, but blockstatus needs to be checked
            if (curr->blockstatus == 'U') { // if current user data block is being used, then make it free
                curr->blockstatus = 'F';
                return;
            } else if (curr->blockstatus == 'F') { // valid pointer found, but it is already a free user data block
                printf("Free Error: User attempted to free pointer to already free user data block in file: %s line: %d\n", file, line);
                return;
            }
        } else {
            curr = curr->next;
        }
    }

    printf("Free Error: User attempted to free invalid pointer in file: %s line: %d\n", file, line);
    return;
}

/**
 * myblock function to print contents of MetaData and user data
 */ 
void printMemory () {
    for (int i = 0; i < MYBLOCK_SIZE; i++) {
        // if (myblock[i] == 0) {
        //     printf("#");
        // } else {
            printf("i: %d, %d ", i, myblock[i]);
        // }
    }
}

/**
 * Linked list function to print contents of each MetaData block
 */
void printMetaData () {
    MetaData *curr_ptr = (MetaData*) myblock;
    MetaData *addressZero;
    int count = 0; // current MetaData node

    if (curr_ptr == NULL) {
        printf("Empty MetaData list");
    }

    printf("---------------------------------------\n");
    printf("MetaData size: %ld\n", METADATA_SIZE);
    
    addressZero = (MetaData*) myblock; // taking first MetaData address, and then subtracting from current MetaData address to convert to 0 - 4095
    printf("addressZero: %lu\n", (unsigned long) addressZero);
    while (curr_ptr != NULL) {
        printf("---------------------------------------\n");

        printf("MetaData #%d at address: %lu\n", count, ((unsigned long) curr_ptr - (unsigned long) addressZero));
        printf("Blockstatus: %c\n", curr_ptr->blockstatus);
        printf("Blocklength: %d\n", curr_ptr->blocklength);

        if (curr_ptr->next == NULL) {
            printf("Next MetaData block: %p\n", curr_ptr->next);
        } else {
            printf("Next MetaData block: %lu\n", ((unsigned long) curr_ptr->next - (unsigned long) addressZero));
        }

        curr_ptr = curr_ptr->next;
        count++;
    }

    printf("\n");
}