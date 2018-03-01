#ifndef FUNC_H
#define FUNC_H
#include "functions.h"
 
//#include "bloomfilter.h"

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

//Second Part of Project

//#define ClearBit(A,k)   ( A[(k/sizeof(int))] &= ~(1 << (k%sizeof(int))) )
//#define TestBit(A,k)    ( A[(k/sizeof(int))] & (1 << (k%sizeof(int))) ) 
//#define SetBit(A,k)     ( A[(k/sizeof(int))] |= (1 << (k%sizeof(int))) )

#endif

#include "libraries.h"
//
#include <stdio.h>
//
/*extern*/ int buffer_size = 16;
/*extern*/ int word_size = 8;
/*extern*/ int table_size = 8;

void printtable(char **pt, int num){
	int a;
	for(a=0;a<=num;a++){
		printf("\x1b[31m""#Table %d: %s %ld\n""\x1b[0m",num ,pt[a],strlen(pt[a]));
	}
}

int init_input(struct index *trie,char * filename){
	//printf("\x1b[32m""INIT_INPUT start\n""\x1b[0m");
	int a;
	char **ptr_table = malloc(table_size*sizeof(char *));
	int *word_lens=malloc(table_size*sizeof(int));
	int words_in = 0;

	FILE* fd = fopen(filename, "r"); //opening input file
	//strcpy(buffer,"\0");
		
	if(fd == NULL){
		perror("Error opening input file");
		printf("File name %s\n",filename);
		return -1;
	}

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *word;
	int static_flag=0;
	int word_len;
	int previous_table;

	for(a=0;a<table_size;a++){
		word_lens[a]=word_size;
		ptr_table[a]=malloc(word_size*sizeof(char));
	}			
	//read first word
	if((read = getline(&line, &len, fd)) != -1){
		words_in = 0;
		word = strtok (line," \n");
		if(strcmp(word,"STATIC")==0) static_flag=1;
	}

	rewind(fd);
	while ((read = getline(&line, &len, fd)) != -1){
		words_in = 0;
		word = strtok (line," \n");
		while(word!=NULL)
		{
		//printf("Read this word: %s\n",word);
			if(words_in==table_size){
				previous_table=table_size;				
				table_size=table_size<<1;
				ptr_table = realloc(ptr_table,table_size*sizeof(char*));
				word_lens=realloc(word_lens,table_size*sizeof(int));
				for(a=previous_table;a<table_size;a++){
					ptr_table[a]=malloc(word_size*sizeof(char));
					word_lens[a]=word_size;
				}
			}
			word_len=strlen(word);
			if(word_len+1>word_lens[words_in]){
				while(word_len>word_lens[words_in]) word_lens[words_in]=word_lens[words_in]<<1;
				ptr_table[words_in] = realloc(ptr_table[words_in],(word_lens[words_in]+1)*sizeof(char));
			}
			//printf("words_in %d and word %s can fit %d\n",words_in,word,word_lens[words_in]);
			strcpy(ptr_table[words_in],word);
			words_in++;
			word=strtok(NULL," \n");
		}
		//append_trie_node_iterative(trie->root,ptr_table,0,words_in-1);
		//append_trie_node(trie->root,ptr_table,0,words_in-1);
		insertTrieNode(trie->hash,ptr_table,words_in,0);
	}
	free(word_lens);
	free(line);
	cleanup(ptr_table);
	fclose(fd);
	return static_flag;	
}




int test_input(struct index *trie,char * filename)
{

	int words_in = 0;
	int A_words_in = 0;
	int A_word_size=word_size;
	int A_table_size=table_size;
	int flag; //1 question, 2 addition, 3 deletion, 4 end of file
	int a;
	FILE* fd = fopen(filename, "r"); //opening input file

	if(fd == NULL)
	{
		perror("Error opening input file");
		return -1;
	}

	topk_threads *top;
	top=create_top_threads(top);
	top=init_top_threads(top);

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *word;
	int count=0;
	int previous_is_Q=1;
	int current_version=0;
	int k;
	int delete_batch=0;
	int word_len;
	//int length_array_capacity=10;
	int last_word=0;
	int lengths_taken=0;
	int threads_quantity  = 128 ;
	JobScheduler *JS = initialize_scheduler(threads_quantity);
	Job *job_to_append = malloc(sizeof(Job));
	int Q_number=0;
	int previous_table;

	char **ptr_table = malloc(table_size*sizeof(char *));
	char **A_ptr_table = malloc(A_table_size*sizeof(char *));
	int *word_lengths=malloc(table_size*sizeof(int));

	ngrams_to_delete *d_grams=malloc(sizeof(ngrams_to_delete));
	init_ngrams_to_delete(d_grams);

	for(a=0;a<table_size;a++){
		A_ptr_table[a]=malloc(A_word_size*sizeof(char));
		ptr_table[a]=malloc(word_size*sizeof(char));
		word_lengths[a]=word_size;
		}
	char ***pointer_to_words=malloc(sizeof(char**));
	*pointer_to_words=ptr_table;
	words_in = 0;
	while ((read = getline(&line, &len, fd)) != -1) {

		//words_in = 0;
		
		word = strtok (line," \n");

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
				words_in=0;
				word=strtok(NULL,"\n");
				int k;
				execute_all_jobs(JS);
				//execute_queries(trie->hash,ptr_table,Q_lengths,version,start,lengths_taken,top);
				lengths_taken=0;
				last_word=0;
				print_print_threads(top,Q_number);
				if(word!=NULL){
					count++;
					k=atoi(word);
					int total_ngrams=get_total_ngrams(top,Q_number);
					char **merged_ngrams=malloc(total_ngrams*sizeof(char*));
					merge_kframes_threads(top,Q_number,total_ngrams,merged_ngrams);
					create_threads_hashtable(top,total_ngrams);
					built_hashtable(top,merged_ngrams,total_ngrams);
					//print_merged(merged_ngrams,total_ngrams);
					print_top_threads(top,merged_ngrams,total_ngrams,k);
					erase_hashtable_from_top(top);
					free(merged_ngrams);	
				}
				top=init_top_threads(top);
				//print_ngrams_to_delete(d_grams);
				delete_ngrams(trie->hash,d_grams);
				reset_ngrams_to_delete(d_grams);
				delete_batch=0;
				Q_number=0;
				continue;
			}
			else{
				continue;
			}
			word=strtok(NULL," \n");
				
			if(flag==1){
				while(word!=NULL){

					if(words_in==table_size-1){
						previous_table=table_size;
						table_size=table_size<<1;
						ptr_table = realloc(ptr_table,table_size*sizeof(char*));
						*pointer_to_words=ptr_table;
						word_lengths = realloc(word_lengths,table_size*sizeof(int));
						if(ptr_table==NULL) exit(-1);
						for(a=previous_table;a<table_size;a++){
							ptr_table[a]=malloc(word_size*sizeof(char));
							if(ptr_table[a]==NULL) exit(-1);
							word_lengths[a] =word_size;
							}

					}
					word_len=strlen(word);
					if(word_len>=word_lengths[words_in]){
						while(word_len>=word_lengths[words_in]){  word_lengths[words_in]=word_lengths[words_in]<<1;}
						ptr_table[words_in] = realloc(ptr_table[words_in],word_lengths[words_in]*sizeof(char));
						}	
						strcpy(ptr_table[words_in],word);
						words_in++;				
						word=strtok(NULL," \n");
					
				}
					}
				else{
					A_words_in=0;
					while(word!=NULL){
						if(A_words_in==A_table_size-1){
							previous_table=A_table_size;
							A_table_size=A_table_size<<1;
							A_ptr_table = realloc(A_ptr_table,A_table_size*sizeof(char*));
							if(A_ptr_table==NULL) exit(-1);
							for(a=previous_table;a<(A_table_size);a++){
								A_ptr_table[a]=malloc(A_word_size*sizeof(char));
								if(A_ptr_table[a]==NULL) exit(-1);
								}
						}
						while(strlen(word)>=A_word_size){
							A_word_size=A_word_size<<1;
							for(a=0;a<A_table_size;a++){
								A_ptr_table[a] = realloc(A_ptr_table[a],A_word_size*sizeof(char));
								if(A_ptr_table[a]==NULL) exit(-1);
								}
						}
						strcpy(A_ptr_table[A_words_in],word);	
						A_words_in++;				
						word=strtok(NULL," \n");
						}
					}		

		switch(flag){
			case 1 :
				
				if(Q_number==top->Q_capacity){
					extend_top_threads(top,top->Q_capacity<<1);
				}
				//job_to_append= malloc(sizeof(Job));
				job_to_append->opt=(lookupTrieNode_with_bloom_versioning_threads);
				
				
				q_args* arguments = malloc(sizeof(q_args));
				arguments->hash = trie->hash;
				
				arguments->words = pointer_to_words;
				arguments->number_of_words = words_in-last_word-1;//Q_lengths[lengths_taken]-1;
				arguments->top = top;
				arguments->version = current_version;
				arguments->start = last_word;//start[lengths_taken];
				arguments->Q_number = Q_number;
				job_to_append->arguments= (void*)arguments;//(void*)ptr;
				submit_job(JS,job_to_append);

				last_word=words_in;
				lengths_taken++;
				Q_number++;
				//this
				//command_error=lookupTrieNode_with_bloom_versioning(trie->hash,ptr_table,words_in-1,top,current_version); //kfrm
								
				previous_is_Q=1;

				break;
			case 2 :
				if(previous_is_Q==1){
					current_version++;}
				previous_is_Q=0;
	
				insertTrieNode(trie->hash,A_ptr_table,A_words_in,current_version);
				A_words_in=0;			
				break;
			case 3 :

				for(k=0;k<A_words_in;k++) add_ngram_to_delete(d_grams,A_ptr_table[k],delete_batch);
				delete_batch++;
				if(previous_is_Q==1) current_version++;

				previous_is_Q=0;
				deleteTrieNode_versioning(trie->hash,A_ptr_table,A_words_in,current_version);
				A_words_in=0;			
				break;
		
		}
		flag=0;	
}

	destroy_ngrams_to_delete(d_grams);
	free(d_grams);
	free(job_to_append);
  	free(line);
	free(word_lengths);
	free(pointer_to_words);
	erase_top_threads(top);
	destroy_scheduler(JS);
	cleanup_A(A_ptr_table,A_table_size);
	cleanup(ptr_table);
	fclose(fd);
	
