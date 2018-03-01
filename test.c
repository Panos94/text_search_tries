#include "test.h"
#include <stdlib.h>
#include "fcntl.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>


int init_test_input(struct index *trie,char * filename,char *command){
	//printf("\x1b[32m""INIT_INPUT start\n""\x1b[0m");
	int table_size = 10;
	int word_size = 25;
	int a;
	char **ptr_table = malloc(table_size*sizeof(char *));
	int words_in = 0;

	FILE* fd = fopen(filename, "r"); //opening input file
	//strcpy(buffer,"\0");

	if(fd == NULL){
		perror("Error opening input file");
		return -1;
	}

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *word;

	for(a=0;a<table_size;a++)
		ptr_table[a]=malloc(word_size*sizeof(char));
			
	while ((read = getline(&line, &len, fd)) != -1){
		words_in = 0;
		word = strtok (line," \n");
		while(word!=NULL)
		{
		//printf("Read this word: %s\n",word);
			if(words_in==table_size){
				table_size*=2;
				ptr_table = realloc(ptr_table,table_size*sizeof(char*));
				for(a=(table_size/2);a<table_size;a++)
					ptr_table[a]=malloc(word_size*sizeof(char));
			}
			if(strlen(word)>word_size){
				word_size*=2;
				for(a=0;a<table_size;a++)
					ptr_table[a] = realloc(ptr_table[a],word_size*sizeof(char));
			}
		//	ptr_table[words_in] = malloc(word_size*sizeof(char));
			strcpy(ptr_table[words_in],word);
			//printf("Kuru word = %s\n",ptr_table[words_in]);
			words_in++;
			word=strtok(NULL," \n");
		}
		if(strcmp("add",command)==0) test_add(trie,ptr_table,words_in,FOUND);
		else deleteTrieNode(trie->hash,ptr_table,words_in-1);
	}
	//printf ("free\n");
	free(line);
	cleanup(ptr_table);
	fclose(fd);

	//printf("\x1b[32m""INIT_INPUT end\n""\x1b[0m");
	return 0;	
}


int test_if_exists(struct index *trie,char **words ,int words_size){
	//printf("Inside check if exists\n");
	int i;
	trie_node *node;
	int found=NOT_FOUND;
	int pos;

	int hash_val=hash_function(trie->hash,words[0]);
	hash_bucket *bucket=&(trie->hash->buckets[hash_val]);
	found=check_exists_in_bucket(words[0],&pos,bucket->children,bucket->children_number);
	if(found==0) return found; //not found
	node=&(bucket->children[pos]);
	
	i=1; //keep checking
	while(i!=words_size){
		found=check_exists_in_children(node,words[i],&pos);
		if(found==NOT_FOUND) break;
		node=&(node->children[pos]);
		i++;  
		}
		if(found==FOUND && node->is_final=='n') found=NOT_FOUND;
		return found;
}


void test_delete(struct index *trie,char **words_to_check ,int words_size,int expected_result){
		int error=deleteTrieNode(trie->hash,words_to_check,words_size-1);
		//printf("error is %d\n",error);
		int result=test_if_exists(trie,words_to_check ,words_size);
		//printf("result is %d\n",result);
		if(result!=expected_result){
			printf("Wrong result in delete\n");
		}
		
	}

void test_add(struct index *trie,char **words_to_check ,int words_size,int expected_result){
		insertTrieNode(trie->hash,words_to_check,words_size,0);
		int found=test_if_exists(trie,words_to_check ,words_size);
		if(found!=FOUND) printf("Ngram was not added\n");
	}

void test_binary_search(struct index *trie,char *word,int expected_found,int expected_position){
	int found;
	int pos;
	found=check_exists_in_children(trie->root,word,&pos);
	if(found!=expected_found){ printf("Wasn't found\n");return;}
	if(pos!=expected_position && found==1){ printf("%s Wasn't found in the right position\n",word);return;}
}

void tests_for_binary(struct index *trie){
	char *test_word=malloc(15*sizeof(char));
	//some tests
	strcpy(test_word,"0000");
	test_binary_search(trie,test_word,0,0);
	
	strcpy(test_word,"zzz");
	test_binary_search(trie,test_word,0,trie->root->number_of_childs-1);

	free(test_word);
}

int parseInt (char c) {
    return c - '0';
}

