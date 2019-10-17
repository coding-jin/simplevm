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

// Represents a page table entry
//typedef unsigned long pte_t;
typedef uint32_t pte_t;

// Represents a page directory entry
//typedef unsigned long pde_t;
typedef uint32_t pde_t;

//#define TLB_SIZE 

//Structure to represents TLB
struct tlb {

    //Assume your TLB is a direct mapped TLB of TBL_SIZE (entries)
    // You must also define wth TBL_SIZE in this file.
    //Assume each bucket to be 4 bytes
};
struct tlb tlb_store;

bool init_physical;
void *memstart;
uint32_t *pbitmap;
uint32_t *vbitmap;
uint32_t pagenum;
uint32_t bitmapsize;

void set_bitmap(uint32_t *bitmap, uint32_t k);
void clear_bitmap(uint32_t *bitmap, uint32_t k);
bool get_bitmap(uint32_t *bitmap, uint32_t k);

void set_physical_mem();
pte_t* translate(pde_t *pgdir, void *va);
int page_map(pde_t *pgdir, void *va, void* pa);
bool check_in_tlb(void *va);
void put_in_tlb(void *va, void *pa);
void *a_malloc(unsigned int num_bytes);
void a_free(void *va, int size);
void put_value(void *va, void *val, int size);
void get_value(void *va, void *val, int size);
void mat_mult(void *mat1, void *mat2, int size, void *answer);

#endif
