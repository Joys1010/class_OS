#include "bitmap.h"
#include <debug.h>
#include <limits.h>
#include <round.h>
#include <stdio.h>
#include "threads/malloc.h"
#ifdef FILESYS
#include "filesys/file.h"
#endif


#define BUDDY_NUM 10
#define GROUP_NUM 32

struct Buddy
{
        uint16_t group[GROUP_NUM];
};

struct Buddy buddy[BUDDY_NUM];

void buddy_init()
{
	/*Function for Buddy System
	initialize buddy[i].group[i]
	and set buddy[9].group[0] = 1;
	*/
        for (size_t i=0; i < BUDDY_NUM ; i++) {
                for (size_t j=0; j< GROUP_NUM ; j++) {
                        buddy[i].group[j] = 0;
                }
        }
        buddy[BUDDY_NUM-1].group[0] = 1;
}

bool buddy_list_empty(struct Buddy buddy)
{
	/*Function for Buddy System
	return true if buddy.group[i] is empty
	return false if buddy.group[i] is not empty
	*/
        
        for (size_t i=0; i<GROUP_NUM; i++) {
                if ( buddy.group[i] != 0 )
                        return false;

        }
        return true;
}

size_t get_buddy_hole (size_t i)
{  
	/*Funciton for Buddy System
	insert the hole's first index to buddy[i] that hole size is 2^i
	select the right index that start from right index 2^i is empty
	*/     
        size_t idx;
        if ( i == BUDDY_NUM) {
                return NULL;
        }
        if ( buddy_list_empty(buddy[i]) ){  
                
                idx = get_buddy_hole(i+1); 
                buddy[i].group[idx / 16] = 1 << (idx%16); 
                idx = idx + (1 << i);
                buddy[i].group[idx/16] += 1<< (idx%16);
        }
        
	// selection
        size_t tmp;
        for (size_t j = 0; j<GROUP_NUM ; j++) {
                
                if ( buddy[i].group[j] != 0) {

			tmp = buddy[i].group[j];

                        for (size_t k = 0 ; k < 16 ; k++) {

				size_t m = tmp % 2;

                                tmp = tmp / 2 ;

                                if (m) {
					
                                        idx = 16*j + k;
					
					buddy[i].group[j] -= 1 << k ; 

                                        return idx;
                                }
                        }
                }
        }

        return BITMAP_ERROR;

}

void merge_func(size_t page_idx, size_t i){
	
	/*Funtion for Buddy System : free
	check if the right of left buddy is available to merge
	if available, call merge_func recursively
	if not return NULL
	*/

	size_t idx_f;
	size_t left;
	size_t right;
	size_t tmp;
	
	if ( i == BUDDY_NUM) {
		
		return NULL;
	}

	buddy[i].group[page_idx / 16] += (1 << (page_idx%16));
	idx_f = page_idx / (1<<i);
	
	if (idx_f % 2) {
				
		left = page_idx - (1<<i);
		tmp = buddy[i].group[left/16];

		if (tmp & (1<<(left%16)) ) {
			
			buddy[i].group[page_idx / 16] -= (1 << (page_idx%16));
			buddy[i].group[left / 16] -= (1 << (left%16));
			
			return merge_func(left, i+1);
		}
	
	}

	else {
		right = page_idx + (1<<i);
		tmp = buddy[i].group[right/16];
		
		if (tmp & (1<<(right%16)) ) {

			buddy[i].group[page_idx / 16] -= (1 << (page_idx%16));
			buddy[i].group[right / 16] -= (1 << (right%16));

			return merge_func(page_idx, i+1);
		}

      }

	return NULL;
}

size_t merge_hole(size_t page_idx, size_t page_cnt){

	/* Funtion for Buddy System : free
	call merge_func to merge two buddies
	*/
        size_t i = 0;
	size_t find_idx = BITMAP_ERROR;

        for (i = 0; i<10 ; i++) {
                if ( page_cnt <= 1 << i)
                        break; 
        }

	merge_func(page_idx, i);

}