return 0;

}

int execute_queries(hash_layer *hash,char **ptr_table,int *ptr_lengths,int *version,int *start,int pos,topk *top){
	int i;
	for(i=0;i<pos;i++){	
		lookupTrieNode_with_bloom_versioning(hash,ptr_table,ptr_lengths[i]-1,top,version[i],start[i]);
	}
	return 0;
}

void cleanup(char ** ptr){
	int a;
	for(a=0;a<table_size;a++){
		free(ptr[a]);
	}
	free(ptr);
}	

void cleanup_A(char ** ptr,int A_table_size){
	int a;
	for(a=0;a<A_table_size;a++){
		free(ptr[a]);
	}
	free(ptr);
}

void print_node(trie_node *node){
	printf("node word is %s\n",node->word);
	printf("node has %d children\n",node->number_of_childs);
	printf("node is final: %c\n",node->is_final);
}

void print_trie(trie_node *node,int level){
	int i;
	if(node->number_of_childs==0){printf("\n"); return;}
	//printf("In level %d : \n",level );
	for(i=0;i<node->number_of_childs;i++){
		//print_node(&(node->children[i]));
		if(level!=0) printf("->");
		printf("%s (%c) ",node->children[i].word,node->children[i].is_final);
		print_trie(&(node->children[i]),level+1);
	}
	return;
}

trie_node *init_trie(){
	trie_node *root=malloc(sizeof(trie_node));
	root->word=NULL;
	root->is_final='n';
	root->number_of_childs=0;	
	root->max_childs=MAX_CHILDS;
	
	root->children=malloc(CHILD_NUM*sizeof(struct trie_node));

	return root;
}

void destroy_childs(trie_node *node){
	int i;
	for(i=0;i<node->number_of_childs;i++){
		destroy_childs(&(node->children[i]));
		free(node->children[i].word);	
	}
	free(node->children);
}
void delete_trie(struct index *trie){
	destroy_childs(trie->root);
	free(trie->root);
	free(trie);

}

trie_node *create_trie_node(char *word,char is_final){
	trie_node *node=malloc(sizeof(trie_node));
	if(strlen(word)+2>WORD_SIZE){
		node->word=malloc((strlen(word)+2)*sizeof(char));
		strcpy(node->word,word);
		//memset(node->static_word,0,WORD_SIZE);
	}
	else{
		//strcpy(node->static_word,word);
		//node->word=node->static_word;
	}
	node->is_final=is_final;
	node->number_of_childs=0;
	node->max_childs=MAX_CHILDS;


	node->children=malloc(CHILD_NUM*sizeof(struct trie_node));

	return node;
}

trie_node *init_trie_node(trie_node *node,char *word,char is_final,int current_version){
	node->word=malloc((strlen(word)+1)*sizeof(char));
	//node->word=malloc(WORD_SIZE*sizeof(char));
	strcpy(node->word,word);
	
	node->is_final=is_final;
	node->number_of_childs=0;
	node->max_childs=MAX_CHILDS;

	node->children=malloc(CHILD_NUM*sizeof(struct trie_node));
	node->A_version=current_version;
	node->D_version=-1;
	//printf("I added in node %s version %d , %d\n",node->word,node->A_version,node->D_version);
	node->children_deleted=0;
	

	return node;
}