int tests_from_file(struct index *trie,char * filename){
	printf("Inside tests from file\n");
	int table_size=10;
	int word_size=25;	
	char **ptr_table = malloc(table_size*sizeof(char *));
	int words_in = 0;
	int flag; //1 question, 2 addition, 3 deletion, 4 end of file
	int a;
	FILE* fd = fopen(filename, "r"); //opening input file
	if(fd == NULL)
	{
		perror("Error opening input file");
		return -1;
	}
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *word;
	int command_error;


	for(a=0;a<table_size;a++)
		ptr_table[a]=malloc(word_size*sizeof(char));

	while ((read = getline(&line, &len, fd)) != -1) {
		//words_in = 1;
		words_in = 0;
		
		word = strtok (line," \n");
		while(word!=NULL){
			if(strcmp(word,"Q")==0){
				flag=1;
			}
			else if(strcmp(word,"A")==0){
				flag=2;
			}
			else if(strcmp(word,"D")==0){
				flag=3;
			}
			else if(strcmp(word,"F")==0){
				//printf("\x1b[36m""EOF -1\n""\x1b[0m");
				/*
				cleanup(ptr_table);
				free(line);
				fclose(fd);
				printf("\x1b[32m""TEST_INPUT end\n""\x1b[0m");
				return 1;
				*/
				//printf("\x1b[32m""F -> print paths\n""\x1b[0m");	
			}else if(strcmp(word,"\0")==0){				
			}
			else{
			
				
				if(words_in==table_size-1){
					table_size*=2;
					ptr_table = realloc(ptr_table,table_size*(sizeof(char*)));
					for(a=table_size/2;a<table_size;a++){
						ptr_table[a]=malloc(word_size*sizeof(char));
					}
				}
				if(strlen(word)>=word_size){
					word_size*=2;
					for(a=0;a<table_size;a++)
						ptr_table[a] = realloc(ptr_table[a],word_size*sizeof(char));
				}		
			strcpy(ptr_table[words_in],word);
			words_in++;				
			}
			word=strtok(NULL," \n");
		}


	int expected_result = parseInt(ptr_table[words_in-1][0]);  //kratasei ton teleytaio arithmo apo tin seira pou diabase
	words_in --;  									//meiwnei to words_in gia na min lifthei upopsin o teleytaios arithmos

	switch(flag){
		case 1 :
			printf("\n"); 
			break;
		case 2 :
			test_add(trie,ptr_table,words_in,expected_result);
			break;
		case 3 :
			test_delete(trie,ptr_table,words_in,expected_result);
		
		}
		flag=0;	
	}

  	free(line);
	cleanup(ptr_table);
	fclose(fd);
	
return 0;

}

void test_hash_function(struct index *trie,char * filename){

	init_test_input(trie,filename, "add");
	init_test_input(trie,filename, "delete");
	int empty=check_if_empty(trie->hash);
	if(empty==1){
		printf("All found and removed correctly\n");
		return ;
	}
	printf("Not removed correctly\n");
	return ;


}


int check_if_empty(hash_layer *hash){
	int i;
	hash_bucket *bucket;
	for(i=0;i<hash->buckets_number;i++){
		bucket=&(hash->buckets[i]);
		if(bucket->children_number>0) return 0;
	}
	return 1;
}


//-------------------------------------------------unit testing in static functions -----------------------------------------//




int test_if_exists_static(struct static_index *trie,char **words ,int words_size){
	//printf("Inside check if exists\n");
	int i;
	char *temp_word=malloc(WORD_SIZE*sizeof(char));
	int word_size=WORD_SIZE;
	
	static_trie_node *node;
	int found=NOT_FOUND;
	int pos;

	int hash_val=static_hash_function(trie->hash,words[0]);
	static_hash_bucket *bucket=&(trie->hash->buckets[hash_val]);
	found=check_exists_in_static_bucket(bucket,words[0],&pos);
	if(found==0) return found; //not found
	node=&(bucket->children[pos]);
	
	i=1; //keep checking
	int node_word=1;
	while(i!=words_size){
		if(node_word==node->number_of_words){
			found=check_exists_in_static_children(node,words[i],&pos);
			if(found==NOT_FOUND) break;
			node=&(node->children[pos]);
			i++;
			node_word=1;
			}
		else {
			temp_word=get_i_word(node,node_word,temp_word,&word_size);
			
			if(strcmp(temp_word,words[i])==0) found=FOUND;
			else found=NOT_FOUND; 
			i++;
			node_word++;
		} 
		}
		if(found==FOUND && node->is_final[node_word-1]>0) found=NOT_FOUND;
		free(temp_word);
		return found;
}

