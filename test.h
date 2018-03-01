#ifndef _TESTH_
#define _TESTH_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "static_functions.h"
#include "bloomfilter.h"

#define FOUND 1
#define NOT_FOUND 0

int test_if_exists(struct index *trie,char **words_to_check ,int words_size);


void test_delete(struct index *trie,char **words_to_check ,int words_size,int expected_result);
int tests_from_file(struct index *trie,char * filename);

void test_add(struct index *trie,char **words_to_check ,int words_size,int expected_result);

void test_search();
void test_binary_search(struct index *trie,char *word,int expected_found,int expected_position);
void tests_for_binary(struct index *trie);
int init_test_input(struct index *trie,char * filename,char *command);
void test_hash_function(struct index *trie,char * filename);
int check_if_empty(hash_layer *hash);
void test_everything_exists(struct static_index *trie,char * filename);
void test_bloom_bit(void);
void test_bloom(void);
int test_top(void);
#endif
