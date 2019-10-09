#include "my_vm.h"

unsigned int init_mem = 0;
unsigned int init_pagedir = 0;

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
	offset = getpow(PGSIZE);
	int bitmapsize = totalpage/32;
	pbitmap = (unsigned int*)calloc(bitmapsize, sizeof(unsigned int));
	if(pbitmap == NULL) {
		fprintf(stderr, "calloc for pbitmap fails!\n");
		exit(1);
	}
	vbitmap = (unsigned int*)calloc(bitmapsize, sizeof(unsigned int));
	if(pbitmap == NULL) {
		fprintf(stderr, "calloc for vbitmap fails!\n");
		exit(1);
	}
	init_mem = 1;
    //HINT: Also calculate the number of physical and virtual pages and allocate virtual and physical bitmaps and initialize them
}

/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
void* translate(/*pde_t *pgdir, */void *va) {
    //HINT: Get the Page directory index (1st level) Then get the
    //2nd-level-page table index using the virtual address.  Using the page
    //directory index and page table index get the physical address
	unsigned int virturaladdress = (unsigned int)va;
	unsigned int vpn = virturaladdress>>offset;
	if(get_bitmap(vbitmap, vpn) == 0)	return NULL;

	// pdi is the index of PageDirectoryEntry
	unsigned int pdi = vpn>>(offset-2);
	// pti is the index of PageTableEntry
	unsigned int pti = vpn & (~((~0)<<(offset-2)));

	//unsigned int *pagetableptr = (unsigned int*)*(pgdir + pdi);
	unsigned int *pagetableptr = (unsigned int*)*(pagedir + pdi);
	if(pagetableptr == NULL)	return NULL;
	if(*(pagetableptr + pti) == 0)	return NULL;

	unsigned int pfn = (unsigned int)*(pagetableptr+pti);
	unsigned int physicaladdr = (pfn<<offset) | getpageoffset((void*)virturaladdress);
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
/*
int page_map(void *va, void *pa) {
	unsigned int physical_pagenumber = get_physical_pagenumber(pa);
	if(physical_pagenumber >= totalpage)	return -1;
	if(get_bitmap(pbitmap, physical_pagenumber) != 0)	return -1;

	unsigned int virturaladdress = (unsigned int)va;
	unsigned int vpn = virturaladdress>>offset;

	// if this virtual address already has a mapping, we cannot add a new entry
	if(get_bitmap(vbitmap, vpn) !=0 )	return -1;
	// get the pagedir index pdi
	unsigned int pdi = vpn>>(offset-2);
	//unsigned int *pagetableptr = *(pgdir + pdi);
	unsigned int *pagetableptr = *(pagedir + pdi);
	if(pagetableptr == NULL) {
		pagetableptr = (unsigned int*)calloc(PGSIZE, 1);
		if(pagetableptr == NULL)	return -1;
		//*(pgdir + pdi) = pagetableptr;
		*(pagedir + pdi) = pagetableptr;
	}
	// get the pagetable index pti
	unsigned int pti = vpn & (~((~0)<<(offset-2)));
	if(*(pagetableptr+pti) != 0)	return -1;

	unsigned int pfn = get_pfn(pa);
	*(pagetableptr+pti) = pfn;
	return 1;
}
*/

bool page_map(unsigned int vpn) {
	unsigned int pfn = get_next_avail_pfn();
	if(pfn==0)	return false;
	unsigned int pdi = vpn>>(offset-2);
	unsigned int pti = vpn & ~((~0)<<(offset-2));
	if(pagedir==NULL)	return false;
	unsigned int *pagedir_entry = pagedir[pdi];
	if(pagedir_entry == NULL) {
		pagedir_entry = (unsigned int*)calloc(PGSIZE, 1);
		pagedir[pdi] = pagedir_entry; // *(pagedir+pdi) = pagedir_entry;
	}
	if(pagedir_entry[pti]!=0)	return false;
	pagedir_entry[pti] = pfn; // *(pagedir+pti) = pfn;
	set_bitmap(vbitmap,vpn);
	set_bitmap(pbitmap,get_ppn(pfn));
	return true;
}

/*Function that gets the next available page
*/
void *get_next_avail(int num_pages) {
	if(num_pages<=0)	return NULL;
	if(check_avail(num_pages))	return get_va(num_pages);
	else	return NULL;
    //Use virtual address bitmap to find the next free page
}

/* Function responsible for allocating pages
 and used by the benchmark
*/
void *a_malloc(unsigned int num_bytes) {

    //HINT: If the physical memory is not yet initialized, then allocate and initialize.
	if(init_mem == 0)	set_physical_mem();
	/* HINT: If the page directory is not initialized, then initialize the page directory. Next, using get_next_avail(), check if there are free pages. If
	free pages are available, set the bitmaps and map a new page. Note, you will have to mark which physical pages are used.
	*/ 
	if(init_pagedir == 0)	pagedir_init();

	int num_pages;
	if(num_bytes>0 && num_bytes<=PGSIZE)	num_pages = 1;
	else if(num_bytes/PGSIZE>=1 && num_bytes%PGSIZE!=0)	num_pages = 1+num_bytes/PGSIZE;
	else if(num_bytes/PGSIZE>1 && num_bytes%PGSIZE==0)	num_pages = num_bytes/PGSIZE;
	else/*(num_bytes==0)*/	return NULL;

	if(num_pages > totalpage)	return NULL;
	return get_next_avail(num_pages);

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

unsigned int getpageoffset(void *addr) {
	unsigned int address = (unsigned int)addr;
	return address&(~((~0)<<offset));
}

unsigned int get_physical_pagenumber(void *pa) {
	return ((unsigned int)pa -(unsigned int)memstart)/PGSIZE;
}

unsigned int get_pfn(void *pa) {
	return ((unsigned int)pa)>>offset;
}

void pagedir_init() {
	pagedir = (unsigned int**)calloc(PGSIZE, sizeof(unsigned int*));
	if(pagedir == NULL)	exit(1);
	init_pagedir = 1;

}

bool check_avail(int num_pages) {
	if(num_pages <= 0)	return false;
	int counter = 0;
	for(int i=0;i<totalpage;++i) {
		if(get_bitmap(pbitmap,i) == 0) {
			++counter;
			if(counter==num_pages)	return true;
		}
	}
	return false;
}

bool check_avail_virtualpages(int num_pages, int *start) {
	int counter=0, begin=-1;
	for(int i=0;i<totalpage;++i) {
		if(get_bitmap(vbitmap, i)) {
			counter = 0;
			begin = -1;
		}else {
			++counter;
			if(counter == 1)	begin=i;
			if(counter==num_pages) {
				*start = begin;
				return true;
			}
		}
	}
	return false;
}

void* get_va(int num_pages) {
	int start=-1;
	if(!check_avail_virtualpages(num_pages, &start) || start<0)	return NULL;
	// allocate 
	int end = start + num_pages - 1;
	// allocate the virtual pages one by one
	for(unsigned int vpn=start;vpn<=end;++vpn)	if(page_map(vpn)==false)	exit(1);
	return (void*)(start<<offset);
}

unsigned int get_next_avail_pfn() {
	for(int i=0;i<totalpage;++i) {
		if(get_bitmap(pbitmap,i)==0)	return ((unsigned int)memstart + i*PGSIZE)>>offset;
	}
	return 0;
}

unsigned int get_ppn(unsigned int pfn) {
	return ((pfn<<offset)-(unsigned int)memstart)/PGSIZE;
}

void set_bitmap(unsigned int *bitmap, int k)	{bitmap[(k>>5)] |= (1<<(k&(~((~0)<<5))));}
void clear_bitmap(unsigned int *bitmap, int k)	{bitmap[(k>>5)] &= ~(1<<(k&(~((~0)<<5))));}
unsigned int get_bitmap(unsigned int *bitmap, int k)	{return bitmap[(k>>5)] & (1<<(k&(~((~0)<<5))));}

