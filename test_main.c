#include "static_functions.h"
#include <errno.h>
#include <time.h>



int main (int argc, char **argv )
{
	int i;
	char file_name[16];
	struct index *trie=malloc(sizeof(struct index));
	trie->hash=createLinearHash(C,10);

	
	
	strcpy(file_name,"unit_test.init");
	test_hash_function(trie,file_name);
	if(init_test_input(trie,file_name,"add")<0) return -1;
	//print_hash(trie->hash);
	//--------------------testing static--------------------------//
	printf("Testing static trie\n");
	
	//tests_for_binary(trie);
	struct static_index *static_trie=malloc(sizeof(struct static_index));	
	static_hash_layer *static_hash=compress(trie->hash);
	static_trie->hash=static_hash;
	test_compress(static_trie->hash);
	test_everything_exists(static_trie,"unit_test.init");
	//print_static_hash(static_trie->hash);
	//print_static_hash(static_hash);
	destroy_static_hash(static_hash);
//-----------------------done testing stativ------------------//
	strcpy(file_name,"unit_test.work");
	if(tests_from_file(trie,file_name)<0) return -1;
	test_versioning(trie->hash);
	print_hash_version(trie->hash);
	free(static_trie);

	destroyLinearHash(trie->hash); //in all the cases i have to delete it
//-------------------------testing bloom and top--------------//
	test_bloom_bit();
	test_bloom();
	test_top();
	free(trie);
	return 0;	
}
