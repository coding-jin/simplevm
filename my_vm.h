#ifndef MY_VM_H_INCLUDED
#define MY_VM_H_INCLUDED
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
//Assume the address space is 32 bits, so the max memory size is 4GB
//Page size is 4KB

//Add any important includes here which you may need

#define PGSIZE 4096

// Maximum size of your memory
#define MAX_MEMSIZE 2*1024*1024*1024

#define MEMSIZE 1024*1024*1024

#define FIRSTLEVELBITS 10

typedef uint32_t address_t;


// Represents a page table entry
//typedef unsigned long pte_t;
typedef uint32_t pte_t;

// Represents a page directory entry
//typedef unsigned long pde_t;
typedef uint32_t pde_t;

typedef uint32_t pageno_t;

//#define TLB_SIZE 

//Structure to represents TLB
struct tlb {

    //Assume your TLB is a direct mapped TLB of TBL_SIZE (entries)
    // You must also define wth TBL_SIZE in this file.
    //Assume each bucket to be 4 bytes
};
struct tlb tlb_store;

//bool _init_physical;
char *memstart;
//pde_t *_pagedir;
uint32_t *pbitmap;
uint32_t *vbitmap;
//uint32_t _pagenum;
uint32_t bitmapsize;

void set_bitmap(uint32_t *bitmap, uint32_t k);
void clear_bitmap(uint32_t *bitmap, uint32_t k);
bool get_bitmap(uint32_t *bitmap, uint32_t k);

uint32_t get_pdindex(address_t va);
uint32_t get_ptindex(address_t va);
uint32_t get_pageoffset(address_t va);
uint32_t get_pow2(uint32_t number);
pageno_t get_next_avail_pfn();
pageno_t transfer_ppntopfn(pageno_t ppn);
pageno_t transfer_pfntoppn(pageno_t pfn);

void set_physical_mem();
pte_t translate(pde_t *pgdir, address_t va);
bool page_map(pde_t *pgdir, pageno_t vpn, pageno_t pfn);
bool check_in_tlb(void *va);
void put_in_tlb(void *va, void *pa);
void *a_malloc(unsigned int num_bytes);
void a_free(void *va, int size);
void put_value(void *va, void *val, int size);
void get_value(void *va, void *val, int size);
void mat_mult(void *mat1, void *mat2, int size, void *answer);

#endif
