/* Swarthmore College, CS 45, Lab 4
 * Copyright (c) 2020 Swarthmore College Computer Science Department,
 * Swarthmore PA
 * Professor Kevin Webb */

#ifndef __FRAMES_H__
#define __FRAMES_H__

#include <inttypes.h>

#define PHY_FREE (0)
#define PHY_INUSE (1)
#define PHY_RESERVED (2)

/* Represents a physical memory frame. */
struct frame {
    /* Whether the frame is free, in use, or reserved. */
    int status;

    /* We're just simulating memory accesses, so you don't need to worry about
     * associating any data with a frame.  This field is here just to remind
     * you that the frame represents physical storage. */
    void *data;
};
typedef struct frame frame_t;


/****
 Please do not change the prototypes of the functions below.
 ****/

/* Allocates and initializes the physical memory.
 *
 * framesize: The size, in bytes, of one physical frame.
 * physize:   The total size of the physical memory, in bytes.
 *
 * Returns: 0 on success, non-zero on error. */
int init_physical_memory(uint64_t framesize, uint64_t physize);


/* Searches the physical frames for an unused (free) frame.
 *
 * Returns: The index of the lowest-numbered free frame, if there is one.
 *          Otherwise, returns zero if no frames are free.
 *
 * Note: frame zero is assumed to be reserved for the OS, so it's never
 * free.  This function takes advantage of that assumption by returning zero
 * to indicate that no frames are free. */
uint64_t find_free_frame(void);


/* Update the status of a frame.
 *
 * index: The index of the frame to update.
 * status: The frame's new status (PHY_FREE, PHY_INUSE, PHY_RESERVED). */
void set_frame_status(uint64_t index, int status);


/* Frees any memory that was allocated for simulating physical memory. */
void free_physical_memory(void);

#endif
