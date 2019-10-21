#include "my_vm.h"

bool _init_physical = false;
pde_t *_pagedir = NULL;
uint32_t _pagenum = 0;
/*
Function responsible for allocating and setting your physical memory 
*/
void set_physical_mem() {

    //Allocate physical memory using mmap or malloc; this is the total size of
    //your memory you are simulating
	//memstart = calloc(MAX_MEMSIZE, 1);
	/*
	memstart = memalign(PGSIZE, MAX_MEMSIZE);
	if(memstart == NULL) {
		fprintf(stderr, "calloc for memstart fails!\n");
		exit(1);
	}*/
	if(posix_memalign(&memstart, PGSIZE, MAX_MEMSIZE) != 0) {
		fprintf(stderr, "posix_memalign error!\n");
		exit(1);
	}

	_pagenum = MAX_MEMSIZE/PGSIZE;
	bitmapsize = _pagenum/8;
    //HINT: Also calculate the number of physical and virtual pages and allocate
    //virtual and physical bitmaps and initialize them
	pbitmap = (uint32_t*)calloc(bitmapsize, 1);
	if(pbitmap == NULL) {
		fprintf(stderr, "calloc for pbitmap fails!\n");
		exit(1);
	}
	vbitmap = (uint32_t*)calloc(bitmapsize, 1);
	if(vbitmap == NULL) {
		fprintf(stderr, "calloc for vbitmap fails!\n");
		exit(1);
	}

	_pagedir = (pde_t*)calloc(1024, sizeof(pde_t));
	if(_pagedir == NULL) {
		fprintf(stderr, "calloc for _pagedir fails!\n");
		exit(1);
	}

	_init_physical = true;
}

/*
The function takes a virtual address and page directories starting address and
performs translation to return the physical address
*/
//pte_t * translate(pde_t *pgdir, void *va) {
address_t translate(pde_t *pgdir, address_t va) {
    //HINT: Get the Page directory index (1st level) Then get the
    //2nd-level-page table index using the virtual address.  Using the page
    //directory index and page table index get the physical address
	if(pgdir == NULL)	return 0;
	uint32_t pdindex = get_pdindex(va);
	uint32_t ptindex = get_ptindex(va);
	uint32_t offset = get_pageoffset(va);
	pte_t *pagetable = pgdir[pdindex];
	if(pagetable == 0)	return 0;
	uint32_t pagesizebits = get_pow2(PGSIZE);
	return (pagetable[ptindex]<<pagesizebits) | offset;
}

/*
The function takes a page directory address, virtual address, physical address
as an argument, and sets a page table entry. This function will walk the page
directory to see if there is an existing mapping for a virtual address. If the
virtual address is not present, then a new entry will be added
*/
//bool page_map(pde_t *pgdir, void *va, void *pa) {
bool page_map(pde_t *pagedir, pageno_t vpn, pageno_t pfn) {

    /*HINT: Similar to translate(), find the page directory (1st level)
    and page table (2nd-level) indices. If no mapping exists, set the
    virtual to physical mapping */
	if(pagedir == NULL)	return false;
	uint32_t pdindex = vpn>>FIRSTLEVELBITS;
	uint32_t ptindex = vpn & ~((~0)<<FIRSTLEVELBITS);
	if(pagedir[pdindex] == NULL) {
		pagedir[pdindex] = (pte_t*)calloc(1024, sizeof(pte_t));
		if(pagedir[pdindex] == NULL) {
			fprintf(stderr, "calloc for pgdir[%"PRIu32"]\n", pdindex);
			exit(1);
		}
	}
	pte_t *pagetable = pagedir[pdindex];
	if(pagetable[ptindex] != NULL)	return false;
	pagetable[ptindex] = pfn;
    return true;
}

/*Function that gets the next available page
*/
void *get_next_avail(uint32_t num_pages) {
	if(num_pages==0)	return NULL;
	uint32_t counter, i, j;
	for(i=0;i<_pagenum;++i) {
		if(get_bitmap(vbitmap, i)==0) {
			counter = 1;
			if(counter == num_pages)	break;
			for(j=i+1;j<_pagenum;++j) {
				if(get_bitmap(vbitmap, j)==0)	++counter;
				else	break;
				if(counter==num_pages)	break;
			}
			if(counter==num_pages)	break;
		}
	}
	if(counter!=num_pages)	return NULL;
	// i corresponds to the 1st virtual page, and j to the last
	pageno_t vpn, ppn=0, pfn;
	for(vpn=i;vpn<i+num_pages;++vpn) {
		while(get_bitmap(pbitmap, ppn))	++ppn;
		set_bitmap(vbitmap, vpn);
		set_bitmap(pbitmap, ppn);
		pfn = transfer_ppntopfn(ppn);
		if(page_map(_pagedir, vpn, pfn) == false) {
			fprintf(stderr, "page_mmap for vpn=%"PRIu32" pfn=%"PRIu32" fails!\n", vpn, pfn);
			exit(1);
		}
	}
	
	return i<<get_pow2(PGSIZE);
}