int append_trie_node(trie_node *root,char **word,int word_number,int number_of_words,int current_version){
	int error;
	if(word_number>number_of_words){
		root->A_version=current_version;
		return SUCCESS;
		}
	char is_final='n';
	if(word_number==number_of_words) is_final='y';

	if(root->number_of_childs==0){
		error=append_word(root,0,word[word_number],is_final,current_version);
		if(error==ERROR) return ERROR;
		root->number_of_childs++;
		append_trie_node(&(root->children[0]),word,word_number+1,number_of_words,current_version);
	}
	else{
		int pos;
		int exists=check_exists_in_children(root,word[word_number],&pos);
		if (exists==1){
			if((&(root->children[pos]))->is_final!='y') (&(root->children[pos]))->is_final=is_final; //this change
			append_trie_node(&(root->children[pos]),word,word_number+1,number_of_words,current_version);
			}
		else{	
			error=append_word(root,pos,word[word_number],is_final,current_version);
			if(error==ERROR) return ERROR;
			root->number_of_childs++;
			append_trie_node(&(root->children[pos]),word,word_number+1,number_of_words,current_version);
			}
	}
	return SUCCESS; //0 errors
}

int append_trie_node_iterative(trie_node *root,char **word,int word_number,int number_of_words,int current_version){
	int error;
	char is_final;
	trie_node *node=root;
	int pos,exists;
	for(word_number=0;word_number<=number_of_words;word_number++){
		is_final='n';
		if(word_number==number_of_words) is_final='y';
		
		if(node->number_of_childs==0){
			error=append_word(node,0,word[word_number],is_final,current_version);
			if(error==ERROR) return ERROR;
			node->number_of_childs++;
			node=&(node->children[0]);
		}
		else{
			exists=check_exists_in_children(node,word[word_number],&pos);
			if (exists==1){
				if((&(node->children[pos]))->is_final!='y') (&(node->children[pos]))->is_final=is_final; //this change
				node=&(node->children[pos]);
				}
			else{	
				error=append_word(node,pos,word[word_number],is_final,current_version);
				if(error==ERROR) return ERROR;
				node->number_of_childs++;
				node=&(node->children[pos]);
				}
			}
				
	}
	if(word_number>number_of_words){
		//printf("out of words to add\n");
		return SUCCESS;
		}


	
	return SUCCESS; //0 errors
}


int check_exists_in_children(trie_node *node,char *word,int *pos){
		int pivot=0; //pivor is integer so in the division it will rounf in the smaller absolute value 5/2=2
		int lower=0;
		int upper=node->number_of_childs-1;
		int compare;

		if(upper==-1) return 0; //i made this change
		//compare=strcmp(node->children[0].word,word); // equal=0 children[i]<word: compare<0 children>word : compare>0
		//if(compare==0) {
			//*pos=0;
			//return 1; //exact match
			//}
		while(1!=0){
			if(upper<=lower){
				pivot=(upper+lower)/2;
				//pivot=(upper+lower)>>1;
				compare=strcmp(node->children[pivot].word,word);
				if(compare==0){
					*pos=pivot;
					return 1; //exact match
				}
				*pos=(compare<0)? pivot+1:pivot; //lower+1:lower
				return 0; //not exact match
				}
			else {
				//pivot=(upper+lower)/2;
				pivot=(upper+lower)>>1;
				compare=strcmp(node->children[pivot].word,word); // equal=0 children[i]<word: compare<0 children>word : compare>0
				if(compare==0) {
					*pos=pivot;
					return 1; //exact match
				}
				else if(compare>0) upper=pivot-1;
				else lower=pivot+1;
			}
		}
}

int append_word(trie_node *node,int pos,char *word,char is_final,int current_version){
		//printf("inside append_word , pos %d\n",pos);
		if(node->number_of_childs==node->max_childs){
				//printf("I have to double the children\n");
				node->max_childs=node->max_childs<<1;
				node->children=realloc(node->children,node->max_childs*sizeof(trie_node));
				//printf("Done Realloc \n");
				if(node->children==NULL){
					node->max_childs=node->max_childs>>1;	
					return ERROR;
				}
		}

		trie_node * backup=node->children;
		memmove(node->children,backup,pos*sizeof(trie_node));
		memmove(node->children+(pos+1),backup+(pos),(node->number_of_childs-pos)*sizeof(trie_node));
		init_trie_node(&(node->children[pos]),word,is_final,current_version);
		return SUCCESS ; //zero errors
}

int delete_ngram(trie_node *root,char **word,int word_number,int number_of_words){
		int error;
		if(word_number==number_of_words+1){
			if(root->number_of_childs!=0 && root->is_final!='y') return ERROR;
			if(root->number_of_childs!=0 &&root->is_final=='y'){
				root->is_final='n';
				return 2;}  
			return SUCCESS;//and return no error . the previous one is gonna delete it	
		}
		if(root->number_of_childs==0) return ERROR;
		else{
			int pos;
			int exists=check_exists_in_children(root,word[word_number],&pos);
			if (exists==1){
				error=delete_ngram(&(root->children[pos]),word,word_number+1,number_of_words);
				if(error==0)
				{
					if(root->children[pos].is_final=='y' && word_number!=number_of_words ) return 2; //return 2 if  i am del half 
					error=delete_from_node(root,pos);
					root->number_of_childs--;
				}
				if(error!=1 && root->number_of_childs!=0) return 2; //dont delete the node if it has more childs 
				return error; 
			}
			else return ERROR;	//return error if the word is not on the trie , so the ngram is not in the trie
			}
}



int delete_from_node(trie_node *node,int pos){
		//printf("in delete node pos %d word to delete is %s\n",pos,node->children[pos].word);

		trie_node * backup=node->children;
		trie_node *node_to_delete=&(node->children[pos]);
		destroy_childs(node_to_delete);
		free(node_to_delete->word);
		memmove(node->children,backup,pos*sizeof(trie_node));
		memmove(node->children+pos,backup+pos+1,(node->number_of_childs-(pos+1))*sizeof(trie_node));
		return SUCCESS ; //zero e-rrors
}


void print_nodes_from_stack(trie_node *root,stack *stack_){
	int number=get_stack_number(stack_);
	int i ,pos;
	trie_node *node=root;
	for(i=0;i<number-1;i++){
		pos=get_stack_elements(stack_,i);
		node=&(node->children[pos]);
		printf("%s ",node->word);
	}
	pos=get_stack_elements(stack_,i);
	node=&(node->children[pos]);
	printf("%s",node->word);
	printf(YELLOW"|"RESET);
}

void print_nodes_from_hash(hash_layer *hash,stack *stack_){
	int number=get_stack_number(stack_);
	int i ,pos;
	trie_node *node;
	pos=get_stack_elements(stack_,0);
	hash_bucket *bucket=&(hash->buckets[pos]);
	pos=get_stack_elements(stack_,1);
	node=&(bucket->children[pos]);
	if(number==2){
		printf("%s|",node->word);
		return;
	}
	printf("%s ",node->word); 
	for(i=2;i<number-1;i++){
		pos=get_stack_elements(stack_,i);
		node=&(node->children[pos]);
		printf("%s ",node->word);
	}
	
	pos=get_stack_elements(stack_,i);
	node=&(node->children[pos]);
	printf("%s",node->word);
	printf("|");
}