/* Element type.

   This must be an unsigned integer type at least as wide as int.

   Each bit represents one bit in the bitmap.
   If bit 0 in an element represents bit K in the bitmap,
   then bit 1 in the element represents bit K+1 in the bitmap,
   and so on. */
typedef unsigned long elem_type;

/* Number of bits in an element. */
#define ELEM_BITS (sizeof (elem_type) * CHAR_BIT)

/* From the outside, a bitmap is an array of bits.  From the
   inside, it's an array of elem_type (defined above) that
   simulates an array of bits. */
struct bitmap
{
	size_t bit_cnt;     /* Number of bits. */
	elem_type *bits;    /* Elements that represent bits. */
};

/* Returns the index of the element that contains the bit
   numbered BIT_IDX. */
	static inline size_t
elem_idx (size_t bit_idx) 
{
	return bit_idx / ELEM_BITS;
}

/* Returns an elem_type where only the bit corresponding to
   BIT_IDX is turned on. */
	static inline elem_type
bit_mask (size_t bit_idx) 
{
	return (elem_type) 1 << (bit_idx % ELEM_BITS);
}

/* Returns the number of elements required for BIT_CNT bits. */
	static inline size_t
elem_cnt (size_t bit_cnt)
{
	return DIV_ROUND_UP (bit_cnt, ELEM_BITS);
}

/* Returns the number of bytes required for BIT_CNT bits. */
	static inline size_t
byte_cnt (size_t bit_cnt)
{
	return sizeof (elem_type) * elem_cnt (bit_cnt);
}

/* Returns a bit mask in which the bits actually used in the last
   element of B's bits are set to 1 and the rest are set to 0. */
	static inline elem_type
last_mask (const struct bitmap *b) 
{
	int last_bits = b->bit_cnt % ELEM_BITS;
	return last_bits ? ((elem_type) 1 << last_bits) - 1 : (elem_type) -1;
}

/* Creation and destruction. */

/* Creates and returns a pointer to a newly allocated bitmap with room for
   BIT_CNT (or more) bits.  Returns a null pointer if memory allocation fails.
   The caller is responsible for freeing the bitmap, with bitmap_destroy(),
   when it is no longer needed. */
	struct bitmap *
bitmap_create (size_t bit_cnt) 
{
	struct bitmap *b = malloc (sizeof *b);
	if (b != NULL)
	{
		b->bit_cnt = bit_cnt;
		b->bits = malloc (byte_cnt (bit_cnt));
		if (b->bits != NULL || bit_cnt == 0)
		{
			bitmap_set_all (b, false);
			return b;
		}
		free (b);
	}
	return NULL;
}

/* Creates and returns a bitmap with BIT_CNT bits in the
   BLOCK_SIZE bytes of storage preallocated at BLOCK.
   BLOCK_SIZE must be at least bitmap_needed_bytes(BIT_CNT). */
	struct bitmap *
bitmap_create_in_buf (size_t bit_cnt, void *block, size_t block_size UNUSED)
{
	struct bitmap *b = block;

	ASSERT (block_size >= bitmap_buf_size (bit_cnt));

	b->bit_cnt = bit_cnt;
	b->bits = (elem_type *) (b + 1);
	bitmap_set_all (b, false);
	return b;
}

/* Returns the number of bytes required to accomodate a bitmap
   with BIT_CNT bits (for use with bitmap_create_in_buf()). */
	size_t
bitmap_buf_size (size_t bit_cnt) 
{
	return sizeof (struct bitmap) + byte_cnt (bit_cnt);
}

/* Destroys bitmap B, freeing its storage.
   Not for use on bitmaps created by bitmap_create_in_buf(). */
	void
bitmap_destroy (struct bitmap *b) 
{
	if (b != NULL) 
	{
		free (b->bits);
		free (b);
	}
}

/* Bitmap size. */

/* Returns the number of bits in B. */
	size_t
