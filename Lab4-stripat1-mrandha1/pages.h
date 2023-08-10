/* Swarthmore College, CS 45, Lab 4
 * Copyright (c) 2020 Swarthmore College Computer Science Department,
 * Swarthmore PA
 * Professor Kevin Webb */

#ifndef __PAGES_H__
#define __PAGES_H__

#include <inttypes.h>

#define POL_LRU (1)
#define POL_CLOCK (2)
#define POL_CUSTOM (3)

/*
FUnction: maintains the linked list for the LRU algorithm
inputs: the p_index of our input
returns: nothing
*/
void insert_head(uint64_t p_index);



/* Represents a page table entry. */
struct page_entry {
    int valid;
    uint64_t frame;
    // TODO: add fields to track replacement state
    struct page_entry* next;
    struct page_entry* prev;
    //reference bit for clock algorithm
    int reference;
    //for our own policy
    int counter;


};
typedef struct page_entry page_t;


/****
 Please do not change the prototypes of the functions below.
 ****/

/* Allocates and initializes the page table.
 *
 * pagesize: The size, in bytes, of one page.
 * virtsize: The size of the virtual address space, in bytes.
 * replace_pol: Which page replacement policy to use.
 *
 * Returns: 0 on success, non-zero on error. */
int init_page_table(uint64_t pagesize, uint64_t virtsize, int replace_pol);


/* Look up an entry in the page table.  If it's valid, return the index
 * of the physical frame it lives in and update page replacement state
 * (i.e., this page is now the most recently used page).
 *
 * p_index: The index (row number) to look up in the page table.
 *
 * Returns: If the page is in memory, this returns the index of the physical
 *          frame containing the page.  Otherwise, return zero to indicate a
 *          page fault. */
uint64_t access_page(uint64_t p_index);


/* Assign a page to a physical frame.
 *
 * p_index: The index (row number) of the page that is being assigned a frame.
 * f_index: The index of the frame the page is being assigned to. */
void map_page(uint64_t p_index, uint64_t f_index);


/* Evict a page from physical memory, mark it invalid, and return the (now
 * free) frame number it was living in.
 *
 * Returns: The index of the frame the evicted page was occupying.
 *
 * Note: This function should choose a victim page according to the selected
 * page replacement policy. */
uint64_t evict_page(void);


/* Free any memory that was allocated for simulating the page table. */
void free_page_table(void);


#endif
