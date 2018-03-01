#ifndef _STATICFUNCTIONSH_
#define _STATICFUNCTIONSH_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "stack.h"
#include "math.h"

#ifndef FUNC_H
#define FUNC_H
#include "functions.h"
#endif

#ifndef WORD_SIZE
#define WORD_SIZE 25
#endif


typedef struct static_trie_node{
	char *word;
	struct static_trie_node *children;
	short *is_final;
	int number_of_words;
	int number_of_childs;
	int max_childs;

}static_trie_node;

typedef struct static_hash_bucket{
	static_trie_node *children;
	int children_number;
	int capacity;
}static_hash_bucket;

typedef struct static_hash_layer{
	int bucket_capacity;
	static_hash_bucket *buckets;
	int buckets_number;
	double load_factor;
	int total_children;
	int bucket_to_split;
	int split_round;
}static_hash_layer;

typedef struct static_index{
	static_hash_layer *hash;
}static_index;

static_hash_layer *initialize_static_hash(hash_layer *hash);
static_hash_layer *compress(hash_layer *hash);
int compress_node(trie_node *node,static_trie_node *static_node);
int merge_nodes(static_trie_node *static_node,trie_node *node,static_trie_node *child);
void init_final_static_node(static_trie_node *static_node,char *word,char is_final,int c);
void destroy_static_hash(static_hash_layer *hash);
void destroy_static_bucket_nodes(static_hash_bucket *bucket);
void destroy_static_childs(static_trie_node *node);
static_hash_layer *compress(hash_layer *hash);
void init_static_node(static_trie_node *static_node,char *word,char is_final);

void print_static_trie(static_trie_node *node,int level);
void print_static_hash(static_hash_layer *hash);

void print_lens(short *is_final,int words_number);


void shrink_static_buckets(static_hash_bucket *bucket,stack *stack_);
int resize_static_hash(static_hash_layer *hash);
static_trie_node* add_to_static_backet(static_hash_layer *hash,int hash_val,char *word,char is_final);
int check_exists_in_static_bucket(static_hash_bucket *bucket,char *word,int *pos);
int insert_staticTrieNode(static_hash_layer *hash,char **words,int word_number);
int  static_hash_function(static_hash_layer *hash, char *word);
char* get_i_word(static_trie_node *node,int i,char* temp_word,int *temp_word_size);

int append_static_trie_node(static_trie_node *root,char **word,int word_number,int number_of_words);
int split_static_node(static_trie_node *node ,int pos);
int concat_word(static_trie_node *static_node,char *word,char is_final);
int check_exists_in_static_children(static_trie_node *node,char *word,int *pos);
int append_static_word(static_trie_node *node,int pos,char *word,char is_final);
int check_exists_in_node(static_trie_node *node,char *word,int expected);
int init_static_input(struct static_index *trie,char * filename);
int test_static_input(struct static_index *trie,char * filename);
void init_static_node_with_children(static_trie_node *static_node,char *word,char is_final,int c);
void update_word(static_trie_node *node,int pos);
int lookup_static_TrieNode(static_hash_layer *hash,char **words,int number_of_words,topk_threads *top,int Q_number);
void print_nodes_from_static_hash(static_hash_layer *hash,stack *stack_);
int check_in_static_paths(paths *paths_, stack *stack_,static_hash_layer *hash);
void  stat_myappend_pan(char **string,int *str_size, char * word);
void  stat_myappend_pan_with_space(char **string,int *str_size, char * word);
int lookup_static_TrieNode_threads(void ** arguments);
#endif