bitmap_size (const struct bitmap *b)
{
	return b->bit_cnt;
}

/* Setting and testing single bits. */

/* Atomically sets the bit numbered IDX in B to VALUE. */
	void
bitmap_set (struct bitmap *b, size_t idx, bool value) 
{
	ASSERT (b != NULL);
	ASSERT (idx < b->bit_cnt);
	if (value)
		bitmap_mark (b, idx);
	else
		bitmap_reset (b, idx);
}

/* Atomically sets the bit numbered BIT_IDX in B to true. */
	void
bitmap_mark (struct bitmap *b, size_t bit_idx) 
{
	size_t idx = elem_idx (bit_idx);
	elem_type mask = bit_mask (bit_idx);

	/* This is equivalent to `b->bits[idx] |= mask' except that it
	   is guaranteed to be atomic on a uniprocessor machine.  See
	   the description of the OR instruction in [IA32-v2b]. */
	asm ("orl %1, %0" : "=m" (b->bits[idx]) : "r" (mask) : "cc");
}

/* Atomically sets the bit numbered BIT_IDX in B to false. */
	void
bitmap_reset (struct bitmap *b, size_t bit_idx) 
{
	size_t idx = elem_idx (bit_idx);
	elem_type mask = bit_mask (bit_idx);

	/* This is equivalent to `b->bits[idx] &= ~mask' except that it
	   is guaranteed to be atomic on a uniprocessor machine.  See
	   the description of the AND instruction in [IA32-v2a]. */
	asm ("andl %1, %0" : "=m" (b->bits[idx]) : "r" (~mask) : "cc");
}

/* Atomically toggles the bit numbered IDX in B;
   that is, if it is true, makes it false,
   and if it is false, makes it true. */
	void
bitmap_flip (struct bitmap *b, size_t bit_idx) 
{
	size_t idx = elem_idx (bit_idx);
	elem_type mask = bit_mask (bit_idx);

	/* This is equivalent to `b->bits[idx] ^= mask' except that it
	   is guaranteed to be atomic on a uniprocessor machine.  See
	   the description of the XOR instruction in [IA32-v2b]. */
	asm ("xorl %1, %0" : "=m" (b->bits[idx]) : "r" (mask) : "cc");
}

/* Returns the value of the bit numbered IDX in B. */
	bool
bitmap_test (const struct bitmap *b, size_t idx) 
{
	ASSERT (b != NULL);
	ASSERT (idx < b->bit_cnt);
	return (b->bits[elem_idx (idx)] & bit_mask (idx)) != 0;
}

/* Setting and testing multiple bits. */

/* Sets all bits in B to VALUE. */
	void
bitmap_set_all (struct bitmap *b, bool value) 
{
	ASSERT (b != NULL);

	bitmap_set_multiple (b, 0, bitmap_size (b), value);
}

/* Sets the CNT bits starting at START in B to VALUE. */
	void
bitmap_set_multiple (struct bitmap *b, size_t start, size_t cnt, bool value) 
{
	size_t i;

	ASSERT (b != NULL);
	ASSERT (start <= b->bit_cnt);
	ASSERT (start + cnt <= b->bit_cnt);

	for (i = 0; i < cnt; i++)
		bitmap_set (b, start + i, value);
}

/* Returns the number of bits in B between START and START + CNT,
   exclusive, that are set to VALUE. */
	size_t
bitmap_count (const struct bitmap *b, size_t start, size_t cnt, bool value) 
{
	size_t i, value_cnt;

	ASSERT (b != NULL);
	ASSERT (start <= b->bit_cnt);
	ASSERT (start + cnt <= b->bit_cnt);

	value_cnt = 0;
	for (i = 0; i < cnt; i++)
		if (bitmap_test (b, start + i) == value)
			value_cnt++;
	return value_cnt;
}

	size_t