int check_number_of_childs(static_trie_node *node){
	if(node->number_of_childs==1) return 1;
	int i,error;
	for(i=0;i<node->number_of_childs;i++){
		error=check_number_of_childs(&(node->children[i]));
		if(error==1) return 1;
	}
	return 0;
}

void test_compress(static_hash_layer *hash){
	int i,j;
	static_hash_bucket *bucket;
	for(i=0;i<hash->buckets_number;i++){
		bucket=&(hash->buckets[i]);
		for(j=0;j<bucket->children_number;j++){
			int error=check_number_of_childs(&(bucket->children[j]));
			if(error == 1){
				printf("Compress failed\n");
				return;
			}
		}
	}
	printf("compress succeded\n");
	return;
}

void test_everything_exists(struct static_index *trie,char * filename){
	//printf("\x1b[32m""INIT_INPUT start\n""\x1b[0m");
	int table_size = 10;
	int word_size = 25;
	int a;
	char **ptr_table = malloc(table_size*sizeof(char *));
	int words_in = 0;

	FILE* fd = fopen(filename, "r"); //opening input file
	//strcpy(buffer,"\0");

	if(fd == NULL){
		perror("Error opening input file");
		return ;
	}

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *word;
	int result;
	for(a=0;a<table_size;a++)
		ptr_table[a]=malloc(word_size*sizeof(char));
			
	while ((read = getline(&line, &len, fd)) != -1){
		words_in = 0;
		word = strtok (line," \n");
		while(word!=NULL)
		{
		//printf("Read this word: %s\n",word);
			if(words_in==table_size){
				table_size*=2;
				ptr_table = realloc(ptr_table,table_size*sizeof(char*));
				for(a=(table_size/2);a<table_size;a++)
					ptr_table[a]=malloc(word_size*sizeof(char));
			}
			if(strlen(word)>word_size){
				word_size*=2;
				for(a=0;a<table_size;a++)
					ptr_table[a] = realloc(ptr_table[a],word_size*sizeof(char));
			}
		//	ptr_table[words_in] = malloc(word_size*sizeof(char));
			strcpy(ptr_table[words_in],word);
			//printf("Kuru word = %s\n",ptr_table[words_in]);
			words_in++;
			word=strtok(NULL," \n");
		}
		result=test_if_exists_static(trie,ptr_table,words_in);
		if(result==NOT_FOUND){
			printf("FAILURE:Something is missing from trie\n");
			free(line);
			cleanup(ptr_table);
			fclose(fd);
			return ;
		}
		//test_static_add(trie,ptr_table,words_in,FOUND);
	}
	//printf ("free\n");
	printf("SUCCED:everything is ok in static_trie\n");
	free(line);
	cleanup(ptr_table);
	fclose(fd);


	return ;
}

void test_bloom_bit(void){
	printf("Test bloom bit: ");
	int flag = 0;
	int a = 63;
	int * bloomfilter = malloc(8192);
	bloomfilter_init(bloomfilter,8192*8);
	if(TestBit(bloomfilter,a)){
		printf("Error in Initbit\n");
		flag++;
	}

	SetBit(bloomfilter,a);
	if(!TestBit(bloomfilter,a)){
		printf("Error in Setbit\n");
		flag++;
	}

	ClearBit(bloomfilter,a);
	if(TestBit(bloomfilter,a)){
		printf("Error in Clearbit\n");
		flag++;
	}

	free(bloomfilter);
 if(flag==0)
	printf("No errors\n");
}

void test_bloom(void){
	printf("Test bloom: ");
	int flag = 0;
	int bits = 8192*8, bytes=8192;
	int * bloomfilter = malloc(bytes);
	bloomfilter_init(bloomfilter,bits);

	if(bloomfilter_check("lets check the bloomfilter",bloomfilter,bytes)){
		printf("Error in bloomfilter init\n");
		flag++;
	}
	bloomfilter_add("lets check the bloomfilter2",bloomfilter,bytes);
		if(!bloomfilter_check("lets check the bloomfilter2",bloomfilter,bytes)){
		printf("Error in bloomfilter add\n");
		flag++;
	}
	
	free(bloomfilter);
if(flag==0)
	printf("No errors\n");
}

