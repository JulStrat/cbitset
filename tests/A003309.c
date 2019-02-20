/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   A003309.c
 * Author: kakoulidis
 *
 * Created on 20 Φεβρουαρίου 2019, 10:04 πμ
 */

/*
142 997
1015 9997
7986 99997
*/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "bitset.h"

#define UBOUND 100000

/*
 * 
 */

bool skipBits(bitset_t *b, size_t c, size_t *i) {
    while (c > 0) {
        (*i)++;
        if (!nextSetBit(b, i))
            return false;
        c--;
    }
    return true;
}

int main(int argc, char** argv) {
    size_t i, j, c;
    float startTime, endTime;
    startTime = (float) clock() / CLOCKS_PER_SEC;
    bitset_t * ludic = bitset_create_with_capacity(UBOUND + 1);

    // setup bit set
    for (i = 7; i < UBOUND + 1; i += 6) {
        bitset_set(ludic, i);
        bitset_set(ludic, i-2);
    }
    if ((i-2) < UBOUND + 1)
        bitset_set(ludic, i-2);
    
    i = 0;
    
    while (nextSetBit(ludic, &i) && (i < UBOUND + 1)) {
        //printf("%zu\n", i);
        c = i;
        while (skipBits(ludic, c, &i))
            bitset_unset(ludic, i);
        i = c+1;
    }

    endTime = (float) clock() / CLOCKS_PER_SEC;
    printf("test ludic - %zu %f\n", bitset_count(ludic) + 3, endTime - startTime);
    bitset_free(ludic);
    return (EXIT_SUCCESS);
}

