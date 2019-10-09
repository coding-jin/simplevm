#ifndef MY_VM_H_INCLUDED
#define MY_VM_H_INCLUDED
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

//Assume the address space is 32 bits, so the max memory size is 4GB
//Page size is 4KB

//Add any important includes here which you may need

#define PGSIZE 4096

// Maximum size of your memory
//#define MAX_MEMSIZE 4*1024*1024*1024
#define MAX_MEMSIZE 2*1024*1024*1024

#define MEMSIZE 1024*1024*1024
// MAX_MEMSIZE/PGSIZE
#define MAXPAGEDIRSIZE 1024

/*
#define SetBit(Bitmap, k)	(Bitmap[(k>>5)] |= (1<<(k&(~((~0)<<5)))))
#define ClearBit(Bitmap, k)	(Bitmap[(k>>5)] &= ~(1<<(k&(~((~0)<<5)))))
#define GetBit(Bitmap, k)	(Bitmap[(k>>5)] & (1<<(k&(~((~0)<<5)))))
*/


// Represents a page table entry
typedef unsigned int pte_t;

// Represents a page directory entry
typedef unsigned int *pde_t;

//#define TLB_SIZE 

//Structure to represents TLB
struct tlb {

    //Assume your TLB is a direct mapped TLB of TBL_SIZE (entries)
    // You must also define wth TBL_SIZE in this file.
    //Assume each bucket to be 4 bytes
};
struct tlb tlb_store;

//unsigned int init_mem;
char *memstart, *memend;
//unsigned int init_pagedir;
//unsigned int **pagedir;
pde_t *pagedir;
unsigned int offset;
unsigned int totalpage;
int bitmapsize;

unsigned int *pbitmap;
unsigned int *vbitmap;

void set_physical_mem();
void pagedir_init();
void* translate(/*pde_t *pgdir, */void *va);
//int page_map(/*pde_t *pgdir, */void *va, void* pa);
bool page_map(unsigned int vpn);
bool check_in_tlb(void *va);
void put_in_tlb(void *va, void *pa);
void *a_malloc(unsigned int num_bytes);
void a_free(void *va, int size);
void put_value(void *va, void *val, int size);
void get_value(void *va, void *val, int size);
void mat_mult(void *mat1, void *mat2, int size, void *answer);

bool check_avail(int num_pages);
bool check_avail_virtualpages(int num_pages, int *start);
void* get_va(int num_pages);
int getpow(unsigned int num);
unsigned int getpageoffset(void *addr);
unsigned int get_pfn(void *pa);
unsigned int get_physical_pagenumber(void *pa);
unsigned int get_next_avail_pfn();
unsigned int get_ppn(unsigned int pfn);

void set_bitmap(unsigned int *bitmap, int k);//	{bitmap[(k>>5)] |= (1<<(k&(~((~0)<<5))));}
void clear_bitmap(unsigned int *bitmap, int k);//	{bitmap[(k>>5)] &= ~(1<<(k&(~((~0)<<5))));}
unsigned int get_bitmap(unsigned int *bitmap, int k);//	{return bitmap[(k>>5)] & (1<<(k&(~((~0)<<5))));}

#endif
