#ifndef MY_VM_H_INCLUDED
#define MY_VM_H_INCLUDED
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
//Assume the address space is 48 bits, so the max memory size is 256*1024GB
//Page size is 4KB

//Add any important includes here which you may need

#define PGSIZE 4096
#define TLBSIZE 32

// Maximum size of your memory
//#define MAX_MEMSIZE (uint64_t)1024*(uint64_t)1024*(uint64_t)1024
//1024*1024*1024=1073741824    1024*1024*1024*1024=1099511627776
#define MAX_MEMSIZE 8589934592

#define LEVELBITS 9

pthread_mutex_t _lock;

typedef uint64_t address_t;

// address format: pgd(9) pud(9) pmd(9) pte(9) offset(12)

typedef uint64_t pgd_t;
typedef uint64_t pud_t;
typedef uint64_t pmd_t;
typedef uint64_t pte_t;

// Represents a page table entry
//typedef unsigned long pte_t;
//typedef uint64_t pte_t;

// Represents a page directory entry
//typedef unsigned long pde_t;
//typedef uint64_t pde_t;

typedef uint64_t pageno_t;

//#define TLB_SIZE 

//Structure to represents TLB
typedef struct tlb{
    //Assume your TLB is a direct mapped TLB of TBL_SIZE (entries). You must also define wth TBL_SIZE in this file.
    //Assume each bucket to be 4 bytes
	bool valid;
	pageno_t key;
	pageno_t value;
	uint64_t timecounter;
}tlb;
tlb _tlb_store[TLBSIZE];
uint64_t _timecounter;

char *memstart;
//pde_t *_pagedir;
uint32_t *pbitmap;
uint32_t *vbitmap;
uint64_t _pagenum;
uint32_t _offsetbits;
uint32_t _tablesize;

void set_bitmap(uint32_t *bitmap, uint64_t k);
void clear_bitmap(uint32_t *bitmap, uint64_t k);
bool get_bitmap(uint32_t *bitmap, uint64_t k);

uint32_t get_pgdindex(address_t va);
uint32_t get_pudindex(address_t va);
uint32_t get_pmdindex(address_t va);
uint32_t get_pteindex(address_t va);

uint32_t get_pageoffset(address_t va);
uint32_t get_pow2(uint64_t number);
pageno_t get_next_avail_pfn();
pageno_t transfer_ppntopfn(pageno_t ppn);
pageno_t transfer_pfntoppn(pageno_t pfn);

void set_physical_mem();
address_t translate(address_t va);
void* get_next_avail(uint64_t num_pages);
bool page_map(pageno_t vpn, pageno_t pfn);
void tlb_add(pageno_t vpn, pageno_t pfn);
void *a_malloc(uint64_t num_bytes);
void a_free(void *va, uint64_t size);
void put_value(void *va, void *val, int size);
void get_value(void *va, void *val, int size);
void mat_mult(void *mat1, void *mat2, int size, void *answer);

uint64_t tlb_lookup(pageno_t vpn);
void tlb_freeupdate(pageno_t vpn);

#endif
