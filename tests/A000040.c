/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   A000040.c
 * Author: kakoulidis
 *
 * Created on 19 Φεβρουαρίου 2019, 1:41 μμ
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "bitset.h"

#define UBOUND 1000000000

/*
 * 
 */
int main(int argc, char** argv) {
    size_t i, j;
    size_t sq = (size_t) sqrt(UBOUND);
    float startTime, endTime;
    startTime = (float) clock() / CLOCKS_PER_SEC;
    bitset_t * primes = bitset_create_with_capacity(UBOUND + 1);

    for (i = 7; i < UBOUND + 1; i += 6) {
        bitset_set(primes, i);
        bitset_set(primes, i-2);
    }
    if ((i-2) < UBOUND + 1)
        bitset_set(primes, i-2);
    i = 0;

    while (nextSetBit(primes, &i) && (i <= sq)) {
        //printf("%zu\n", i);
        for (j = i * i; j < UBOUND + 1; j += 2 * i) {
            bitset_unset(primes, j);
        }
        i++;
    }

    endTime = (float) clock() / CLOCKS_PER_SEC;
    printf("test primes - %zu %f\n", bitset_count(primes), endTime - startTime);
    bitset_free(primes);
    return (EXIT_SUCCESS);
}