char * myappend(char * string, char * word){
	if(string==NULL) 
		string = malloc(0);
	size_t new_length = strlen(string)+strlen(word)+2;
	char *string2;
	string2 = malloc(new_length);
	strcpy(string2,string);
	strcat(string2," ");
	strcat(string2,word);
	free(string);

	if(string2[0] == ' ') //lets chop off the first space in each n gram
	{
		int i;
		for(i=0;i<strlen(string2);i++)
			string2[i]=string2[i+1];
	}
	return string2;

}

void  myappend_pan(char **string,int *str_size, char * word){

	int string_len=strlen(*string);
	int word_len=strlen(word);

	if((*str_size)<=string_len+word_len+1){
		//*str_size=*str_size*2*((string_len+word_len+1)/ *str_size);
		(*str_size)=(*str_size)<<1;
		while(*str_size<=string_len+word_len+1) *str_size=(*str_size)*2;

		*string=realloc(*string, (*str_size)*sizeof(char));
	}
	strcat(*string,word);
	

}

void  myappend_pan_with_space(char **string,int *str_size, char * word){
	
	int string_len=strlen(*string);
	int word_len=strlen(word);

	if((*str_size)<=string_len+word_len+1){

		*str_size=*str_size*2*((string_len+word_len+1)/ *str_size);
		*string=realloc(*string, (*str_size)*sizeof(char));
	}

	strcat(*string," ");
	strcat(*string,word);
	
	return;
}

void initialize_bucket(hash_bucket *bucket,int m){
	bucket->children=malloc(m*sizeof(trie_node));
	bucket->children_number=0;
	bucket->capacity=m;
}

hash_layer *createLinearHash(int c ,int m){ //c is number of buckets ,m is number of cells per bucket
	int i;
	hash_layer *hash=malloc(sizeof(hash_layer));
	if(hash==NULL) return NULL;

	hash->bucket_capacity=m;
	hash->buckets_number=c;
	hash->load_factor=0.9;
	hash->total_children=0;
	hash->bucket_to_split=0;
	hash->split_round=0;
	hash->buckets=malloc(c*sizeof(hash_bucket));
	if(hash->buckets==NULL) return NULL;
	
	for(i=0;i<c;i++){
		initialize_bucket(&(hash->buckets[i]),m);
	}
	return hash;
}


int  hash_function(hash_layer *hash, char *word)
{	
	int hash_value;
    unsigned long hash_int = 5381;
    int c;
    while (c = *word++)
        hash_int = ((hash_int << 5) + hash_int) + c; /* hash * 33 + c */
	
	hash_value=hash_int%(C<<hash->split_round);
	//hash_value=hash_int%(C*(int)pow(2,hash->split_round));

	int temp=hash_int%(C<<(hash->split_round+1));
	//int temp=hash_int%(C*(int)pow(2,hash->split_round+1));
	if(temp<hash->buckets_number) return temp;
    return hash_value;
}


void destroyLinearHash(hash_layer *hash){
	int i;
	hash_bucket *bucket;
	for(i=0;i<hash->buckets_number;i++){
		bucket=&(hash->buckets[i]);
		destroy_bucket_nodes(bucket);
	}
	free(hash->buckets);
	free(hash);
	return ;
}

void destroy_bucket_nodes(hash_bucket *bucket){
	trie_node *node;
	int j;
	for(j=0;j<bucket->children_number;j++){
		node=&(bucket->children[j]);
		destroy_childs(node);
		free(node->word);
		//free(node);
		}
	free(bucket->children);
}

int insertTrieNode(hash_layer *hash,char **words,int word_number,int current_version){
	//printf("appending %s\n",words[0]);
	char is_final='n';
	if(word_number==1) is_final='y';

	if((hash->total_children/((float)hash->buckets_number*hash->bucket_capacity)) > hash->load_factor){
		int resize_error=resize_hash(hash);
		if(resize_error==ERROR) return ERROR;
	}
	
	int hash_val=hash_function(hash,words[0]);
	

	trie_node *node;
	node=add_to_backet(hash,hash_val,words[0],is_final,current_version); //node gets the value of the first trie node

	if(word_number>1){ 
		append_trie_node(node,words,1,word_number-1,current_version);
		//node->number_of_childs++;
		//node->A_version=current_version;
		return 0;	
	}	
	
	if(node->is_final=='n') node->is_final=is_final;
	node->A_version=current_version;
	return 0;
}


/*int check_exists_in_bucket(hash_bucket *bucket,char *word,int *pos){
		int pivot=0; //pivor is integer so in the division it will rounf in the smaller absolute value 5/2=2
		int lower=0;
		if(bucket->children_number==0) {
			*pos=0;
			return 0;
		}
		int upper=bucket->children_number-1;
		//printf("inside check exists\n");
		//if(upper==-1) return 0; //i made this change
		while(1!=0){
			//printf("upper %d lower %d pivot %d\n",upper,lower,pivot);
			if(upper<=lower){
				pivot=(upper+lower)/2;
				int compare=strcmp(bucket->children[pivot].word,word);
				if(compare==0){
					*pos=pivot;
					return 1; //exact match
				}
				//printf("compare is %d\n",compare);	
				*pos=(compare<0)? pivot+1:pivot; //lower+1:lower
				return 0; //not exact match
				}
			else {
				pivot=(upper+lower)/2;
				int compare=strcmp(bucket->children[pivot].word,word); // equal=0 children[i]<word: compare<0 children>word : compare>0
				if(compare==0) {
					*pos=pivot;
					return 1; //exact match
				}
				else if(compare>0) upper=pivot-1;
				else lower=pivot+1;
			}
		}
}*/

int check_exists_in_bucket(char *word,int *pos,trie_node *children,int children_number){
		int pivot=0; //pivor is integer so in the division it will rounf in the smaller absolute value 5/2=2
		int lower=0;
		int compare;
		if(children_number==0) {
			*pos=0;
			return 0;
		}
		int upper=children_number-1;
		if(upper==-1) return 0; //i made this change
		while(1!=0){
			//printf("upper %d lower %d pivot %d\n",upper,lower,pivot);
			if(upper<=lower){
				pivot=(upper+lower)/2;
				compare=strcmp(children[pivot].word,word);
				if(compare==0){
					*pos=pivot;
					return 1; //exact match
				}
				//printf("compare is %d\n",compare);	
				*pos=(compare<0)? pivot+1:pivot; //lower+1:lower
				return 0; //not exact match
				}
			else {
				//pivot=(upper+lower)/2;
				pivot=(upper+lower)>>1;
				compare=strcmp(children[pivot].word,word); // equal=0 children[i]<word: compare<0 children>word : compare>0
				if(compare==0) {
					*pos=pivot;
					return 1; //exact match
				}
				else if(compare>0) upper=pivot-1;
				else lower=pivot+1;
			}
		}
}