/* Function responsible for allocating pages
 and used by the benchmark
*/
void *a_malloc(unsigned int num_bytes) {

    //HINT: If the physical memory is not yet initialized, then allocate and initialize.
	if(_init_physical == false)	set_physical_mem();

	/* HINT: If the page directory is not initialized, then initialize the
	page directory. Next, using get_next_avail(), check if there are free pages. If
	free pages are available, set the bitmaps and map a new page. Note, you will 
	have to mark which physical pages are used. */
	if(num_bytes == 0)	return NULL;
	uint32_t pagesize_bits = get_pow2(PGSIZE);
	uint32_t num_pages = num_bytes>>pagesize_bits;
	if(num_bytes&~((~0)<<pagesize_bits))	++num_pages;
	return get_next_avail(num_pages);
}

/* Responsible for releasing one or more memory pages using virtual address (va)
*/
void a_free(void *va, int size) {
    //Free the page table entries starting from this virtual address (va)
    // Also mark the pages free in the bitmap
    //Only free if the memory from "va" to va+size is valid
	if(size <=0 )	return;
	uint32_t pagesize_bits = get_pow2(PGSIZE);
	uint32_t num_pages = size>>pagesize_bits;
	if(size&~((~0)<<pagesize_bits))	++num_pages;
	pageno_t vpn = (pageno_t)va >> pagesize_bits;

	bool free_flag = true;
	for(pageno_t i=vpn;i<vpn+num_pages;++i)
		if(get_bitmap(vbitmap, i) == 0)	{
			free_flag = false;
			break;
		}

	if(free_flag) {
		uint32_t pdindex, ptindex;
		pte_t *pagetable;
		pageno_t pfn, ppn;
		for(pageno_t vpn_i=vpn;vpn_i<vpn+num_pages;++vpn_i) {
			pdindex = vpn_i>>FIRSTLEVELBITS;
			ptindex = vpn_i & ~((~0)<<FIRSTLEVELBITS);
			pagetable = _pagedir[pdindex];
			pfn = pagetable[ptindex];
			ppn = transfer_pfntoppn(pfn);

			// free
			pagetable[ptindex] = 0;
			bool pagetable_free = true;
			for(int i=0;i<1024;++i) {
				if(pagetable[i]) {
					pagetable_free = false;
					break;
				}
			}
			if(pagetable_free) {
				printf("free pagetable!\n");
				free(_pagedir[pdindex]);
				_pagedir[pdindex] = NULL;
			}
			clear_bitmap(pbitmap, ppn);
			clear_bitmap(vbitmap, vpn_i);
			/*
			if(get_bitmap(pbitmap, ppn))	printf("ppn=%d clear failed!\n", ppn);
			else	printf("ppn=%d clear succeed!\n", ppn);
			if(get_bitmap(vbitmap, vpn_i))	printf("vpn_i=%d clear failed!\n", vpn_i);
			else	printf("vpn_i=%d clear succeed!\n", vpn_i);
			*/
		}
	}

}

/* The function copies data pointed by "val" to physical
 * memory pages using virtual address (va)
*/
void put_value(void *va, void *val, int size) {

    /* HINT: Using the virtual address and translate(), find the physical page. Copy
       the contents of "val" to a physical page. NOTE: The "size" value can be larger
       than one page. Therefore, you may have to find multiple pages using translate()
       function.*/
	if(size<=0 || val==NULL)	return;
	uint32_t pagesizebits = get_pow2(PGSIZE);
	pageno_t vpn_start = (address_t)va >> pagesizebits;
	pageno_t vpn_end = ((address_t)va + size-1) >> pagesizebits;
	address_t pa;

	// check the validation first!
	for(pageno_t vpn=vpn_start;vpn<=vpn_end;++vpn)	if(get_bitmap(vbitmap, vpn)==0)	return;

	if(vpn_start == vpn_end) {
		pa = translate(_pagedir, (address_t)va);
		memcpy((void*)pa, val, size);
		//printf("putin value=%d\n", *(int*)pa);
	}else {
		uint32_t remain = ((vpn_start+1)<<pagesizebits) - (address_t)va;
		pa = translate(_pagedir, (address_t)va);
		memcpy((void*)pa, val, remain);
		val = (void*)((address_t)val + remain);
		size -= remain;
		address_t va_tmp;
		for(pageno_t vpn_mid=vpn_start+1;vpn_mid<vpn_end;++vpn_mid) {
			va_tmp = vpn_mid << pagesizebits;
			pa = translate(_pagedir, va_tmp);
			memcpy((void*)pa, val, PGSIZE);
			size -= PGSIZE;
			val = (void*)((address_t)val + PGSIZE);
		}

		pa = translate(_pagedir, (address_t)(vpn_end<<pagesizebits));
		memcpy((void*)pa, val, size);
	}
}

