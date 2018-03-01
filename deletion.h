#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"



struct trie_node;
struct hash_layer;

typedef struct ngrams_to_delete{
	int nodes_capacity;
	int length_capacity;
	int *length;
	int pos;
	int word_size;
	char **nodes_to_delete;
}ngrams_to_delete;

void init_ngrams_to_delete(ngrams_to_delete *d_grams);
void add_ngram_to_delete(ngrams_to_delete *d_grams,char *node,int ngram);
void reset_ngrams_to_delete(ngrams_to_delete *d_grams);
void destroy_ngrams_to_delete(ngrams_to_delete *d_grams);
void print_ngrams_to_delete(ngrams_to_delete *d_grams);