trie_node* add_to_backet(hash_layer *hash,int hash_val,char *word,char is_final,int current_version){
	//printf("In add to bucket \"%s\"\n",word);
	
	
	hash_bucket *bucket=&(hash->buckets[hash_val]);
	
	int *last=&(bucket->children_number);
	trie_node *node;


	if(*last==bucket->capacity){ //initializing overflow bucket
		bucket->children=realloc(bucket->children,2*bucket->children_number*sizeof(trie_node));
		if(bucket->children==NULL) return NULL;
		bucket->capacity=bucket->capacity<<1;
		//printf("I made an overflow at bucket %d\n",hash_val);
	} 

	int pos;
	//int exists=check_exists_in_bucket(bucket,word,&pos);
	int exists=check_exists_in_bucket(word,&pos,bucket->children,bucket->children_number);
	node=&(bucket->children[pos]);
	if(exists==1){
		//node->A_version=current_version;
		//printf("exists"); 
		return node;
	}
	//memove nodes to the right
	trie_node *backup=bucket->children;
	memmove(bucket->children,backup,pos*sizeof(trie_node));
	memmove(bucket->children+(pos+1),backup+(pos),(bucket->children_number-pos)*sizeof(trie_node));//
	init_trie_node(node,word,is_final,current_version);

	*last=*last+1;
	hash->total_children++;
	return node;
}

int resize_hash(hash_layer *hash){
	hash_bucket *bucket;
	//printf("attempting to expand buckets\n");
	hash->buckets=realloc(hash->buckets,(hash->buckets_number+1)*sizeof(hash_bucket)); //add bucket lineat
	if(hash->buckets==NULL){
		printf("error in realloc\n");
		return ERROR;
	}
	hash->buckets_number++;
	//initialize bucket
	initialize_bucket(&(hash->buckets[hash->buckets_number-1]),hash->bucket_capacity);
	int i;
	hash_bucket *new_bucket=&(hash->buckets[hash->buckets_number-1]); //pointer to the new bucket
	bucket=&(hash->buckets[hash->bucket_to_split]); //re arranging bucket to split
	int new_hash_val=-1;
	stack *stack_=init_stack();
	//print_stack(stack_);

	if(bucket->children_number==0){
		hash->bucket_to_split=(hash->bucket_to_split+1)%(C<<hash->split_round);//without pow
		//hash->bucket_to_split=(hash->bucket_to_split+1)%(C*(int)pow(2,hash->split_round));//
		if(hash->bucket_to_split==0 && hash->buckets_number>C) hash->split_round++;
		stack_destroy(stack_); 
		return SUCCESS;
	} // no need for rearranging bucket
	
	int previous=hash->bucket_to_split;
	hash->bucket_to_split=(hash->bucket_to_split+1)%(C<<hash->split_round);// without pow
 	//hash->bucket_to_split=(hash->bucket_to_split+1)%(C*(int)pow(2,hash->split_round));//
	for(i=0;i<bucket->children_number;i++){

		new_hash_val=hash_function(hash,bucket->children[i].word);
		if(new_hash_val==previous) continue; //if hash value is the same then no need to change bucket
		if(new_bucket->children_number==new_bucket->capacity){	//if new bucket fills then create an overflow bucket
			new_bucket->children=realloc(new_bucket->children,bucket->capacity*2*sizeof(trie_node));
			if(new_bucket->children==NULL){
				stack_destroy(stack_); 
				return ERROR;
			}
			new_bucket->capacity=new_bucket->capacity<<1;	
		}
		memmove(&(new_bucket->children[new_bucket->children_number]),&(bucket->children[i]),sizeof(trie_node)); //copy nod
		push(stack_,i);		
		new_bucket->children_number++;	
	}

	//print_stack(stack_);
	shrink_buckets(&(hash->buckets[previous]),stack_);
	if(hash->bucket_to_split==0 && hash->buckets_number>C) hash->split_round++;
	stack_destroy(stack_); 
	return SUCCESS;
}

void shrink_buckets(hash_bucket *bucket,stack *stack_){
	int i;
	int pos;
	int *total=&(bucket->children_number);
	for(i=stack_->top-1;i>=0;i--){
		pos=get_stack_elements(stack_,i);
		memmove(&(bucket->children[pos]),&(bucket->children[pos+1]),(*total-(pos+1))*sizeof(trie_node));
		*total=*total-1;
		}
}

int deleteTrieNode(hash_layer *hash,char **words,int word_number){
	int error;

	int hash_val=hash_function(hash,words[0]);
	//printf("hash val is %d\n",hash_val);

	trie_node *node;
	int pos;
	node=delete_from_backet(hash,hash_val,words[0],&pos); //node gets the value of the first trie node
	if(node==NULL) return ERROR;
	if(word_number>1){ 
		//printf("deleting more words\n");
		error=delete_ngram(node,words,1,word_number-1);
		if(error!=SUCCESS){
			return error;
		}
	
		if(node->number_of_childs!=0 || node->is_final=='y') return 2;
		hash_bucket *bucket=&(hash->buckets[hash_val]);
		int total=bucket->children_number;
		free(node->word);
		free(node->children);
		memmove(&(bucket->children[pos]),&(bucket->children[pos+1]),(total-(pos+1))*sizeof(trie_node));
		bucket->children_number--;
		return SUCCESS;
	}
	if(node->is_final=='n') return ERROR;
	if(node->number_of_childs!=0){ 
		node->is_final='n';
		return 2;
	}

	hash_bucket *bucket=&(hash->buckets[hash_val]);
	int total=bucket->children_number;
	free(node->word);
	free(node->children);
	memmove(&(bucket->children[pos]),&(bucket->children[pos+1]),(total-(pos+1))*sizeof(trie_node));
	bucket->children_number--;
	hash->total_children--;
	
	
	return SUCCESS;
}

trie_node *delete_from_backet(hash_layer *hash,int hash_val,char *word,int *pos){
	hash_bucket *bucket=&(hash->buckets[hash_val]);

	trie_node *node;
	//int exists=check_exists_in_bucket(bucket,word,pos);
	int exists=check_exists_in_bucket(word,pos,bucket->children,bucket->children_number);
	//printf("exists in delete is %d and word is %s and hash_val is %d\n",exists,word,hash_val);
	if(exists==0) return NULL;
	node=&(bucket->children[*pos]);
	return node;
}

void print_hash(hash_layer *hash){
	int i,j;
	hash_bucket bucket;
	trie_node node;
	for(i=0;i<hash->buckets_number;i++){
		bucket=hash->buckets[i];
		printf("Bucket[%d]::",i);
		for(j=0;j<bucket.children_number;j++){
			node=bucket.children[j];
			printf("-%s(%c)->",node.word,node.is_final);
			print_trie(&node,0);
		}
		printf("\n");
	}
}



