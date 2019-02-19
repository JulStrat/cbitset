#ifndef BITSET_H
#define BITSET_H

//#define SOFT_POP_COUNT
//#define SOFT_CTZ
//#define SOFT_CLZ

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "portability.h"

#define GROW_FACTOR 2

// p size_t
#define BIT_SLOT(p) ((p) >> 6)
//#define BIT_OFFSET(p) ((p) & 127)
#define BIT_OFFSET(p) ((p) % 64)
#define BIT_MASK(p) (((uint64_t) 1) << BIT_OFFSET(p))

// Soft population count
static inline size_t pop_count(uint64_t v) {
    size_t c;
    for (c = 0; v; c++) {
        v &= v - 1;
    }
    return c;
}
// Soft trailing zero bits count
static inline size_t ctz(uint64_t v) {
    size_t c = 0;
    if (v == 0)
        return 64;

    if ((v & 0x00000000FFFFFFFF) == 0) {
        c += 32;
        v >>= 32;
    }
    if ((v & 0x000000000000FFFF) == 0) {
        c += 16;
        v >>= 16;
    }
    if ((v & 0x00000000000000FF) == 0) {
        c += 8;
        v >>= 8;
    }
    if ((v & 0x000000000000000F) == 0) {
        c += 4;
        v >>= 4;
    }
    if ((v & 0x0000000000000003) == 0) {
        c += 2;
        v >>= 2;
    }
    if ((v & 0x0000000000000001) == 0)
        c += 1;
    return c;
}
// Soft leading zero bits count
static inline size_t clz(uint64_t v) {
    size_t c = 0;

    if ((v & 0xFFFFFFFF00000000) == 0) {
        c += 32;
        v <<= 32;
    }
    if ((v & 0xFFFF000000000000) == 0) {
        c += 16;
        v <<= 16;
    }
    if ((v & 0xFF00000000000000) == 0) {
        c += 8;
        v <<= 8;
    }
    if ((v & 0xF000000000000000) == 0) {
        c += 4;
        v <<= 4;
    }
    if ((v & 0xC000000000000000) == 0) {
        c += 2;
        v <<= 2;
    }
    if ((v & 0x8000000000000000) == 0)
        c += 1;
    return c;
}

struct bitset_s {
    uint64_t * restrict array;
    size_t arraysize;
    size_t capacity;
};

typedef struct bitset_s bitset_t;

/* Create a new bitset. Return NULL in case of failure. */
bitset_t *bitset_create(void);

/* Create a new bitset able to contain size bits. Return NULL in case of failure. */
bitset_t *bitset_create_with_capacity(size_t size);

/* Free memory. */
void bitset_free(bitset_t *bitset);

/* Set all bits to zero. */
void bitset_clear(bitset_t *bitset);

/* Set all bits to one. */
void bitset_set_all(bitset_t *bitset);

/* Create a copy */
bitset_t * bitset_copy(const bitset_t *bitset);

/* Resize the bitset. Return true in case of success, false for failure. Pad with zeroes new buffer areas if requested. */
bool bitset_resize(bitset_t *bitset, size_t newarraysize, bool padwithzeroes);

/* returns how many bytes of memory the backend buffer uses */
static inline size_t bitset_size_in_bytes(const bitset_t *bitset) {
    return bitset->arraysize * sizeof (uint64_t);
}

/* returns how many bits can be accessed */
static inline size_t bitset_size_in_bits(const bitset_t *bitset) {
    return bitset->arraysize * 64;
}

/* returns how many words (64-bit) of memory the backend buffer uses */
static inline size_t bitset_size_in_words(const bitset_t *bitset) {
    return bitset->arraysize;
}

/* Grow the bitset so that it can support newarraysize * 64 bits with padding. Return true in case of success, false for failure. */
static inline bool bitset_grow(bitset_t *bitset, size_t newarraysize) {
    if (bitset->capacity < newarraysize) {
        uint64_t *newarray;
        bitset->capacity = newarraysize * GROW_FACTOR;
        if ((newarray = (uint64_t *) realloc(bitset->array, sizeof (uint64_t) * bitset->capacity)) == NULL) {
            free(bitset->array);
            return false;
        }
        bitset->array = newarray;
    }
    memset(bitset->array + bitset->arraysize, 0, sizeof (uint64_t) * (newarraysize - bitset->arraysize));
    bitset->arraysize = newarraysize;
    return true; // success!
}

/* attempts to recover unused memory, return false in case of reallocation failure */
bool bitset_trim(bitset_t *bitset);

/* shifts all bits by 's' positions so that the bitset representing values 1,2,10 would represent values 1+s, 2+s, 10+s */
void bitset_shift_left(bitset_t *bitset, size_t s);

/* shifts all bits by 's' positions so that the bitset representing values 1,2,10 would represent values 1-s, 2-s, 10-s, negative values are deleted */
void bitset_shift_right(bitset_t *bitset, size_t s);

/* Set the ith bit. Attempts to resize the bitset if needed (may silently fail) */
static inline void bitset_set(bitset_t *bitset, size_t i) {
    size_t slot = BIT_SLOT(i);
    if (slot >= bitset->arraysize) {
        if (!bitset_grow(bitset, slot + 1)) {
            return;
        }
    }
    bitset->array[slot] |= ((uint64_t) 1) << BIT_OFFSET(i);
}

static inline void bitset_unset(bitset_t *bitset, size_t i) {
    size_t slot = BIT_SLOT(i);
    if (slot >= bitset->arraysize) {
            return;
    }
    bitset->array[slot] &= ~(((uint64_t) 1) << BIT_OFFSET(i));
}


