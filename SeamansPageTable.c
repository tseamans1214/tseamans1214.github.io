/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SeamansPageTable.c
 * Author: tommy
 *
 * Created on October 1, 2020, 10:42 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PageTable.h"
/*
 * 
 */
//forward declarations for structs
struct page_table_entry{
    // 1 byte, 8 bits in a byte
    // Stores bothe the dirty bit and the valid/invalid bit
    // The right most bit will be the valid/invalid bit.
    // The second bit from the right will be the dirty bit
    // Dirty bit should always be 0
    // 000000VD
    int frameNum;
    unsigned int dirty_valid_bits;
    int usedCount;
};
struct page_table{
    int page_count;
    int frame_count;
    enum replacement_algorithm algorithm;
    struct page_table_entry* pages;
    int numFaults;
    int firstIn;
    int lru;
    int mfu;
    int numFramesUsed;
    
};

/**
 * Creates a new page table object. Returns a pointer to created page table.
 *
 * @param page_count Number of pages.
 * @param frame_count Numbers of frames.
 * @param algorithm Page replacement algorithm
 * @param verbose Enables showing verbose table contents.
 * @return A page table object.
 */
struct page_table* page_table_create(int page_count, int frame_count, enum replacement_algorithm algorithm, int verbose)
{
    struct page_table* pt = (struct page_table*)malloc(sizeof(struct page_table));
    pt->page_count = page_count;
    pt->frame_count = frame_count;
    pt->algorithm = algorithm;
    pt->pages = (struct page_table_entry*)malloc(sizeof(struct page_table_entry)*pt->page_count);
    for (int i=0; i<pt->page_count; i++)
    {
        //pt->pages[i] = (struct page_table_entry)malloc(sizeof(struct page_table_entry));
        pt->pages[i].dirty_valid_bits = 0;
    }     
    
    pt->numFaults = 0;
    pt->firstIn = -1;
    pt->numFramesUsed = 0;
    return pt;
}

/**
 * Destroys an existing page table object. Sets outside variable to NULL.
 *
 * @param pt A page table object.
 */
void page_table_destroy(struct page_table** pt)
{
    struct page_table* ptTemp = *pt;
    for (int i=0; i<ptTemp->page_count; i++)
    {
        //free(ptTemp->pages[i].dirty_valid_bits);
        //free(&ptTemp->pages[i].frameNum);
        //free(&ptTemp->pages[i].usedCount);
        //free(&ptTemp->pages[i]);
        
    }
    //free(ptTemp->pages);
    //free(ptTemp);
    //ptTemp = NULL;
}

/**
 * Simulates an instruction accessing a particular page in the page table.
 *
 * @param pt A page table object.
 * @param page The page being accessed.
 */
