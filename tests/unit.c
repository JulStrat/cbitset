#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "bitset.h"

#define DELTA 100
#define MAX_INDEX 100000000L

void test_iterate() {
    bitset_t * b = bitset_create();
    for (int k = 0; k < MAX_INDEX; ++k)
        bitset_set(b, 3 * k);
    assert(bitset_count(b) == MAX_INDEX);
    size_t k = 0;
    for (size_t i = 0; nextSetBit(b, &i); i++) {
        assert(i == k);
        k += 3;
    }
    assert(k == 3 * MAX_INDEX);
    bitset_free(b);
}

bool increment(size_t value, void *param) {
    size_t k;
    memcpy(&k, param, sizeof (size_t));
    assert(value == k);
    k += 3;
    memcpy(param, &k, sizeof (size_t));
    return true;
}

void test_iterate2() {
    bitset_t * b = bitset_create();
    for (int k = 0; k < MAX_INDEX; ++k)
        bitset_set(b, 3 * k);
    assert(bitset_count(b) == MAX_INDEX);
    size_t k = 0;
    bitset_for_each(b, increment, &k);
    assert(k == 3 * MAX_INDEX);
    bitset_free(b);
}

void test_construct() {
    bitset_t * b = bitset_create();
    for (int k = 0; k < MAX_INDEX; ++k)
        bitset_set(b, 3 * k);
    assert(bitset_count(b) == MAX_INDEX);
    for (int k = 0; k < 3 * MAX_INDEX; ++k)
        assert(bitset_get(b, k) == (k / 3 * 3 == k));
    bitset_free(b);
}

void test_max_min() {
    bitset_t * b = bitset_create();
    for (size_t k = 100; k < MAX_INDEX; ++k) {
        bitset_set(b, 3 * k);
        assert(bitset_minimum(b) == 3 * 100);
        assert(bitset_maximum(b) == 3 * k);
    }
    bitset_free(b);
}

void test_shift_left() {
    for (size_t sh = 0; sh < 256; sh++) {
        bitset_t * b = bitset_create();
        int power = 3;
        size_t s1 = 100;
        size_t s2 = 5000;
        for (size_t k = s1; k < s2; ++k) {
            bitset_set(b, power * k);
        }
        size_t mycount = bitset_count(b);
        bitset_shift_left(b, sh);
        assert(bitset_count(b) == mycount);
        for (size_t k = s1; k < s2; ++k) {
            assert(bitset_get(b, power * k + sh));
        }
        bitset_free(b);
    }
}

void test_shift_right() {
    for (size_t sh = 0; sh < 256; sh++) {
        bitset_t * b = bitset_create();
        int power = 3;
        size_t s1 = 100 + sh;
        size_t s2 = s1 + 5000;
        for (size_t k = s1; k < s2; ++k) {
            bitset_set(b, power * k);
        }
        size_t mycount = bitset_count(b);
        bitset_shift_right(b, sh);
        assert(bitset_count(b) == mycount);
        for (size_t k = s1; k < s2; ++k) {
            assert(bitset_get(b, power * k - sh));
        }
        bitset_free(b);
    }
}

void test_union_intersection() {
    bitset_t * b1 = bitset_create();
    bitset_t * b2 = bitset_create();

    for (int k = 0; k < MAX_INDEX; ++k) {
        bitset_set(b1, 2 * k);
        bitset_set(b2, 2 * k + 1);
    }
    // calling xor twice should leave things unchanged
    bitset_inplace_symmetric_difference(b1, b2);
    assert(bitset_count(b1) == 2 * MAX_INDEX);
    bitset_inplace_symmetric_difference(b1, b2);
    assert(bitset_count(b1) == MAX_INDEX);
    bitset_inplace_difference(b1, b2); // should make no difference
    assert(bitset_count(b1) == MAX_INDEX);
    bitset_inplace_union(b1, b2);
    assert(bitset_count(b1) == 2 * MAX_INDEX);
    bitset_inplace_intersection(b1, b2);
    assert(bitset_count(b1) == MAX_INDEX);
    bitset_inplace_difference(b1, b2);
    assert(bitset_count(b1) == 0);
    bitset_inplace_union(b1, b2);
    bitset_inplace_difference(b2, b1);
    assert(bitset_count(b2) == 0);
    bitset_free(b1);
    bitset_free(b2);
}

void test_counts() {
    bitset_t * b1 = bitset_create();
    bitset_t * b2 = bitset_create();

    for (int k = 0; k < 1000; ++k) {
        bitset_set(b1, 2 * k);
        bitset_set(b2, 3 * k);
    }
    assert(bitset_intersection_count(b1, b2) == 334);
    assert(bitset_union_count(b1, b2) == 1666);
    bitset_free(b1);
    bitset_free(b2);
}

int main() {
    test_construct();
    test_union_intersection();
    test_iterate();
    test_iterate2();
    test_max_min();
    test_counts();
    test_shift_right();
    test_shift_left();
    printf("All asserts passed. Code is probably ok.\n");
}