int lookupTrieNode_with_bloom(hash_layer *hash,char **words,int number_of_words,topk * top){
	size_t bloomfilterbytes=M*8;
	int * bloomfilter = malloc(bloomfilterbytes/8);
	bloomfilter_init(bloomfilter,bloomfilterbytes);

	char *str;	
	int str_size;
	int word_number;
	int exists;
	int pos;
	trie_node *node;
	int start=0;
	int ngrams_found=0;
	while(start!=number_of_words+1) {

		str=malloc(20*sizeof(char)); //check this size
		strcpy(str,"");
		str_size=20;
		word_number=start;
		
		int hash_val=hash_function(hash,words[start]);

		hash_bucket *bucket=&(hash->buckets[hash_val]);
		exists=check_exists_in_bucket(words[start],&pos,bucket->children,bucket->children_number);
		if(exists==0){ 
			start++;
			free(str);
			continue;
			}

		node=&(bucket->children[pos]);
		myappend_pan(&str,&str_size,words[start]);
		
		while(node->number_of_childs!=0) {
			
			if(node->is_final=='y') { //found ngram
				if(bloomfilter_check(str,bloomfilter,bloomfilterbytes)==0){
						bloomfilter_add(str,bloomfilter,bloomfilterbytes);
						top=add_top(top,str);
						ngrams_found++;
					}
				}
			word_number++;
			if(word_number>number_of_words) break;
			exists=check_exists_in_children(node,words[word_number],&pos);
			if(exists==0) break;

			myappend_pan_with_space(&str,&str_size,words[word_number]);
			node=&(node->children[pos]);
		}
		if(exists==1 && word_number<=number_of_words) {
			if(bloomfilter_check(str,bloomfilter,bloomfilterbytes)==0){
						bloomfilter_add(str,bloomfilter,bloomfilterbytes);
						top=add_top(top,str);
						ngrams_found++;
			}
		}
		free(str);
		start++;
	}
	end_gram_table(top,ngrams_found);
	int found=SUCCESS;
	//free(str);
	free(bloomfilter);	
	return found;
	if(TestAllBits(bloomfilter,bloomfilterbytes)==0) found=-1;
	if(exists==0) return ERROR;
	
	return SUCCESS;	


	return 0;
}


char * detableize(char * str, char ** table){
	int i;
	for (i=0;i<table_size;i++)
		str = myappend(str,table[i]);

return str;
}


//-------------------------------------versioning delete------------------------------//
int delete_ngram_versioning(trie_node *root,char **word,int word_number,int number_of_words,int current_version){
		int error;
		//printf("in delete ngram word \"%s\"\n",word[word_number]);
		if(word_number==number_of_words+1){
			if(root->number_of_childs!=0 && root->is_final!='y') return ERROR;
			if(root->number_of_childs-root->children_deleted!=0 && root->is_final=='y'){
				//root->is_final='n';
				//printf("The node here is %s\n",root->word);
				root->D_version=current_version;
				return 2;}  
			return SUCCESS;//and return no error . the previous one is gonna delete it	
		}
		if(root->number_of_childs==0) return ERROR;
		else{
			int pos;
			//printf("before exists \"%s\"\n",word[word_number]);
			int exists=check_exists_in_children(root,word[word_number],&pos);
			//printf("exists :%d ",exists);
			if (exists==1){
				error=delete_ngram_versioning(&(root->children[pos]),word,word_number+1,number_of_words,current_version);
				if(error==0)
				{
					if(root->children[pos].is_final=='y' && word_number!=number_of_words ) return 2; //return 2 if  i am del half 
					//printf("parent node before deleting is %s\n",root->word);
					error=delete_from_node_versioning(root,pos,current_version);
					//root->number_of_childs--;
				}
				if(error!=1 && (root->number_of_childs - root->children_deleted)!=0) return 2; 
				return error; 
			}
			else return ERROR;	//return error if the word is not on the trie , so the ngram is not in the trie
			}
}



int delete_from_node_versioning(trie_node *node,int pos,int current_version){
		trie_node *node_to_delete=&(node->children[pos]);
		node_to_delete->D_version=current_version;
		//node_to_delete->is_final='n';

		//printf("word i changed version is %s\n",node_to_delete->word);
		node->children_deleted++;
		return SUCCESS ; //zero e-rrors
}

int deleteTrieNode_versioning(hash_layer *hash,char **words,int word_number,int current_version){
	int error;

	int hash_val=hash_function(hash,words[0]);
	//printf("hash val is %d\n",hash_val);

	trie_node *node;
	int pos;
	node=delete_from_backet_versioning(hash,hash_val,words[0],&pos); //node gets the value of the first trie node
	if(node==NULL) return ERROR;
	if(word_number>1){ 
		error=delete_ngram_versioning(node,words,1,word_number-1,current_version);
		if(error==ERROR){
			//printf("not succeded %d\n",error);
			return error;
		}
		//node->D_version=current_version; //check that
		if(node->number_of_childs - node->children_deleted!=0 || node->is_final=='y') return 2;
		node->D_version=current_version; //check that
		return SUCCESS;
	}
	if(node->is_final=='n') return ERROR;
	if(node->number_of_childs-node->children_deleted!=0){
		//node->is_final='n';
		node->D_version=current_version;
		return 2;
	}
	
	//node->is_final='n';
	node->D_version=current_version;
	
	
	return SUCCESS;
}

trie_node *delete_from_backet_versioning(hash_layer *hash,int hash_val,char *word,int *pos){
	hash_bucket *bucket=&(hash->buckets[hash_val]);

	trie_node *node;
	//int exists=check_exists_in_bucket(bucket,word,pos);
	int exists=check_exists_in_bucket(word,pos,bucket->children,bucket->children_number);
	//printf("exists in delete is %d and word is %s and hash_val is %d\n",exists,word,hash_val);
	if(exists==0) return NULL;
	node=&(bucket->children[*pos]);
	return node;
}

//-------------------------------------versioning delete with clean up------------------------------//
int delete_ngram_versioning_cleanup(trie_node *root,char **word,int word_number,int number_of_words,int current_version){
		int error;
		//printf("in delete ngram word \"%s\"\n",word[word_number]);
		if(word_number==number_of_words+1){
			if(root->number_of_childs!=0 && root->is_final!='y') return ERROR;
			if(root->number_of_childs-root->children_deleted!=0 &&root->is_final=='y'){
				root->is_final='n';
				//printf("The node here is %s\n",root->word);
				root->D_version=current_version;
				return 2;}  
			return SUCCESS;//and return no error . the previous one is gonna delete it	
		}
		if(root->number_of_childs==0) return ERROR;
		else{
			int pos;
			int exists=check_exists_in_children(root,word[word_number],&pos);
			if (exists==1){
				error=delete_ngram_versioning_cleanup(&(root->children[pos]),word,word_number+1,number_of_words,current_version);
				if(error==0)
				{
					if(root->children[pos].is_final=='y' && word_number!=number_of_words ) return 2; //return 2 if  i am del half 
					error=delete_from_node_versioning_cleanup(root,pos,current_version);
					//root->number_of_childs--;
				}
				if(error!=1 && (root->number_of_childs - root->children_deleted)!=0) return 2; 
				return error; 
			}
			else return ERROR;	//return error if the word is not on the trie , so the ngram is not in the trie
			}
}



