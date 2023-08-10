/* Swarthmore College, CS 45, Lab 4
 * Copyright (c) 2020 Swarthmore College Computer Science Department,
 * Swarthmore PA
 * Professor Kevin Webb */
/*Ghazi Randhwawa and Sidhika Tripathee*/
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "pages.h"

/* For storing the page table. */
static page_t *page_table;

/* For keeping track of which replacement policy we're using. */
static int policy;
//maintain head and tail
static struct page_entry* head;
static struct page_entry* tail;

// TODO: declare any other page table state as 'static' here.
static int page_table_size;
static int clock_hand;
//static int* LRU_replacement_tracker;
// TODO: declare any helper functions as 'static' (only usable in this file)

void insert_head(uint64_t p_index){
  //if the tail is equal to heads
    if(head==tail&& head==&page_table[p_index]){
      //we do nothing
    }
    //if tail is equal to current index
    else if(&page_table[p_index]==tail){
      tail->prev=head;
      tail=tail->next;
      tail->prev=NULL;
      head->next=&page_table[p_index];
      head=&page_table[p_index];
      head->next=NULL;
    }
    //if head is equal to the page under consideration
    else if(head==&page_table[p_index]){
      //we do nothing
    }
    //otherwise
    else{
      if(&page_table[p_index]!=head){
        page_table[p_index].prev->next=page_table[p_index].next;
      }

      if(&page_table[p_index]!=tail){
        page_table[p_index].next->prev=page_table[p_index].prev;
        }
        page_table[p_index].prev=head;
        page_table[p_index].prev->next=&page_table[p_index];
        head=&page_table[p_index];
        head->next=NULL;
    }

}


int init_page_table(uint64_t pagesize, uint64_t virtsize, int replace_pol) {
    if (virtsize % pagesize != 0) {
        fprintf(stderr, "Page size must evenly divide VAS size.\n");
        errno = EINVAL;
        return EINVAL;
    }

    switch (replace_pol) {
        case POL_LRU:
            fprintf(stderr, "DEBUG: Using LRU replacement policy.\n");
            policy = replace_pol;
            break;

        case POL_CLOCK:
            fprintf(stderr, "DEBUG: Using Clock replacement policy.\n");
            policy = replace_pol;
            break;

        case POL_CUSTOM:
            fprintf(stderr, "DEBUG: Using Custom replacement policy.\n");
            policy = replace_pol;
            break;

        default:
            fprintf(stderr, "Unknown replacement policy: %d\n", replace_pol);
            errno = EINVAL;
            return EINVAL;
    }

    //Allocate and initialize page table.
    page_table_size = virtsize/pagesize;
    page_table = malloc(sizeof(page_t)*(page_table_size));
    for(int i=0;i<page_table_size;i++){
      page_table[i].valid = 0;
      page_table[i].reference = 0;
      page_table[i].frame = 0;
      page_table[i].next = NULL;
      page_table[i].prev = NULL;
      page_table[i].counter=0;
    }

    head=NULL;
    tail=NULL;
    clock_hand = 0;

    /* Success */
    return 0;
}
/* Look up an entry in the page table.  If it's valid, return the index
 * of the physical frame it lives in
 */
uint64_t access_page(uint64_t p_index) {

  //each time we access a page, we increase its counter by
  page_table[p_index].counter++;

  //if page is valid
  if(page_table[p_index].valid == 1){
    //set the reference bit to 1
    page_table[p_index].reference = 1;
    //adjust the list
    if(policy==1){
      insert_head(p_index);
    }

    //return the relevant frame
    return page_table[p_index].frame;
  }
  //adjust the linked list if the policy is equal to 1
  if(policy==1){
    // if there was no head or tail
    if(head==NULL&&tail==NULL){
      head=&page_table[p_index];
      tail=&page_table[p_index];
    }
    //otherwise we adjust the list
    else{
        page_table[p_index].prev=head;
        page_table[p_index].prev->next=&page_table[p_index];
        head=&page_table[p_index];
        head->next=NULL;
      }
    }

    //if page is not found in memory, return zero
    return 0;
}

void map_page(uint64_t p_index, uint64_t f_index) {
    //Assign a page to a physical frame.
    page_table[p_index].frame = f_index;
    page_table[p_index].valid = 1;
    page_table[p_index].reference = 1;

}

/* Evict a page from physical memory, mark it invalid, and return the (now
 * free) frame number it was living in.*/
uint64_t evict_page(void) {
  uint64_t frame_number=0;
  //if LRU is used
    if(policy==1){
      //get the frame at the tail, and mark it as invalid, and shift the tail onward
      frame_number=tail->frame;
      tail->valid = 0;
      tail=tail->next;
      tail->prev=NULL;
      head->next=NULL;
    }
    //if policy was clock algorithm
    if(policy ==2){
      //while we dont get a valid frame number to be evicted
      while (frame_number == 0){
        //if the page is invalid
        if(page_table[clock_hand].valid == 1){
          //if page should be evicted
          if(page_table[clock_hand].reference == 0){
            //set the valid bit to 0 and break the loop
            page_table[clock_hand].valid = 0;
            frame_number = page_table[clock_hand].frame;
            break;
          }
          else{
         //if ref bit is one, set it to zero
          page_table[clock_hand].reference = 0;
          }
        }
        //increment the clock hand index
        clock_hand++;
        //if it overflows, set it back to 0
        if(clock_hand ==page_table_size){
          clock_hand = 0;
        }
      }

    }
    //if it is our own policy
    if(policy==3){

      int lowest_checks=0;
      uint64_t p_index_lowest=0x0;
      //search for the lowest number of checks for a valid p_index
      for(int j=0;j<page_table_size;j++){
        if(page_table[j].valid==1){
          if(lowest_checks>page_table[j].counter){
            lowest_checks=page_table[j].counter;
            p_index_lowest=j;
          }
        }
      }
      //free the frame and mark the p_index invalid
      frame_number=page_table[p_index_lowest].frame;
      page_table[p_index_lowest].valid=0;
    }
    return frame_number;
}

void free_page_table(void) {
    // free page page_table
    free(page_table);
}