void page_table_access_page(struct page_table *pt, int page)
{
    
    if (pt->algorithm == 0) // FIFO --------------------------------------------
    {
        if (pt->firstIn == -1) // fills the first frame
        {
            pt->pages[page].frameNum = 0;
            pt->pages[page].dirty_valid_bits = 1;
            pt->firstIn = 0;
            pt->numFaults++;
            pt->numFramesUsed = pt->numFramesUsed + 1;
        }
        else if (pt->numFramesUsed < pt->frame_count) // fills the first frames
        {
            if (pt->pages[page].dirty_valid_bits == 0)
            {
                pt->pages[page].frameNum =  pt->numFramesUsed;
                pt->pages[page].dirty_valid_bits = 1;
                pt->numFaults++;
                pt->numFramesUsed = pt->numFramesUsed + 1;
            }
        }
        else
        {
            
            if (pt->pages[page].dirty_valid_bits == 0) // page isn't loaded
            {
                pt->pages[pt->firstIn].dirty_valid_bits = 0;
                pt->pages[page].frameNum = pt->firstIn;
                pt->pages[page].dirty_valid_bits = 1;
                pt->firstIn = pt->firstIn + 1;
                pt->numFaults++;
            }
        }
    }
    else if (pt->algorithm == 1) // LRU ----------------------------------------
    {
        if (pt->firstIn == -1) // If it is the first number
        {
            pt->pages[page].frameNum = 0;
            pt->pages[page].dirty_valid_bits = 1;
            pt->firstIn = 0;
            pt->lru = 0;
            pt->numFaults++;
            pt->numFramesUsed = pt->numFramesUsed + 1;
        }
        else if (pt->numFramesUsed < pt->frame_count) // fills all the frames
        {
            if (pt->pages[page].dirty_valid_bits == 1)
            {
                pt->lru = pt->lru + 1;
            }
            else if (pt->pages[page].dirty_valid_bits == 0)
            {
                pt->pages[page].frameNum =  pt->numFramesUsed;
                pt->pages[page].dirty_valid_bits = 1;
                pt->numFaults++;
                pt->numFramesUsed = pt->numFramesUsed + 1;
            }
        }
        else
        {
            if (pt->pages[page].dirty_valid_bits == 1) // page is already loaded
            {
                pt->lru = pt->lru + 1;
            }
            else if (pt->pages[page].dirty_valid_bits == 0) // page is not loaded
            {
                pt->pages[pt->lru].dirty_valid_bits = 0;
                pt->pages[page].frameNum = pt->lru;
                pt->pages[page].dirty_valid_bits = 1;
                pt->firstIn = pt->firstIn + 1;
                pt->numFaults++;
            }
        }
    }
    else if (pt->algorithm == 2) // MFU ---------------------------------------
    {
        if (pt->firstIn == -1) // If it is the first number
        {
            pt->pages[page].frameNum = 0;
            pt->pages[page].usedCount =  pt->pages[page].usedCount + 1;
            pt->pages[page].dirty_valid_bits = 1;
            pt->firstIn = 0;
            //pt->lru = 0;
            pt->numFaults++;
            pt->numFramesUsed = pt->numFramesUsed + 1;
        }
        else if (pt->numFramesUsed < pt->frame_count) // fills all the frames
        {
            if (pt->pages[page].dirty_valid_bits == 1)
            {
                //pt->lru = pt->lru + 1;
                pt->pages[page].usedCount =  pt->pages[page].usedCount + 1;
                
            }
            else if (pt->pages[page].dirty_valid_bits == 0)
            {
                pt->pages[page].frameNum =  pt->numFramesUsed;
                pt->pages[page].dirty_valid_bits = 1;
                pt->pages[page].usedCount =  pt->pages[page].usedCount + 1;
                pt->numFaults++;
                pt->numFramesUsed = pt->numFramesUsed + 1;
            }
        }
        else
        {
            if (pt->pages[page].dirty_valid_bits == 1) // page is already loaded
            {
                //pt->lru = pt->lru + 1;
                pt->pages[page].usedCount =  pt->pages[page].usedCount + 1;
            }
            else if (pt->pages[page].dirty_valid_bits == 0) // page is not loaded
            {
                int tempGreatest = 0;
                for (int i=0; i<pt->page_count; i++)
                {
                    if (pt->pages[i].dirty_valid_bits == 1)
                    {
                        if (tempGreatest < pt->pages[i].usedCount)
                        {
                            tempGreatest = pt->pages[i].usedCount;
                            pt->mfu = i;
                        }
                    }
                }
                pt->pages[pt->mfu].dirty_valid_bits = 0;
                pt->pages[pt->mfu].usedCount = pt->pages[page].usedCount + 1;
                pt->pages[page].frameNum = pt->mfu;
                pt->pages[page].dirty_valid_bits = 1;
                pt->firstIn = pt->firstIn + 1;
                pt->numFaults++;
            }
        }
    }
}
char* getMode(enum replacement_algorithm algorithm)
{
    switch (algorithm)
    {
        case FIFO: return "FIFO";
        case LRU: return "LRU";
        case MFU: return "MFU";
    }
}

/**
 * Displays page table replacement algorithm, number of page faults, and the
 * current contents of the page table.
 *
 * @param pt A page table object.
 */
void page_table_display(struct page_table* pt)
{
    printf("----Page Table----\n");
    printf("Mode: %s\n", getMode(pt->algorithm));
    printf("Page Faults: %d\n", pt->numFaults);
    printf("page frame | dirty valid\n");
    for(int i=0; i<pt->page_count; i++)
    {
        printf("   %d     %d |     0     %d\n", i, pt->pages[i].frameNum, pt->pages[i].dirty_valid_bits);
    }
}


/**
 * Displays the current contents of the page table.
 *
 * @param pt A page table object.
 */
void page_table_display_contents(struct page_table *pt)
{
    printf("----Page Table----\n");
    printf("Mode: %s\n", getMode(pt->algorithm));
    printf("Page Faults: %d\n", pt->numFaults);
    printf("page frame | dirty valid\n");
    for(int i=0; i<pt->page_count; i++)
    {
        printf("   %d     %d |     0     %d\n", i, pt->pages[i].frameNum, pt->pages[i].dirty_valid_bits);
    }
}

