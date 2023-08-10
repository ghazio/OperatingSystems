/* Swarthmore College, CS 45, Lab 4
 * Copyright (c) 2020 Swarthmore College Computer Science Department,
 * Swarthmore PA
 * Professor Kevin Webb */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "frames.h"

/* For storing an array of physical frames. */
static frame_t *frames;

// TODO: declare any other physical memory state as 'static' here.
static int n_frames;
int init_physical_memory(uint64_t framesize, uint64_t physize) {
    if (physize % framesize != 0) {
        fprintf(stderr, "Frame size must evenly divide PAS size.\n");
        errno = EINVAL;
        return EINVAL;
    }
    n_frames=physize/framesize;

    // Allocate and initialize physical frames.
    frames = malloc(sizeof(frame_t)*n_frames);
    // Frame 0 is reserved for the OS, all others start as free.
    for(int i=0;i < n_frames;i++){
      frames[i].status = 0;

    }
    frames[0].status = 2;

    /* Success */
    return 0;
}

uint64_t find_free_frame(void) {
  //finds free frame
    for(int i=0;i <n_frames;i++){
      if(frames[i].status == 0){
        //if we find a free frame, we return that
        return i;
      }
    }
    /* No free frame found. */
    return 0;
}

void set_frame_status(uint64_t index, int status) {
    //set the status of the frame appropriately
    frames[index].status = status;
}

void free_physical_memory(void) {
    // frees the frames
    free(frames);
}
