/* Swarthmore College, CS 45, Lab 4
 * Copyright (c) 2020 Swarthmore College Computer Science Department,
 * Swarthmore PA
 * Professor Kevin Webb */

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "pages.h"
#include "frames.h"

/* Tests whether a value is a power of two. */
int power_of_two (uint64_t operand) {
  return (operand != 0 && (operand & (operand - 1)) == 0);
}

/* Returns the integral log (base 2) of an integer. */
uint64_t log2_integer(uint64_t operand) {
    uint64_t log = 0;
    while (operand != 1) {
        operand = operand >> 1;
        log += 1;
    }
    return log;
}
/*Calculates and Returns the right Physical address when given the offset size and the
frame number*/
uint64_t physical_translator(uint64_t frame_number, uint64_t offset, int offset_size){
    frame_number=frame_number<<offset_size;
    uint64_t physical_address=frame_number+offset;
    return physical_address;

}
/* Map a file into memory and return it as an array of uint64_t integers.
 *
 * filename: The name of the file to open and map.
 * file_size: A pointer that this function should fill in to indicate how many
 *            bytes are in memory mapped file.
 *
 * Returns: an array of unsigned, 8-byte integers representing the mapped
 *          file's contents. */
uint64_t *memory_map_file(char *filename, off_t *file_size) {
    int fd;
    struct stat status;
    uint64_t *addrs = NULL;

    /* Open the input file. */
    if ((fd = open(filename, O_RDONLY)) == -1) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        exit(1);
    }

    /* Use stat() to get the file's details (for us, the st_size field). */
    if (fstat(fd, &status)) {
        fprintf(stderr, "fstat: %s\n", strerror(errno));
        exit(1);
    }

    /* Map the file into this process's virtual address space. */
    //  Map the file with mmap() and set 'addrs'
    addrs =  mmap(NULL,status.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    //  Set file_size based on the 'status' struct's st_size field.
    *file_size=status.st_size;
    /* Close the file. */
    if (close(fd)) {
        fprintf(stderr, "close: %s\n", strerror(errno));
        exit(1);
    }

    return addrs;
}

/* The translate function takes the virtual address and prints the
 * corresponding physical address as a 64-bit hex number.
 * If a page fault occurs, it should increment the fault counter.
 * You'll need to pass it some information about address sizes so that
 * it can determine how to split the address into page number / offset. */

void translate(uint64_t virt_addr, int *faults,int offset_size,int virtual_address_size) {
    //right shift the virtual address by the offset size
    uint64_t page_num=virt_addr>>offset_size;

    int num = (1<<offset_size) - 1;
    //use the and function to get the right offset address
    uint64_t offset_address = virt_addr&num;
    uint64_t physical_address=0x0;
    // Use this format to print the final physical address.

    uint64_t page_index=access_page(page_num);
    if(page_index==0x0){//if the page is not already in physical memory
      *faults=*faults+1;//increment the fault count
      uint64_t free_frame_index=find_free_frame();//checks for free frame
        if(free_frame_index==0x0){//if there are no free frames
          uint64_t frame_evicted = evict_page();
          //evict a frame accoridng to policy and map it
          map_page(page_num,frame_evicted);
          physical_address=physical_translator(frame_evicted,offset_address,offset_size);

        }
        else{//if there was a free frame, map the page to the free frame
          set_frame_status(free_frame_index,1);
          map_page(page_num,free_frame_index);
           physical_address=physical_translator(free_frame_index,offset_address,offset_size);
        }
    }
    else{
      // alter the frame variables
       physical_address=physical_translator(page_index,offset_address,offset_size);
    }
    printf("0x%016lX\n",physical_address);

}


int main(int argc, char **argv) {
    int i, faults = 0;
    off_t file_size = 0;
    char *end = NULL;
    uint64_t *addrs;
    uint64_t pagesize, virtsize, physize;

    /* Verify that we're given the right number of arguments. */
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <page size (bytes)> <VAS size (bytes)> <PAS size (bytes)> <policy> <filename>\n", argv[0]);
        exit(1);
    }

    /* Convert string arguments to unsigned long integers. */
    errno = 0;
    pagesize = strtoul(argv[1], &end, 10);
    if (errno || argv[1] == end) {
        fprintf(stderr, "Invalid page size: %s\n", argv[1]);
        exit(1);
    }

    errno = 0;
    virtsize = strtoul(argv[2], &end, 10);
    if (errno || argv[2] == end) {
        fprintf(stderr, "Invalid virtual address space size: %s\n", argv[2]);
        exit(1);
    }

    errno = 0;
    physize = strtoul(argv[3], &end, 10);
    if (errno || argv[3] == end) {
        fprintf(stderr, "Invalid physical address space size: %s\n", argv[3]);
        exit(1);
    }

    /* Verify powers of two. */
    if (!power_of_two(pagesize) || !power_of_two(virtsize)) {
        fprintf(stderr, "The page size and VAS size must be powers of two.\n");
        exit(1);
    }

    /* Initialize memory structures. */
    if (init_page_table(pagesize, virtsize, atoi(argv[4]))) {
        perror("init_page_table");
        fprintf(stderr, "Couldn't initialize page table!\n");
        exit(1);
    }
    if (init_physical_memory(pagesize, physize)) {
        perror("init_physical_memory");
        fprintf(stderr, "Couldn't initialize physical memory!\n");
        free_page_table();
        exit(1);
    }

    /* Map the input file into memory. */
    addrs = memory_map_file(argv[5], &file_size);

    /* Loop through the file as an array in memory, printing the values. */
    for (i = 0; i < file_size / sizeof(uint64_t); ++i) {
        fprintf(stderr, "\nDEBUG: translating virtual address: 0x%016lX\n", addrs[i]);

        // TODO: Call translate, which should determine and print the physical address.
        //translate(addrs[i], &faults, ...);
        translate(addrs[i],&faults,log2_integer(pagesize),log2_integer(virtsize));
    }

    /* After all translation is done, print the total number of page faults. */
    printf("%d\n", faults);

    /* Clean up allocated memory. */
    free_page_table();
    free_physical_memory();

    /* Unmap the input file from memory. */
    if (munmap(addrs, file_size)) {
        fprintf(stderr, "munmap: %s\n", strerror(errno));
        exit(1);
    }

    return 0;
}