int delete_from_node_versioning_cleanup(trie_node *node,int pos,int current_version){
		trie_node *node_to_delete=&(node->children[pos]);
		node_to_delete->D_version=current_version;
		node_to_delete->is_final='n';
		//printf("word i changed version is %s\n",node_to_delete->word);
		node->children_deleted++;
		return SUCCESS ; //zero e-rrors
}

int deleteTrieNode_versioning_cleanup(hash_layer *hash,char **words,int word_number,int current_version){
	int error;

	int hash_val=hash_function(hash,words[0]);

	trie_node *node;
	int pos;
	node=delete_from_backet_versioning_cleanup(hash,hash_val,words[0],&pos); //node gets the value of the first trie node
	if(node==NULL) return ERROR;
	if(word_number>1){ 
		error=delete_ngram_versioning_cleanup(node,words,1,word_number-1,current_version);
		if(error==ERROR){
			return error;
		}
		if(node->number_of_childs - node->children_deleted!=0 || node->is_final=='y') return 2;
		node->D_version=current_version; //check that
		return SUCCESS;
	}
	if(node->is_final=='n') return ERROR;
	if(node->number_of_childs-node->children_deleted!=0){
		node->is_final='n';
		node->D_version=current_version;
		return 2;
	}
	
	node->is_final='n';
	node->D_version=current_version;
	
	
	return SUCCESS;
}

trie_node *delete_from_backet_versioning_cleanup(hash_layer *hash,int hash_val,char *word,int *pos){
	hash_bucket *bucket=&(hash->buckets[hash_val]);

	trie_node *node;
	//int exists=check_exists_in_bucket(bucket,word,pos);
	int exists=check_exists_in_bucket(word,pos,bucket->children,bucket->children_number);
	//printf("exists in delete is %d and word is %s and hash_val is %d\n",exists,word,hash_val);
	if(exists==0) return NULL;
	node=&(bucket->children[*pos]);
	return node;
}
//-----------------------------------------versioning lookup--------------------------------/

int check_node(trie_node *node,int current_version){
		//printf("A version %d ,  D _ version %d, current version %d\n",node->A_version,node->D_version,current_version);
		if(node->D_version==-1){
			if(node->A_version>current_version) return ERROR;
			return SUCCESS;
		}
		
		if(node->A_version>current_version || (node->D_version<=current_version && node->D_version>node->A_version)){
			//printf("Error\n");
			return ERROR;
		} 
		return SUCCESS;
}

int lookupTrieNode_with_bloom_versioning(hash_layer *hash,char **words,int number_of_words,topk* top,int current_version,int section_start)
{
	size_t bloomfilterbits =bloomfiltersize(number_of_words); 
	size_t bloomfilterbytes = bloomfilterbits/8;
	int * bloomfilter = malloc(bloomfilterbytes);
	bloomfilter_init(bloomfilter,bloomfilterbytes);

	char *str;	
	int str_size;
	int word_number;
	int exists;
	int pos;
	trie_node *node;
	int start=section_start;
	int ngrams_found=0;
	while(start!=section_start+number_of_words+1) {
		str=malloc(20*sizeof(char)); //check this size
		strcpy(str,"");
		str_size=20;
		word_number=start;
		int check;
		int hash_val=hash_function(hash,words[start]);

		hash_bucket *bucket=&(hash->buckets[hash_val]);
		//exists=check_exists_in_bucket(bucket,words[start],&pos);
		exists=check_exists_in_bucket(words[start],&pos,bucket->children,bucket->children_number);
		if(exists==0){ 
			start++;
			free(str);
			continue;
			}
		node=&(bucket->children[pos]);
		myappend_pan(&str,&str_size,words[start]);
		
		while(node->number_of_childs!=0) {
			check=check_node(node,current_version);
			if(node->is_final=='y' && check!=ERROR) { 
				if(bloomfilter_check(str,bloomfilter,bloomfilterbytes)==0){
						bloomfilter_add(str,bloomfilter,bloomfilterbytes);
						//top=add_top_threads(top,str,Q_number);
						top=add_top(top,str);
						ngrams_found++;
					}
				}
			word_number++;
			if(word_number>number_of_words+section_start) break;
			exists=check_exists_in_children(node,words[word_number],&pos);
			if(exists==0) break;
			node=&(node->children[pos]);			
			myappend_pan_with_space(&str,&str_size,words[word_number]);
		}

		if(exists==1 && word_number<=section_start+number_of_words) {
			check=check_node(node,current_version);
			if(check!=ERROR){
			if(bloomfilter_check(str,bloomfilter,bloomfilterbytes)==0){
						bloomfilter_add(str,bloomfilter,bloomfilterbytes);
						//top=add_top_threads(top,str,Q_number);
						top=add_top(top,str);
						ngrams_found++;
			}
			}
		}
		free(str);
		start++;
	}
	end_gram_table(top,ngrams_found);
	int found=SUCCESS;
	//free(str);
	free(bloomfilter);
	//free(*data);	
	return found;
	if(TestAllBits(bloomfilter,bloomfilterbytes)==0) found=-1;
	if(exists==0) return ERROR;
	
	return SUCCESS;	


	return 0;
}