bitmap_count_bestfit (const struct bitmap *b, size_t start, size_t cnt, bool value) 
{
	
	/*Function for Best Fit
	count the number of continuous value
	return the number
	*/
	size_t i, value_cnt;

	ASSERT (b != NULL);
	ASSERT (start <= b->bit_cnt);
	ASSERT (start + cnt <= b->bit_cnt);
	
	value_cnt = 0;
	for (i = 0; i + start < b->bit_cnt; i++) {
		if (bitmap_test (b, start + i) == value)
			value_cnt++;
		else
			break;
	}
	if (value_cnt >= cnt) {
		return value_cnt;
	}
	else 
		return BITMAP_ERROR;
}

/* Returns true if any bits in B between START and START + CNT,
   exclusive, are set to VALUE, and false otherwise. */
	bool
bitmap_contains (const struct bitmap *b, size_t start, size_t cnt, bool value) 
{
	size_t i;

	ASSERT (b != NULL);
	ASSERT (start <= b->bit_cnt);
	ASSERT (start + cnt <= b->bit_cnt);

	for (i = 0; i < cnt; i++)
		if (bitmap_test (b, start + i) == value)
			return true;
	return false;
}

/* Returns true if any bits in B between START and START + CNT,
   exclusive, are set to true, and false otherwise.*/
	bool
bitmap_any (const struct bitmap *b, size_t start, size_t cnt) 
{
	return bitmap_contains (b, start, cnt, true);
}

/* Returns true if no bits in B between START and START + CNT,
   exclusive, are set to true, and false otherwise.*/
	bool
bitmap_none (const struct bitmap *b, size_t start, size_t cnt) 
{
	return !bitmap_contains (b, start, cnt, true);
}

/* Returns true if every bit in B between START and START + CNT,
   exclusive, is set to true, and false otherwise. */
	bool
bitmap_all (const struct bitmap *b, size_t start, size_t cnt) 
{
	return !bitmap_contains (b, start, cnt, false);
}

/* Finding set or unset bits. */

/* Finds and returns the starting index of the first group of CNT
   consecutive bits in B at or after START that are all set to
   VALUE.
   If there is no such group, returns BITMAP_ERROR. */
	size_t
bitmap_scan (const struct bitmap *b, size_t start, size_t cnt, bool value) 
{
	ASSERT (b != NULL);
	ASSERT (start <= b->bit_cnt);

	if (cnt <= b->bit_cnt) 
	{
		size_t last = b->bit_cnt - cnt;
		size_t i;
		for (i = start; i <= last; i++)
			if (!bitmap_contains (b, i, cnt, !value))
				return i; 
	}
	return BITMAP_ERROR;
}

/* Finds the first group of CNT consecutive bits in B at or after
   START that are all set to VALUE, flips them all to !VALUE,
   and returns the index of the first bit in the group.
   If there is no such group, returns BITMAP_ERROR.
   If CNT is zero, returns 0.
   Bits are set atomically, but testing bits is not atomic with
   setting them. */
	size_t
bitmap_scan_and_flip (struct bitmap *b, size_t start, size_t cnt, bool value)
{
	size_t idx = bitmap_scan (b, start, cnt, value);
	if (idx != BITMAP_ERROR) 
		bitmap_set_multiple (b, idx, cnt, !value);
	return idx;
}

	size_t    
bitmap_scan_and_flip_nextfit (struct bitmap *b, size_t start, size_t cnt, bool value)
{
	/*Function for Next Fit
	Searching for the index start from 'start' 
	find out the first appearing point of continuous value of the desired size of 'cnt'
	flip the cnt number of bit from the index
	*/

	size_t range = 0; 
	size_t i = 0;
	size_t find_idx = 0;


	if (cnt <= b->bit_cnt - start) {
	
		range = b->bit_cnt - cnt ;

		for (i=start; i<=range; i++) { 
			if (!bitmap_contains (b, i, cnt, !value)) {
				find_idx = i;
				break;
			}
		}
				
	}

	else if (cnt <= b->bit_cnt && i == range) {

		range = start;
		
		for (i=0; i<=range; i++) {
			if (!bitmap_contains (b, i, cnt, !value)) {
				find_idx = i;
				break;
			}
	
		}
	}
	else {

		find_idx = BITMAP_ERROR;
	}


	if (find_idx != BITMAP_ERROR) 
		bitmap_set_multiple (b, find_idx, cnt, !value);

	return find_idx;
}

	size_t