/*Given a virtual address, this function copies the contents of the page to val*/
void get_value(void *va, void *val, int size) {

    /* HINT: put the values pointed to by "va" inside the physical memory at given
    "val" address. Assume you can access "val" directly by derefencing them.
    If you are implementing TLB,  always check first the presence of translation
    in TLB before proceeding forward */
	if(size<=0 || val==NULL)	return;
	uint32_t pagesizebits = get_pow2(PGSIZE);
	pageno_t vpn_start = (address_t)va >> pagesizebits;
	pageno_t vpn_end = ((address_t)va+size-1) >> pagesizebits;
	address_t pa;
	if(vpn_start == vpn_end) {
		pa = translate(_pagedir, (address_t)va);
		memcpy(val, (void*)pa, size);
	}else {
		uint32_t remain = ((vpn_start+1)<<pagesizebits) - (address_t)va;
		pa = translate(_pagedir, (address_t)va);
		memcpy(val, (void*)pa, remain);
		val = (void*)((address_t)val + remain);
		size -= remain;
		address_t va_tmp;
		for(pageno_t vpn_mid=vpn_start+1;vpn_mid<vpn_end;++vpn_mid) {
			va_tmp = vpn_mid << pagesizebits;
			pa = translate(_pagedir, va_tmp);
			memcpy(val, (void*)pa, PGSIZE);
			size -= PGSIZE;
			val = (void*)((address_t)val + PGSIZE);
		}

		pa = translate(_pagedir, (address_t)(vpn_end<<pagesizebits));
		memcpy(val, (void*)pa, size);
	}
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
	address_t address_m1, address_m2, address_ans;
	int tmp, tmp1, tmp2;
	for(int i=0;i<size;++i)
		for(int j=0;j<size;++j) {
			tmp = 0;
			for(int k=0;k<size;++k) {
				address_m1 = (address_t)mat1 + (i*size+k)*sizeof(int);
				address_m2 = (address_t)mat2 + (k*size+j)*sizeof(int);
				get_value((void*)address_m1, &tmp1, sizeof(int));
				get_value((void*)address_m2, &tmp2, sizeof(int));
				if(tmp2 != 2) {
					printf("i=%d j=%d k=%d tmp1=%d tmp2=%d\n", i, j, k, tmp1, tmp2);
				}
				tmp += tmp1*tmp2;
			}
			address_ans = (address_t)answer + (i*size+j)*sizeof(int);
			put_value((void*)address_ans, &tmp, sizeof(int));
		}

}

void set_bitmap(uint32_t *bitmap, uint32_t k) {
	bitmap[k>>5] |= (1<<(k&(~((~0)<<5))));
}

void clear_bitmap(uint32_t *bitmap, uint32_t k) {
	bitmap[k>>5] &= ~(1<<(k&(~((~0)<<5))));
}

bool get_bitmap(uint32_t *bitmap, uint32_t k) {
	if((bitmap[k>>5]>>(k&~((~0)<<5))) & 1)	return true;
	else	return false;
}

uint32_t get_pageoffset(address_t va) {
	return va & ~((~0)<<PGSIZE);
}

uint32_t get_ptindex(address_t va) {
	return ((va>>get_pow2(PGSIZE)) & ~((~0)<<FIRSTLEVELBITS));
}

uint32_t get_pdindex(address_t va) {
	return va>>(get_pow2(PGSIZE)+FIRSTLEVELBITS);
}

uint32_t get_pow2(uint32_t number) {
	uint32_t counter = 0;
	number >>= 1;
	while(number) {
		++counter;
		number >>= 1;
	}
	return counter;
}

pageno_t get_next_avail_pfn() {}

pageno_t transfer_ppntopfn(pageno_t ppn) {
	return (((address_t)memstart)>>get_pow2(PGSIZE)) + ppn;
}

pageno_t transfer_pfntoppn(pageno_t pfn) {
	return pfn - ((address_t)memstart >> get_pow2(PGSIZE));
}