int lookupTrieNode_with_bloom_versioning_threads(void ** arguments)//hash_layer *hash,char **words,int number_of_words,topk * top,int current_version)
{
	q_args *data=(q_args*)arguments;
	hash_layer *hash=data->hash;
	char **words=*(data->words);
	int number_of_words=data->number_of_words;
	topk_threads *top=data->top;
	int current_version=data->version;
	int section_start=data->start;
	int Q_number=data->Q_number;
	//size_t bloomfilterbytes = ((M*128)/8);
	size_t bloomfilterbits =bloomfiltersize(number_of_words); 
	size_t bloomfilterbytes = bloomfilterbits/8;
	int * bloomfilter = malloc(bloomfilterbytes);
	bloomfilter_init(bloomfilter,bloomfilterbytes);

	char *str;	
	int str_size;
	int word_number;
	int exists;
	int pos;
	trie_node *node;
	int start=section_start;
	int ngrams_found=0;
	while(start!=section_start+number_of_words+1) {
		//printf("word %d is %s start %d\n",word_number,words[start],start);
		str=malloc(20*sizeof(char)); //check this size
		strcpy(str,"");
		str_size=20;
		word_number=start;
		int check;
		//printf("word %d is %s\n",word_number,words[start]);
		//exit(-1);
		int hash_val=hash_function(hash,words[start]);

		hash_bucket *bucket=&(hash->buckets[hash_val]);
		//exists=check_exists_in_bucket(bucket,words[start],&pos);
		exists=check_exists_in_bucket(words[start],&pos,bucket->children,bucket->children_number);
		if(exists==0){ 
			start++;
			free(str);
			continue;
			}
		node=&(bucket->children[pos]);
		myappend_pan(&str,&str_size,words[start]);
		
		while(node->number_of_childs!=0) {
			check=check_node(node,current_version);
			if(node->is_final=='y' && check!=ERROR) { //found ngram
				if(bloomfilter_check(str,bloomfilter,bloomfilterbytes)==0){
						bloomfilter_add(str,bloomfilter,bloomfilterbytes);
						top=add_top_threads(top,str,Q_number);
						ngrams_found++;
					}
				}

			word_number++;
			if(word_number>number_of_words+section_start) break;
			exists=check_exists_in_children(node,words[word_number],&pos);
			if(exists==0) break;
			node=&(node->children[pos]);			
			myappend_pan_with_space(&str,&str_size,words[word_number]);
		}

		if(exists==1 && word_number<=section_start+number_of_words) {
			check=check_node(node,current_version);
			if(check!=ERROR){
			if(bloomfilter_check(str,bloomfilter,bloomfilterbytes)==0){
						bloomfilter_add(str,bloomfilter,bloomfilterbytes);
						//printf("Found %s\n",str);
						top=add_top_threads(top,str,Q_number);
						ngrams_found++;
			}
			}
		}
		free(str);
		start++;
	}
	//end_gram_table(top,ngrams_found);
	int found=SUCCESS;
	//free(str);
	free(bloomfilter);
	//free(*data);	
	return found;
	if(TestAllBits(bloomfilter,bloomfilterbytes)==0) found=-1;
	if(exists==0) return ERROR;
	
	return SUCCESS;	


	return 0;
}

//--------------------------------cleanup functions----------------------------------------//
int deleteTrieNode_cleanup(hash_layer *hash,char **words,int word_number,int current_version){
	int error;

	int hash_val=hash_function(hash,words[0]);
	//printf("hash val is %d\n",hash_val);

	trie_node *node;
	int pos;
	node=delete_from_backet_cleanup(hash,hash_val,words[0],&pos,current_version); //node gets the value of the first trie node
	if(node==NULL) return ERROR;
	if(word_number>1){ 
		//printf("deleting more words\n");
		error=delete_ngram_cleanup(node,words,1,word_number-1,current_version);
		if(error!=SUCCESS){
			return error;
		}
	
		if(node->number_of_childs!=0 || node->is_final=='y') return 2;
		hash_bucket *bucket=&(hash->buckets[hash_val]);
		int total=bucket->children_number;
		free(node->word);
		free(node->children);
		memmove(&(bucket->children[pos]),&(bucket->children[pos+1]),(total-(pos+1))*sizeof(trie_node));
		bucket->children_number--;
		return SUCCESS;
	}
	if(node->is_final=='n') return ERROR;
	if(node->number_of_childs!=0){ 
		node->is_final='n';
		return 2;
	}

	hash_bucket *bucket=&(hash->buckets[hash_val]);
	int total=bucket->children_number;
	free(node->word);
	free(node->children);
	memmove(&(bucket->children[pos]),&(bucket->children[pos+1]),(total-(pos+1))*sizeof(trie_node));
	bucket->children_number--;
	hash->total_children--;
	
	
	return SUCCESS;
}

trie_node *delete_from_backet_cleanup(hash_layer *hash,int hash_val,char *word,int *pos,int current_version){
	hash_bucket *bucket=&(hash->buckets[hash_val]);

	trie_node *node;
	//int exists=check_exists_in_bucket(bucket,word,pos);
	int exists=check_exists_in_bucket(word,pos,bucket->children,bucket->children_number);
	//printf("exists in delete is %d and word is %s and hash_val is %d\n",exists,word,hash_val);
	if(exists==0) return NULL;
	node=&(bucket->children[*pos]);
	return node;
}

int delete_ngram_cleanup(trie_node *root,char **word,int word_number,int number_of_words,int current_version){
		int error;
		//printf("in delete ngram word \"%s\"\n",word[word_number]);
		if(word_number==number_of_words+1){
			if(root->number_of_childs!=0 && root->is_final!='y') return ERROR;
			if(root->number_of_childs!=0 &&root->is_final=='y'){
				root->is_final='n';
				return 2;}  
			return SUCCESS;//and return no error . the previous one is gonna delete it	
		}
		if(root->number_of_childs==0) return ERROR;
		else{
			int pos;
			int exists=check_exists_in_children(root,word[word_number],&pos);
			if (exists==1){
				error=delete_ngram_cleanup(&(root->children[pos]),word,word_number+1,number_of_words,current_version);
				if(error==0)
				{
					if(root->children[pos].is_final=='y' && word_number!=number_of_words ) return 2; //return 2 if  i am del half 
					error=delete_from_node_cleanup(root,pos,current_version);
					root->number_of_childs--;
				}
				if(error!=1 && root->number_of_childs!=0) return 2; //dont delete the node if it has more childs 
				return error; 
			}
			else return ERROR;	//return error if the word is not on the trie , so the ngram is not in the trie
			}
}



int delete_from_node_cleanup(trie_node *node,int pos,int current_version){

		trie_node * backup=node->children;
		trie_node *node_to_delete=&(node->children[pos]);
		destroy_childs(node_to_delete);
		free(node_to_delete->word);
		memmove(node->children,backup,pos*sizeof(trie_node));
		memmove(node->children+pos,backup+pos+1,(node->number_of_childs-(pos+1))*sizeof(trie_node));
		node->children_deleted--;
		return SUCCESS ; //zero errors
}

void delete_ngrams(hash_layer *hash,ngrams_to_delete *d_grams){
	int i=0;
	int start=0;
	//int j;
	int length;
	while(d_grams->length[i]!=0){
		length=d_grams->length[i];
		deleteTrieNode_cleanup(hash,&(d_grams->nodes_to_delete[start]),length,0);
		start=start+d_grams->length[i];
		i++;
	}

}

void print_hash_version(hash_layer *hash){
	int i,j;
	hash_bucket bucket;
	trie_node node;
	for(i=0;i<hash->buckets_number;i++){
		bucket=hash->buckets[i];
		printf("Bucket[%d]::",i);
		for(j=0;j<bucket.children_number;j++){
			node=bucket.children[j];
			printf("-%s(%c,%d,%d)->",node.word,node.is_final,node.A_version,node.D_version);
			print_trie_version(&node,0);
		}
		printf("\n");
	}
}

void print_trie_version(trie_node *node,int level){
	int i;
	if(node->number_of_childs==0){printf("\n"); return;}
	//printf("In level %d : \n",level );
	for(i=0;i<node->number_of_childs;i++){
		//print_node(&(node->children[i]));
		if(level!=0) printf("->");
		printf("%s (%c,%d,%d) ",node->children[i].word,node->children[i].is_final,node->children[i].A_version,node->children[i].D_version);
		print_trie_version(&(node->children[i]),level+1);
	}
	return;
}


void test(void){

printf("Hello from Test\n");

}