bitmap_scan_and_flip_bestfit (struct bitmap *b, size_t start, size_t cnt, bool value)
{
	/*Function for Best Fit
	Searching for index continuous value of the desired size of 'cnt'
	find the index of nearest but bigger than cnt
	flip the cnt number of bit from the index 
	*/
	size_t i = 0;
	size_t j = 0;
	size_t find_idx = BITMAP_ERROR;
	size_t min = 0;
	size_t min_cnt = b->bit_cnt;

	while (i+cnt <= b->bit_cnt) {
	
		min = bitmap_count_bestfit(b,i,cnt,value);
		if (min < min_cnt) {
			min_cnt = min;
			find_idx = i;
		}

		
		else if (min == BITMAP_ERROR) {			
			i++;
			continue;
		}

		i += min;			
	}
	

	if (find_idx != BITMAP_ERROR || min_cnt == b->bit_cnt) {
		if (find_idx == BITMAP_ERROR) {
			find_idx = start;
		}
		bitmap_set_multiple (b, find_idx, cnt, !value);
	}
	return find_idx;
}

size_t
bitmap_scan_and_flip_buddy (struct bitmap *b, size_t cnt, bool value)
{
	/* Funciton for Buddy System
	searching for the index of right hole
	flip the cnt number of bit from the index 
	*/
	size_t i = 0;
	size_t find_idx = BITMAP_ERROR;
	for (i = 0; i<10 ; i++) {
		if ( cnt <= 1 << i) 
			break; 
	} 
	
	find_idx = get_buddy_hole(i);

	if (find_idx != BITMAP_ERROR) {
		bitmap_set_multiple(b, find_idx, cnt, !value);
	}
	return find_idx;
}




/* File input and output. */

#ifdef FILESYS
/* Returns the number of bytes needed to store B in a file. */
	size_t
bitmap_file_size (const struct bitmap *b) 
{
	return byte_cnt (b->bit_cnt);
}

/* Reads B from FILE.  Returns true if successful, false
   otherwise. */
	bool
bitmap_read (struct bitmap *b, struct file *file) 
{
	bool success = true;
	if (b->bit_cnt > 0) 
	{
		off_t size = byte_cnt (b->bit_cnt);
		success = file_read_at (file, b->bits, size, 0) == size;
		b->bits[elem_cnt (b->bit_cnt) - 1] &= last_mask (b);
	}
	return success;
}

/* Writes B to FILE.  Return true if successful, false
   otherwise. */
	bool
bitmap_write (const struct bitmap *b, struct file *file)
{
	off_t size = byte_cnt (b->bit_cnt);
	return file_write_at (file, b->bits, size, 0) == size;
}
#endif /* FILESYS */

/* Debugging. */

/* Dumps the contents of B to the console as hexadecimal. */
	void
bitmap_dump (const struct bitmap *b) 
{
	hex_dump (0, b->bits, byte_cnt (b->bit_cnt), false);
}

/* Dumps the contents of B to the console as binary. */
	void
bitmap_dump2 (const struct bitmap *b)
{
	size_t i, j;

	printf ("========== bitmap dump start ==========\n");
	for (i=0; i<elem_cnt (b->bit_cnt); i++) {
		for (j=0; j<ELEM_BITS; j++) {
			if ((i * ELEM_BITS + j) < b->bit_cnt) {
				printf ("%u", (unsigned int) (b->bits[i] >> j) & 0x1);
			}      
		}
		printf ("\n");
	}
	printf ("========== bitmap dump end ==========\n");
}
