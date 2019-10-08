#include "my_vm.h"

/*
Function responsible for allocating and setting your physical memory 
*/
void set_physical_mem() {

    //Allocate physical memory using mmap or malloc; this is the total size of your memory you are simulating
	memstart = (char*)calloc(MAX_MEMSIZE, 1);
	if(memstart == NULL) {
		fprintf(stderr, "calloc for physical memory fails!\n");
		exit(1);
	}
	memend = memstart + MAX_MEMSIZE - 1;
	totalpage = MAX_MEMSIZE/PGSIZE;
	int bitmapsize = totalpage/32;
	pbitmap = (unsigned int*)calloc(bitmapsize, sizeof(unsigned int));
	if(pbitmap == NULL) {
		fprintf(stderr, "calloc for pbitmap fails!\n");
		exit(1);
	}
	/*
	vbitmap = (unsigned int*)calloc(bitmapsize, sizeof(unsigned int));
	if(pbitmap == NULL) {
		fprintf(stderr, "calloc for vbitmap fails!\n");
		exit(1);
	}
	*/
    //HINT: Also calculate the number of physical and virtual pages and allocate virtual and physical bitmaps and initialize them
}

/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
void* translate(pde_t *pgdir, void *va) {
    //HINT: Get the Page directory index (1st level) Then get the
    //2nd-level-page table index using the virtual address.  Using the page
    //directory index and page table index get the physical address
	
	unsigned int virtualaddress = (unsigned int)va;
	int offset = getpow(PGSIZE);
	unsigned int vpn = virturaladdress>>offset;

	// pdi is the index of PageDirectoryEntry
	unsigned int pdi = vpn>>(offset-2);
	// pti is the index of PageTableEntry
	unsigned int pti = vpn & (~((~0)<<(offset-2)));

	unsigned int *pagetableaddr = (unsigned int*)*(pgdir + pdi);
	if(pagetableaddr == NULL)	return NULL;
	if(*(pagetableaddr + pti) == NULL)	return NULL;

	unsigned int pfn = (unsigned int)*(pagetableaddr+pti);
	unsigned int physicaladdr = (pfn<<offset) | getpageoffset((void*)virtualaddress, offset);
	return (void*)physicaladdr;
    //If translation not successfull
    //return NULL; 
}


/*
The function takes a page directory address, virtual address, physical address
as an argument, and sets a page table entry. This function will walk the page
directory to see if there is an existing mapping for a virtual address. If the
virtual address is not present, then a new entry will be added
*/
int page_map(pde_t *pgdir, void *va, void *pa) {
    /*HINT: Similar to translate(), find the page directory (1st level)
    and page table (2nd-level) indices. If no mapping exists, set the
    virtual to physical mapping */
	unsigned int physical_pagenumber = get_physical_pagenumber((void*)pa);
	if(physical_pagenumber >= totalpage)	return -1;
	if(get_bitmap(pbitmap, physical_pagenumber) != 0)	return -1;

	unsigned int virtualaddress = (unsigned int)va;
	int offset = getpow(PGSIZE);
	unsigned int vpn = virturaladdress>>offset;
	
	unsigned int pdi = vpn>>(offset-2);
	unsigned int *pagetableptr = *(pgdir + pdi);
	if(*(pgdir + pdi) == NULL) {
		pagetableptr = (unsigned int*)calloc(PGSIZE, 1);
		if(pagetableptr == NULL)	return -1;
		*(pgdir + pdi) = pagetableptr;
	}

	unsigned int pti = vpn & (~((~0)<<(offset-2)));
	if(*(pagetableptr+pti) != 0)	return -1;
	unsigned int pfn = 


}


/*Function that gets the next available page
*/
void *get_next_avail(int num_pages) {
 
    //Use virtual address bitmap to find the next free page
}


/* Function responsible for allocating pages
 and used by the benchmark
*/
void *a_malloc(unsigned int num_bytes) {

    //HINT: If the physical memory is not yet initialized, then allocate and initialize.

   /* HINT: If the page directory is not initialized, then initialize the
   page directory. Next, using get_next_avail(), check if there are free pages. If
   free pages are available, set the bitmaps and map a new page. Note, you will 
   have to mark which physical pages are used. */

    return NULL;
}

/* Responsible for releasing one or more memory pages using virtual address (va)
*/
void a_free(void *va, int size) {

    //Free the page table entries starting from this virtual address (va)
    // Also mark the pages free in the bitmap
    //Only free if the memory from "va" to va+size is valid
}


/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
*/
void put_value(void *va, void *val, int size) {

    /* HINT: Using the virtual address and translate(), find the physical page. Copy
       the contents of "val" to a physical page. NOTE: The "size" value can be larger
       than one page. Therefore, you may have to find multiple pages using translate()
       function.*/

}


/*Given a virtual address, this function copies the contents of the page to val*/
void get_value(void *va, void *val, int size) {

    /* HINT: put the values pointed to by "va" inside the physical memory at given
    "val" address. Assume you can access "val" directly by derefencing them.
    If you are implementing TLB,  always check first the presence of translation
    in TLB before proceeding forward */


}



/*
This function receives two matrices mat1 and mat2 as an argument with size
argument representing the number of rows and columns. After performing matrix
multiplication, copy the result to answer.
*/
void mat_mult(void *mat1, void *mat2, int size, void *answer) {

    /* Hint: You will index as [i * size + j] where  "i, j" are the indices of the
    matrix accessed. Similar to the code in test.c, you will use get_value() to
    load each element and perform multiplication. Take a look at test.c! In addition to 
    getting the values from two matrices, you will perform multiplication and 
    store the result to the "answer array"*/

       
}

int getpow(unsigned int num) {
	int pow = 0;
	while(!(num&1)) {
		++pow;
		num >>= 1;
	}
	return pow;
}

unsigned int getpageoffset(void *addr, int offset) {
	unsigned int address = (unsigned int)addr;
	return address&(~((~0)<<offset));
}

unsigned int get_physical_pagenumber(void *physicaladdr) {
	unsigned int physicaladdress = (unsigned int)physicaladdr;
	return (physicaladdress-(unsigned int)memstart)/PGSIZE;
}


