#include "bloomfilter.h"

#ifndef FUNC_H
#define FUNC_H
#include "functions.h"
#endif
#include "static_functions.h"
#include <errno.h>
#include <time.h>

//#define hash_length 1024
#define bitsint sizeof(int)*8


int main (int argc, char **argv ){	
//	clock_t begin = clock();
	int c;
	char *init_file;
	char *query_file;
	int init_result;
	
	if(argc<5){
		printf("Wrong quantity of arguments\n");
		return -1;
	}
	

	for(c=0;c<argc;c++)
	{
		if(strcmp(argv[c],"-i")==0){
			init_file=malloc((strlen(argv[c+1])+1)*sizeof(char));
			strcpy(init_file,argv[c+1]);
		}
		if(strcmp(argv[c],"-q")==0){
			query_file=malloc((strlen(argv[c+1])+1)*sizeof(char));
			strcpy(query_file,argv[c+1]);
		}	
	}
	
	//printf("Init %s\nQueries %s\n",init_file,query_file);

	struct index *trie=malloc(sizeof(struct index));
	trie->hash=createLinearHash(C,10);
	//trie->root=init_trie();
	init_result=init_input(trie,init_file);
	if(init_result<0) return -1;
	else if(init_result==1){//static
			//printf("in static\n");
			struct static_index *static_trie=malloc(sizeof(struct static_index));	
			static_hash_layer *static_hash=compress(trie->hash);
			static_trie->hash=static_hash;
			if(test_static_input(static_trie,query_file)<0) return -1;
			//print_static_hash(static_hash);
			destroy_static_hash(static_hash);
			free(static_trie);
	}
	else{//no static
		//printf("queries\n");
		if(test_input(trie,query_file)<0) return -1;
		//print_hash_version(trie->hash);
	}
	destroyLinearHash(trie->hash); //in all the cases i have to delete it
	free(trie);
	free(init_file);
	free(query_file);
	//printf("exited normally\n");
	return 0;	
}
