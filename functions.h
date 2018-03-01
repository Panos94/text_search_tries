#ifndef _FUNCTIONSH_
#define _FUNCTIONSH_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"


#ifndef STACK_H
#define STACK_H
#include "stack.h"
#endif

#include "top.h"
#include "bloomfilter.h"
#include "libraries.h"
#include "schedule.h"

#ifndef DEL_H
#define DEL_H
#include "deletion.h"
#endif

#ifndef WORD_SIZE
#define WORD_SIZE 25
#endif

#ifndef CHILD_NUM
#define CHILD_NUM 10
#endif

#ifndef MAX_CHILDS
#define MAX_CHILDS 10
#endif
 
#ifndef ERROR
#define ERROR 1
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

#define PATH_COLUMN 10

#define EMPTY_POINTER NULL

#define C 8
//#define STACK_NUMBER 10
//#define STACK_EMPTY -1

typedef struct trie_node{
	//char static_word[WORD_SIZE];
	char *word;
	struct trie_node *children;
	char is_final;
	int number_of_childs;
	int max_childs;
	int A_version;
	int D_version;
	int children_deleted;
}trie_node;


typedef struct paths{

	int **paths_array;//[10][10];
	int words_in;
	int max_words;
}paths;

typedef struct hash_bucket{
	//struct hash_bucket *overflow_bucket;
	trie_node *children;
	int children_number;
	int capacity;
}hash_bucket;

typedef struct hash_layer{
	int bucket_capacity;
	hash_bucket *buckets;
	int buckets_number;
	double load_factor;
	int total_children;
	int bucket_to_split;
	int split_round;
}hash_layer;

struct index{
	trie_node *root;
	hash_layer *hash;
};

void cleanup_A(char ** ptr,int A_table_size);
void cleanup2(char ** ptr);
void printtable(char ** pt,int num);
void printpanos(void);
trie_node *init_trie();
void delete_trie(struct index *trie);
void destroy_childs(trie_node *node);
int delete_ngram(trie_node *root,char **word,int word_number,int number_of_words);
int search_in_trie(trie_node *root,char **word,int number_of_words,kframes * kf);
int check_exists_in_children(trie_node *node,char *word,int *pos);
int append_trie_node(trie_node *root,char **word,int word_number,int number_of_words,int current_version);
int delete_from_node(trie_node *node,int pos);
int delete_ngram(trie_node *root,char **word,int word_number,int number_of_words);
int append_word(trie_node *node,int pos,char *word,char is_final,int current_version);
int append_trie_node_iterative(trie_node *root,char **word,int word_number,int number_of_words,int current_version);
void print_trie(trie_node *node,int level);
void print_nodes_from_stack(trie_node *root,stack *stack_);
int init_input(struct index *trie,char * filename);
int test_input(struct index *trie,char * filename);

void cleanup(char ** ptr);



//part 2
char * myappend(char * word, char * string);
char * myappend1(char * word, char * string);
//unsigned long hash(/*unsigned*/ char *str, int key);
hash_layer	*createLinearHash(int c ,int m);
void destroyLinearHash(hash_layer *hash);
int insertTrieNode(hash_layer *hash,char **words,int word_number,int current_version);
int lookupTrieNode(hash_layer *hash,char **words,int word_number);

trie_node *add_to_backet(hash_layer *hash,int hash_val,char *word,char is_final,int current_version);

void shrink_buckets(hash_bucket *bucket,stack *stack_);
void shrink_bucket(hash_bucket *bucket,stack *stack_,int first,int last);
void print_hash(hash_layer *hash);
int resize_hash(hash_layer *hash);
void destroy_bucket_nodes(hash_bucket *bucket);
void destroy_buckets(hash_bucket *bucket,int level);

trie_node *delete_from_backet(hash_layer *hash,int hash_val,char *word,int *pos);
int deleteTrieNode(hash_layer *hash,char **words,int number_of_words);
int  hash_function(hash_layer *hash, char *word);
void test(void);
char * detableize(char * str, char ** table);
int lookupTrieNode_with_bloom(hash_layer *hash,char **words,int number_of_words,topk *top);//topk * top); //kframes *kf
int check_exists_in_bucket(char *word,int *pos,trie_node *children,int children_number);
///*int check_exists_in_bucket(hash_bucket *bucket,char *word,int *pos);


void delete_ngrams(hash_layer *hash,ngrams_to_delete *d_grams);
int delete_from_node_cleanup(trie_node *node,int pos,int current_version);
int delete_ngram_cleanup(trie_node *root,char **word,int word_number,int number_of_words,int current_version);
trie_node *delete_from_backet_cleanup(hash_layer *hash,int hash_val,char *word,int *pos,int current_version);
int deleteTrieNode_cleanup(hash_layer *hash,char **words,int word_number,int current_version);


int check_node(trie_node *node,int current_version);
int lookupTrieNode_with_bloom_versioning(hash_layer *hash,char **words,int number_of_words,topk* top,int current_version,int section_start);
int lookupTrieNode_with_bloom_versioning_threads(void ** arguments);

trie_node *delete_from_backet_versioning(hash_layer *hash,int hash_val,char *word,int *pos);
int deleteTrieNode_versioning(hash_layer *hash,char **words,int word_number,int current_version);
int delete_from_node_versioning(trie_node *node,int pos,int current_version);
int delete_ngram_versioning(trie_node *root,char **word,int word_number,int number_of_words,int current_version);

//------------------------versioning cleanup functions---------------------------------//
trie_node *delete_from_backet_versioning_cleanup(hash_layer *hash,int hash_val,char *word,int *pos);
int deleteTrieNode_versioning_cleanup(hash_layer *hash,char **words,int word_number,int current_version);
int delete_from_node_versioning_cleanup(trie_node *node,int pos,int current_version);
int delete_ngram_versioning_cleanup(trie_node *root,char **word,int word_number,int number_of_words,int current_version);
void print_trie_version(trie_node *node,int level);
//=======
#endif
//>>>>>>> antonis_3