/* Get the value of the ith bit.  */
static inline bool bitset_get(const bitset_t *bitset, size_t i) {
    size_t slot = BIT_SLOT(i);
    if (slot >= bitset->arraysize) {
        return false;
    }
    return (bitset->array[slot] & (((uint64_t) 1) << BIT_OFFSET(i))) != 0;
}

/* Toggle the value of the ith bit. */
static inline bool bitset_toggle(const bitset_t *bitset, size_t i) {
    size_t slot = BIT_SLOT(i);
    if (slot >= bitset->arraysize) {
        return false;
    }
    bitset->array[slot] ^= (((uint64_t) 1) << BIT_OFFSET(i));
    return true;
}

/* Count number of bit sets.  */
size_t bitset_count(const bitset_t *bitset);
size_t bitset_count_s(const bitset_t *bitset);

/* Find the index of the first bit set.  */
size_t bitset_minimum(const bitset_t *bitset);
size_t bitset_minimum_s(const bitset_t *bitset);

/* Find the index of the last bit set.  */
size_t bitset_maximum(const bitset_t *bitset);
size_t bitset_maximum_s(const bitset_t *bitset);

/* compute the union in-place (to b1), returns true if successful, to generate a new bitset first call bitset_copy */
bool bitset_inplace_union(bitset_t * restrict b1, const bitset_t * restrict b2);

/* report the size of the union (without materializing it) */
size_t bitset_union_count(const bitset_t * restrict b1, const bitset_t * restrict b2);

/* compute the intersection in-place (to b1), to generate a new bitset first call bitset_copy */
void bitset_inplace_intersection(bitset_t * restrict b1, const bitset_t * restrict b2);

/* report the size of the intersection (without materializing it) */
size_t bitset_intersection_count(const bitset_t * restrict b1, const bitset_t * restrict b2);

/* compute the difference in-place (to b1), to generate a new bitset first call bitset_copy */
void bitset_inplace_difference(bitset_t * restrict b1, const bitset_t * restrict b2);

/* compute the size of the difference */
size_t bitset_difference_count(const bitset_t *restrict b1, const bitset_t * restrict b2);

/* compute the symmetric difference in-place (to b1), return true if successful, to generate a new bitset first call bitset_copy */
bool bitset_inplace_symmetric_difference(bitset_t * restrict b1, const bitset_t * restrict b2);

/* compute the size of the symmetric difference  */
size_t bitset_symmetric_difference_count(const bitset_t *restrict b1, const bitset_t * restrict b2);

void bitset_inplace_complement(bitset_t * restrict b);

/* iterate over the set bits
 like so :
  for(size_t i = 0; nextSetBit(b,&i) ; i++) {
    //.....
  }
 */
static inline bool nextSetBit(const bitset_t *bitset, size_t *i) {
    size_t x = BIT_SLOT(*i);
    if (x >= bitset->arraysize) {
        return false;
    }
    uint64_t w = bitset->array[x];
    w >>= (*i & 63);
    if (w != 0) {
#ifndef SOFT_CTZ        
        *i += __builtin_ctzll(w);
#else
        *i += ctz(w);
#endif        
        return true;
    }
    x++;
    while (x < bitset->arraysize) {
        w = bitset->array[x];
        if (w != 0) {
#ifndef SOFT_CTZ                    
            *i = x * 64 + __builtin_ctzll(w);
#else            
            *i = x * 64 + ctz(w);            
#endif            
            return true;
        }
        x++;
    }
    return false;
}

/* iterate over the set bits
 like so :
   size_t buffer[256];
   size_t howmany = 0;
  for(size_t startfrom = 0; (howmany = nextSetBits(b,buffer,256, &startfrom)) > 0 ; startfrom++) {
    //.....
  }
 */
static inline size_t nextSetBits(const bitset_t *bitset, size_t *buffer, size_t capacity, size_t * startfrom) {
    if (capacity == 0) return 0; // sanity check
    size_t x = BIT_SLOT(*startfrom);
    if (x >= bitset->arraysize) {
        return 0; // nothing more to iterate over
    }
    uint64_t w = bitset->array[x];
    w >>= (*startfrom & 63);
    size_t howmany = 0;
    size_t base = x << 6;
    while (howmany < capacity) {
        while (w != 0) {
            uint64_t t = w & (~w + 1);
#ifndef SOFT_CTZ            
            int r = __builtin_ctzll(w);
#else
            int r = ctz(w);
#endif            
            buffer[howmany++] = r + base;
            if (howmany == capacity) goto end;
            w ^= t;
        }
        x += 1;
        if (x == bitset->arraysize) {
            break;
        }
        base += 64;
        w = bitset->array[x];
    }
end:
    if (howmany > 0) {
        *startfrom = buffer[howmany - 1];
    }
    return howmany;
}

typedef bool (*bitset_iterator)(size_t value, void *param);

// return true if uninterrupted

static inline bool bitset_for_each(const bitset_t *b, bitset_iterator iterator, void *ptr) {
    size_t base = 0;
    for (size_t i = 0; i < b->arraysize; ++i) {
        uint64_t w = b->array[i];
        while (w != 0) {
            uint64_t t = w & (~w + 1);
#ifndef SOFT_CTZ            
            int r = __builtin_ctzll(w);
#else            
            int r = ctz(w);
#endif            
            if (!iterator(r + base, ptr)) return false;
            w ^= t;
        }
        base += 64;
    }
    return true;
}

static inline void bitset_print(const bitset_t *b) {
    printf("{");
    for (size_t i = 0; nextSetBit(b, &i); i++) {
        printf("%zu, ", i);
    }
    printf("}");
}

#endif