int test_top(void){
	printf("Test_top: ");
	int flag = 0;
	topk * top;
	top = create_top(top);
	top = init_top(top);
	int previous_capacity=top->kf->capacity;
	top = extend_top(top);
	if (top->kf->capacity!=previous_capacity*2){
		printf("Error in extend top\n");
		flag++;
		}
	top = add_top(top,"antonis");
	if(top->fr->frequency[0] != 1){
		printf("Error in add top\n");
		flag++;
		}
	top->fr->frequency[0] = 5;
	top->fr->frequency[1] = 35;
	top->fr->frequency[2] = 23;
	top->fr->frequency[3] = 240;
	int i;

	top = add_top(top,"antonis");
	top = add_top(top,"antonis");
	top = add_top(top,"antonis");
	top = add_top(top,"antonis");
	top = add_top(top,"bntonis");
	top = add_top(top,"bntonis");
	top = add_top(top,"bntonis");
	top = add_top(top,"cntonis");
	top = add_top(top,"dntonis");
	top = add_top(top,"dntonis");

	quickSort(top->fr->frequency,top->fr->ngram,0,top->fr->unique-1,top->kf->ngrams);

	for (i=1;i<5;i++){
	//	printf("%s ",top->kf->ngrams[i]);
	//	printf("%d\n",top->fr->frequency[i]);
	}
	if((top->fr->frequency[0]<top->fr->frequency[1])||(top->fr->frequency[1]<top->fr->frequency[2])||
		(top->fr->frequency[2]<top->fr->frequency[3]))
		{
		printf("Error in init/create top\n");
		flag++;
	}
	if(flag==0)
		printf("No errors\n");
	
	erase_top(top);
}

void test_versioning(hash_layer *hash){
	int i;
	
	int results=open("cout.log",O_CREAT | O_RDWR,0600);
	if(results==-1){
		printf("error in opening");
		return;	
	}
	//rewind(results);
	int save_out=dup(fileno(stdout));
	if(dup2(results,fileno(stdout))==-1){
		printf("error in redirecting stdout");
		return;	
	}

	topk * top;
	top = create_top(top);
	top = init_top(top);

	char **test_version=malloc(10*sizeof(char*));
	int *versions=malloc(10*sizeof(int));
	for(i=0;i<10;i++){
		test_version[i]=malloc(50*sizeof(char));
	}
	strcpy(test_version[0],"panos");
	versions[0]=0;
	
	strcpy(test_version[1],"eats");
	versions[1]=1;

	strcpy(test_version[2],"a");
	versions[2]=2;
	strcpy(test_version[3],"lot");
	versions[3]=3;

	strcpy(test_version[4],"of");
	versions[4]=4;

	for(i=0;i<5;i++){
			insertTrieNode(hash,test_version,i+1,i);
	}
	for(i=0;i<5;i++){
		lookupTrieNode_with_bloom_versioning(hash,test_version,i+1,top,i,0);
	}
	for(i=0;i<5;i++){
		deleteTrieNode_versioning(hash,test_version,i+1,5);
	}
	for(i=0;i<5;i++){
		lookupTrieNode_with_bloom_versioning(hash,test_version,i+1,top,5,0);
	}

	print_print(top);
	
	fflush(stdout);
	close(results);
	dup2(save_out,fileno(stdout));
	close(save_out);
	if(check_identical_files("cout.log","expected_r.log")==1){
		printf("No errors in versioning\n");	
	}
	else printf("There are errors in versioning\n");	
	free(versions);
	for(i=0;i<10;i++){
		free(test_version[i]);
	}
	free(test_version);
	erase_top(top);
}

int check_identical_files(char *filename1,char *filename2){
	FILE *f1 = fopen(filename1, "r");
 	if (!f1) { printf("file %s doesnt_exist",filename1); return 0; }
 	FILE *f2 = fopen(filename2, "r");
 	if (!f2) { printf("file %s doesnt_exist",filename2); return 0; }
 	int samefile = 1;
 	int c1, c2;
	c1 = getc(f1);
	c2 = getc(f2);
 	while (samefile && (c1!= EOF) || c2!= EOF){
    	if (c1 != c2){
			samefile = 0;
			printf("error in letter \"%c\" \"%c\"\n",c1,c2 );
			//return samefile;
		}
	c1 = getc(f1);
	c2 = getc(f2);
	}
 	fclose (f1), fclose (f2);
	return samefile;
}









