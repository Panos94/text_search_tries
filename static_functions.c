#include "static_functions.h"
//#include "libraries.h"

extern int buffer_size;
extern int word_size;
extern int table_size;

static_hash_layer *initialize_static_hash(hash_layer *hash){
	static_hash_layer *static_hash;
	static_hash=malloc(sizeof(static_hash_layer));
	static_hash->buckets=malloc(hash->buckets_number*sizeof(static_hash_bucket));
	static_hash->buckets_number=hash->buckets_number;
	static_hash->total_children=hash->total_children;
	static_hash->load_factor=hash->load_factor;
	static_hash->bucket_capacity=hash->bucket_capacity;
	static_hash->split_round=hash->split_round;
	return static_hash;
}

static_hash_layer *compress(hash_layer *hash){
	int i,j;
	hash_bucket *bucket;
	static_hash_bucket *static_bucket;
	trie_node *node;
	static_trie_node *static_node;
	static_hash_layer *static_hash;
	static_hash=initialize_static_hash(hash);
	//printf("buckets are %d\n",static_hash->buckets_number);
	for(i=0;i<hash->buckets_number;i++){
		bucket=&(hash->buckets[i]);
		static_bucket=&(static_hash->buckets[i]);
		//printf("bucket[i] is %d\n",i);
		static_bucket->children=malloc(bucket->capacity*sizeof(static_trie_node));
		static_bucket->children_number=bucket->children_number;
		static_bucket->capacity=bucket->capacity;
		//initialize static nodes
		for(j=0;j<bucket->children_number;j++){
			//printf("j is %d\n",j);
			node=&(bucket->children[j]);
			static_node=&(static_bucket->children[j]);
			//init_static_nodes(static_node);
			compress_node(node,static_node);
			//printf("Final word is %s \n",static_node->word);
		}
	}
	return static_hash;
}

int compress_node(trie_node *node,static_trie_node *static_node){
	if(node->number_of_childs==0){
		init_final_static_node(static_node,node->word,'y',CHILD_NUM);
		//printf("last word is %s\n",static_node->word);
		return 0; 
	}//ok
	int i;
	int merge_true=0;
	static_node->children=malloc(node->number_of_childs*sizeof(static_trie_node));
	if(static_node->children==NULL) exit(2);
	//printf("here\n");
	if(node->number_of_childs!=1) init_static_node(static_node,node->word,node->is_final);//add the word to the static node
	static_node->number_of_childs=node->number_of_childs;
	for(i=0;i<node->number_of_childs;i++){
		//printf("k is %d\n",i);
		merge_true=compress_node(&(node->children[i]),&(static_node->children[i]));

		if(node->number_of_childs!=1) continue;
		//printf("merged\n");
		merge_nodes(static_node,node,&(static_node->children[0]));
		//printf("static_node word result is is %s\n",static_node->word);
		//free(static_node->children);

		}
		
	//printf("childs are %d\n",static_node->number_of_childs);
	return merge_true;
}

int merge_nodes(static_trie_node *static_node,trie_node *node,static_trie_node *child){

	static_node->word=malloc( (strlen(child->word)+ strlen(node->word)+1) *sizeof(char));
	strcpy(static_node->word,node->word);
	strcat(static_node->word,child->word);

	static_node->is_final=malloc((child->number_of_words+1)*sizeof(short));

	if(node->is_final=='y')	static_node->is_final[0]=(-1*strlen(node->word));
	else static_node->is_final[0]=strlen(node->word);
	memmove(&(static_node->is_final[1]),child->is_final,child->number_of_words*sizeof(short));

	static_node->number_of_childs=child->number_of_childs;
	
	static_node->number_of_words=1+	child->number_of_words;
	free(child->word);
	free(child->is_final);
	//free(static_node->children);
	if(child->number_of_childs==0){
			free(static_node->children);
		static_node->children=NULL;
		return 1; //SUCCESS
	}

	free(static_node->children);
	static_node->children=child->children;
	return 1; //SUCCESS
}

void init_final_static_node(static_trie_node *static_node,char *word,char is_final,int c){
	int word_len=strlen(word);
	static_node->word=malloc((word_len+1)*sizeof(char));
	static_node->is_final=malloc(1*sizeof(short));
	strcpy(static_node->word,word);
	if(is_final=='y') static_node->is_final[0]=(-1*word_len);
	else static_node->is_final[0]=(word_len);
	//printf("strlen is %d\n",static_node->is_final[0]); here is ok
	static_node->number_of_childs=0;
	static_node->number_of_words=1;
	
	static_node->max_childs=c;
	static_node->children=NULL; //check that later
}

void init_static_node(static_trie_node *static_node,char *word,char is_final){
	int word_len=strlen(word);
	static_node->word=malloc((word_len+1)*sizeof(char));
	static_node->is_final=malloc(sizeof(short));
	strcpy(static_node->word,word);
	if(is_final=='y') static_node->is_final[0]=(-1*word_len);
	else static_node->is_final[0]=(word_len);
	static_node->number_of_words=1;
	static_node->number_of_childs=0;
}

void init_static_node_with_children(static_trie_node *static_node,char *word,char is_final,int c){
	int word_len=strlen(word);
	static_node->word=malloc((word_len+1)*sizeof(char));
	static_node->is_final=malloc(sizeof(short));
	strcpy(static_node->word,word);
	if(is_final=='y') static_node->is_final[0]=(-1*word_len);
	else static_node->is_final[0]=(word_len);
	static_node->number_of_words=1;
	static_node->number_of_childs=0;

	static_node->max_childs=c;
	static_node->children=malloc(c*sizeof(static_trie_node)); //check that later
}

void destroy_static_hash(static_hash_layer *hash){
	int i;
	static_hash_bucket *bucket;
	for(i=0;i<hash->buckets_number;i++){
		//printf("Bucket[i]:: %d\n",i);
		bucket=&(hash->buckets[i]);
		destroy_static_bucket_nodes(bucket);
	}
	free(hash->buckets);
	free(hash);
	return ;
}

void destroy_static_bucket_nodes(static_hash_bucket *bucket){
	static_trie_node *node;
	int j;
	for(j=0;j<bucket->children_number;j++){
		//printf("node:: %d\n",j);
		node=&(bucket->children[j]);
		destroy_static_childs(node);
		free(node->word);
		free(node->is_final);
		}
	free(bucket->children);
}

void destroy_static_childs(static_trie_node *node){
	int i;
	//printf("number of childs are %d\n",node->number_of_childs);
	for(i=0;i<node->number_of_childs;i++){
		//printf("child:: %d\n",i);
		destroy_static_childs(&(node->children[i]));
		free(node->children[i].word);
		free(node->children[i].is_final);	
	}
	if(node->children!=NULL) free(node->children);
}

void print_static_hash(static_hash_layer *hash){
	int i,j;
	static_hash_bucket bucket;
	static_trie_node node;
	printf("In print static_hash with buckets %d\n",hash->buckets_number);
	for(i=0;i<hash->buckets_number;i++){
		bucket=hash->buckets[i];
		printf("Bucket[%d]::",i);
		for(j=0;j<bucket.children_number;j++){
			node=bucket.children[j];
			printf("%s (",node.word);
			print_lens(node.is_final,node.number_of_words);
			printf(")->");
			print_static_trie(&node,0);
		}
		printf("\n");
	}
}

void print_lens(short *is_final,int words_number){
	int i;
	for(i=0;i<words_number;i++){
		printf("%d,",is_final[i]);
	}
}

void print_static_trie(static_trie_node *node,int level){
	int i;
	if(node->number_of_childs==0){printf("\n"); return;}
	printf("childs %d : \n",node->number_of_childs );
	for(i=0;i<node->number_of_childs;i++){
		//print_node(&(node->children[i]));
		if(level!=0) printf("->");
		printf("%s (",node->children[i].word);
		print_lens((node->children[i]).is_final,(node->children[i]).number_of_words);
		printf(")");
		print_static_trie(&(node->children[i]),level+1);
	}
	return;
}


int init_static_input(struct static_index *trie,char * filename){
	//printf("\x1b[32m""INIT_INPUT start\n""\x1b[0m");
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
				for(a=words_in;a<table_size;a++) //a=0
					ptr_table[a] = realloc(ptr_table[a],(word_size+1)*sizeof(char));
			}
		//	ptr_table[words_in] = malloc(word_size*sizeof(char));
			strcpy(ptr_table[words_in],word);
			//printf("Kuru word = %s\n",ptr_table[words_in]);
			words_in++;
			word=strtok(NULL," \n");
		}
		insert_staticTrieNode(trie->hash,ptr_table,words_in);
		//print_hash(trie->hash);
	}
	//printf ("free\n");
	free(line);
	cleanup(ptr_table);
	fclose(fd);
	return 0;	
}


int test_static_input(struct static_index *trie,char * filename)
{ 	
	//printf("\x1b[32m""TEST_INPUT start\n""\x1b[0m");
	int words_in = 0;
	int flag; //1 question, 2 addition, 3 deletion, 4 end of file
	int a;
	printf("filename is %s\n",filename);
	FILE* fd = fopen(filename, "r"); //opening input file
	if(fd == NULL)
	{
		perror("Error opening input file");
		return -1;
	}
	//printf("word_size here %d",word_size);
	
	topk_threads *top;
	top=create_top_threads(top);
	top=init_top_threads(top);
	
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *word;
	//int command_error;
	int word_len;
	int length_array_capacity=10;
	int last_word=0;
	int lengths_taken=0;

	int threads_quantity  = 15 ;
	JobScheduler *JS = initialize_scheduler(threads_quantity);
	Job *job_to_append = malloc(sizeof(Job));
	int Q_number=0;
	int previous_table;	
	
	int *Q_lengths=malloc(length_array_capacity*sizeof(int));
	int *start=malloc(length_array_capacity*sizeof(int));
	int *word_lengths=malloc(table_size*sizeof(int));

	char **ptr_table = malloc(table_size*sizeof(char *));
	for(a=0;a<table_size;a++){
		ptr_table[a]=malloc(word_size*sizeof(char));
		word_lengths[a]=word_size;
	}

	char ***pointer_to_words=malloc(sizeof(char**));
	*pointer_to_words=ptr_table;
	words_in = 0;
	while ((read = getline(&line, &len, fd)) != -1) {
		//words_in = 1;
		
		word = strtok (line," \n");
		
		if(strcmp(word,"Q")==0) flag=1;
		else if(strcmp(word,"F")==0){
				word=strtok(NULL,"\n");
				int k;
				words_in=0;
				execute_all_jobs(JS);
				//execute_static_queries(trie->hash,ptr_table,Q_lengths,start,lengths_taken,top);
				//execute_queries(trie->hash,ptr_table,Q_lengths,version,start,lengths_taken,top);
				lengths_taken=0;
				last_word=0;

				print_print_threads(top,Q_number);
				if(word!=NULL){
					k=atoi(word);
					int total_ngrams=get_total_ngrams(top,Q_number);
					char **merged_ngrams=malloc(total_ngrams*sizeof(char*));
					merge_kframes_threads(top,Q_number,total_ngrams,merged_ngrams);
					create_threads_hashtable(top,total_ngrams);
					built_hashtable(top,merged_ngrams,total_ngrams);
					print_top_threads(top,merged_ngrams,total_ngrams,k);
					erase_hashtable_from_top(top);
					free(merged_ngrams);
				}
				top=init_top_threads(top);
				Q_number=0;
			}
		else if(strcmp(word,"\0")==0) continue;
		word=strtok(NULL," \n");
		while(word!=NULL){
				
			if(words_in==table_size-1){
				previous_table=table_size;
				table_size=table_size<<1;
				ptr_table = realloc(ptr_table,table_size*sizeof(char*));
				*pointer_to_words=ptr_table;
				word_lengths = realloc(word_lengths,table_size*sizeof(int));
				if(ptr_table==NULL) exit(-1);
				for(a=previous_table;a<(table_size);a++){
					ptr_table[a]=malloc(word_size*sizeof(char));
					if(ptr_table[a]==NULL) exit(-1);
					word_lengths[a] =word_size;
					}
				//table_size*=2;
				}
			word_len=strlen(word);
			if(word_len>=word_lengths[words_in]){
				while(word_len>=word_lengths[words_in]){ word_lengths[words_in]*=2;}
				ptr_table[words_in] = realloc(ptr_table[words_in],word_lengths[words_in]*sizeof(char));
			}		

				//ptr_table[words_in] = malloc(word_size*sizeof(char));
		strcpy(ptr_table[words_in],word);
				
		words_in++;				
			
		word=strtok(NULL," \n");
		}

		switch(flag){
			case 1 :
				
				if(Q_number==top->Q_capacity){
					extend_top_threads(top,top->Q_capacity<<1);
				}
				//job_to_append= malloc(sizeof(Job));
				job_to_append->opt=(lookup_static_TrieNode_threads);
				
				
				q_args_static* arguments = malloc(sizeof(q_args_static));
				arguments->hash = trie->hash;
				
				arguments->words = pointer_to_words;
				arguments->number_of_words = words_in-last_word-1;//Q_lengths[lengths_taken]-1;
				arguments->top = top;
				//arguments->version = 0;//current_version;
				arguments->start = last_word;//start[lengths_taken];
				arguments->Q_number = Q_number;
				job_to_append->arguments= (void*)arguments;//(void*)ptr;
				submit_job(JS,job_to_append);

				last_word=words_in;
				lengths_taken++;
				Q_number++;
				//command_error=lookup_static_TrieNode(trie->hash,ptr_table,words_in-1,top);
				//if(command_error==-1) printf("%d\n",command_error);
				break;
		
		}
		flag=0;	
}
	free(word_lengths);
	free(pointer_to_words);
	free(job_to_append);
	erase_top_threads(top);
	destroy_scheduler(JS);
	free(Q_lengths);
	free(start);
  	free(line);
	cleanup(ptr_table);
	fclose(fd);
	
return 0;
}

int execute_static_queries(static_hash_layer *hash,char **ptr_table,int *ptr_lengths,int *start,int pos,topk_threads *top){
	int i;
	//int command_error;
	for(i=0;i<pos;i++){
		lookup_static_TrieNode(hash,&(ptr_table[start[i]]),ptr_lengths[i]-1,top,i);	
		//command_error=lookupTrieNode_with_bloom_versioning(hash,&(ptr_table[start[i]]),ptr_lengths[i]-1,top,version[i]);
	}
	//printf("results:\n");
	return 0;
}

int insert_staticTrieNode(static_hash_layer *hash,char **words,int word_number){
	//printf("appending %s\n",words[0]);
	char is_final='n';
	if(word_number==1) is_final='y';

	//int hash_val=hash_function(hash,words[0]);
	//printf("total children are %d, buckets_number are %d\n",hash->total_children,hash->buckets_number);
	if((hash->total_children/((float)hash->buckets_number*hash->bucket_capacity)) > hash->load_factor){
		printf("total children are %d, buckets_number are %d\n",hash->total_children,hash->buckets_number);
        printf("load factor: %f \n",(hash->total_children/((float)hash->buckets_number*hash->bucket_capacity)));
		printf("capacity: %d \n",hash->bucket_capacity);
		int resize_error=resize_static_hash(hash);
		if(resize_error==ERROR) return ERROR;
	}
	
	int hash_val=static_hash_function(hash,words[0]);
	

	static_trie_node *node;
	node=add_to_static_backet(hash,hash_val,words[0],is_final); //node gets the value of the first trie node

	if(word_number>1){ 
		append_static_trie_node(node,words,1,word_number-1);
		//node->number_of_childs++;
		return 0;	
	}	
	if(node->is_final[0]>0) node->is_final[0]=-1*node->is_final[0];
	return 0;
}

int append_static_trie_node(static_trie_node *node,char **word,int word_number,int number_of_words){
	int start=1;
	int exists=1;
	int exists_children=0;
	int pos;
	char is_final='n';
	int error;
	while(word_number<=number_of_words){
		printf("word is %s\n",word[word_number]);
		if(word_number==number_of_words) is_final='y';
		exists=check_exists_in_node(node,word[word_number],start);
		printf("exists = %d , start = %d node->number of words %d\n",exists,start,node->number_of_words);
		if(exists==0 && start==node->number_of_words){
			printf("here exists=0 start==node_number_of_words\n"); 
			exists_children=check_exists_in_static_children(node,word[word_number],&pos);
			printf("exists in children %d\n",exists_children);
			if(exists_children==1){ //if it exists in the child then continue searching ther
				node=&(node->children[pos]);
				start=1;
				word_number++;
				continue;
			}
			break;
		} //search in a child node
		if(exists==0) break; //ihave to split the node
		start++;
		word_number++;
	} //while finished
	printf("exists = %d , start = %d node->number of words %d\n",exists,start,node->number_of_words);
	if(word_number>number_of_words){
		if(node->is_final[start-1]>0) node->is_final[start-1]=-1*node->is_final[start-1];
		 return SUCCESS;
		} //Already found the word
	if(start==node->number_of_words && exists_children==0){ //time to add it
		//add it to children
		printf("Add to children\n");
		if(node->number_of_childs==0){ //if the word doesn't exists and children are zero then concat
			while(word_number<=number_of_words){
				printf("here\n");
				if(word_number==number_of_words) is_final='y';
				error=concat_word(node,word[word_number],is_final);
				if(error==ERROR) return ERROR;
				word_number++;
				printf("number of words now %d\n",node->number_of_words);
			}
			return SUCCESS;
		}
		error=append_static_word(node,pos,word[word_number],is_final); //if the children are not zero then add it to children
		if(error==ERROR) return ERROR;
		node->number_of_childs++;
		word_number++;
		while(word_number<=number_of_words){ //append the rest pf the words to the child created

			if(word_number==number_of_words) is_final='y';
			error=concat_word(&(node->children[pos]),word[word_number],is_final);
			if(error==ERROR) return ERROR;
			word_number++;
		}

	}
	else if(exists==0 && start!=node->number_of_words){ //split node
		printf("split node in %d\n",start);
		char *temp_word=malloc(WORD_SIZE*sizeof(char));
		int temp_word_size=WORD_SIZE;
		split_static_node(node,start);
		temp_word=get_i_word(&(node->children[0]),0,temp_word,&temp_word_size);
		//add the rest of the words;
		pos=1;
		if(strcmp(temp_word,word[word_number])>0) pos=0; // where to add the new child
		
		error=append_static_word(node,pos,word[word_number],is_final);
		if(error==ERROR) return ERROR;
		node->number_of_childs++;
		word_number++;
		while(word_number<=number_of_words){
			//append the rest of the words here
			if(word_number==number_of_words) is_final='y';
			error=concat_word(&(node->children[pos]),word[word_number],is_final);
			if(error==ERROR) return ERROR;
			word_number++;
		}			
		
		free(temp_word);
		printf("did something\n");
		return SUCCESS; //0 errors
	}
	//add it as a child
	return SUCCESS; //0 errors
}

int split_static_node(static_trie_node *node ,int pos){
	int i;
	char is_final;
	char *temp_word=malloc(WORD_SIZE*sizeof(char));
	int temp_word_size=WORD_SIZE;
	int error;
	static_trie_node *target;
	temp_word=get_i_word(node,pos,temp_word,&temp_word_size);
	printf("first child is %s pos %d\n",temp_word,pos);				
	static_trie_node *backup=node->children;
	printf("node max childs %d",node->max_childs);
	node->children=malloc(node->max_childs*sizeof(static_trie_node));

	target=&(node->children[0]);	
	printf("first child is %s\n",temp_word);
	node->is_final[pos]<0?is_final='y':'n';				
	init_final_static_node(target,temp_word,is_final,node->max_childs);//check here

	for(i=pos+1;i<node->number_of_words;i++){
		temp_word=get_i_word(node,i,temp_word,&temp_word_size);
		is_final='n';
		if(node->is_final[i]<0) is_final='y';
		error=concat_word(target,temp_word,is_final);
		if(error==ERROR) return ERROR;	
	}

	target->children=backup;
	target->number_of_words=node->number_of_words-pos;
	target->number_of_childs=node->number_of_childs;

	print_lens(target->is_final,target->number_of_words);

	node->number_of_words=pos;
	node->number_of_childs=1;
	update_word(node,pos);
	//update children and words in the two nodes
	//update is final
	free(temp_word);
	return SUCCESS;
}

void update_word(static_trie_node *node,int pos){
	int i;
	int len=0;
	printf("pos is in update %d\n",pos);
	for(i=0;i<pos;i++) len=len+abs(node->is_final[i]);
	printf("len is %d\n",len);
	node->word[len]='\0';
	return;
}

int concat_word(static_trie_node *static_node,char *word,char is_final){
	static_node->word=realloc(static_node->word, (strlen(word)+ strlen(static_node->word)+1) *sizeof(char));
	if(static_node->word==NULL) return ERROR; 
	strcat(static_node->word,word);

	static_node->is_final=realloc(static_node->is_final,(static_node->number_of_words+1)*sizeof(short));
	if(static_node->is_final==NULL) return ERROR; 

	if(is_final=='y')	static_node->is_final[static_node->number_of_words]=(-1*strlen(word));
	else static_node->is_final[static_node->number_of_words]=strlen(word);
	
	static_node->number_of_words=1+	static_node->number_of_words;
	return SUCCESS;
}


int check_exists_in_static_children(static_trie_node *node,char *word,int *pos){
		int pivot=0; //pivor is integer so in the division it will rounf in the smaller absolute value 5/2=2
		int lower=0;
		int upper=node->number_of_childs-1;
		char *temp_word=malloc(WORD_SIZE*sizeof(char));
		int temp_word_size=WORD_SIZE;
		//printf("inside check exists\n");
		if(upper==-1) {
			free(temp_word);
			return 0;
		} //i made this change
		while(1!=0){
			//printf("upper %d lower %d pivot %d\n",upper,lower,pivot);
			if(upper<=lower){
				pivot=(upper+lower)/2;
				temp_word=get_i_word(&(node->children[pivot]),0,temp_word,&temp_word_size);				
				int compare=strcmp(temp_word,word);
				if(compare==0){
					*pos=pivot;
					free(temp_word);
					return 1; //exact match
				}
				//printf("compare is %d\n",compare);	
				*pos=(compare<0)? pivot+1:pivot; //lower+1:lower
				free(temp_word);
				return 0; //not exact match
				}
			else {
				pivot=(upper+lower)/2;
				temp_word=get_i_word(&(node->children[pivot]),0,temp_word,&temp_word_size);
				int compare=strcmp(temp_word,word); // equal=0 children[i]<word: compare<0 children>word : compare>0
				if(compare==0) {
					*pos=pivot;
					free(temp_word);
					return 1; //exact match
				}
				else if(compare>0) upper=pivot-1;
				else lower=pivot+1;
			}
		}
}

int append_static_word(static_trie_node *node,int pos,char *word,char is_final){
		//printf("inside append_word , pos %d\n",pos);
		if(node->number_of_childs==node->max_childs){
				//printf("I have to double the children\n");
				node->children=realloc(node->children,node->max_childs*2*sizeof(static_trie_node));
				//printf("Done Realloc \n");
				if(node->children==NULL) return ERROR;
		        node->max_childs*=2;
				//print_node(&(node->children[0]));	
		}

		static_trie_node * backup=node->children;
		memmove(node->children,backup,pos*sizeof(static_trie_node));
		memmove(node->children+(pos+1),backup+(pos),(node->number_of_childs-pos)*sizeof(static_trie_node));
		init_static_node_with_children(&(node->children[pos]),word,is_final,CHILD_NUM);
		return SUCCESS ; //zero errors
}


int  static_hash_function(static_hash_layer *hash, char *word)
{	
	int hash_value;
    unsigned long hash_int = 5381;
    int c;

    while (c = *word++)
        hash_int = ((hash_int << 5) + hash_int) + c; 
	//printf("hash is %ld\n",hash_int);

	hash_value=hash_int%(C*(int)pow(2,hash->split_round));

	int temp=hash_int%(C*(int)pow(2,hash->split_round+1));
	if(temp<hash->buckets_number) return temp;
	//printf("new hash value after is %ld\n",hash_value);
    return hash_value;
}

int check_exists_in_node(static_trie_node *node,char *word,int expected){
	char *temp_word=malloc(WORD_SIZE*sizeof(char));
	int temp_word_size=WORD_SIZE;
	temp_word=get_i_word(node,expected,temp_word,&temp_word_size);
	int eval;
	if(temp_word==NULL){ //error
		free(temp_word); 
		return 0; //doesnt exists
	}
	eval=strcmp(word,temp_word);
	free(temp_word);
	if(eval==0) return 1; //exists
	return 0; //doesnt exist
}

int check_exists_in_static_bucket(static_hash_bucket *bucket,char *word,int *pos){
		char *temp_word=malloc(WORD_SIZE*sizeof(char));
		int temp_word_size=WORD_SIZE;
		int pivot=0; //pivor is integer so in the division it will rounf in the smaller absolute value 5/2=2
		int lower=0;
		if(bucket->children_number==0) {
			*pos=0;
			free(temp_word);
			return 0;
		}
		int upper=bucket->children_number-1;
		//printf("inside check exists\n");
		//if(upper==-1) return 0; //i made this change
		while(1!=0){
			//printf("upper %d lower %d pivot %d\n",upper,lower,pivot);
			if(upper<=lower){
				pivot=(upper+lower)/2;
				temp_word=get_i_word(&(bucket->children[pivot]),0,temp_word,&temp_word_size);
				//printf("word is %s\n",word);
				int compare=strcmp(temp_word,word);
				//printf("compare is %d\n",compare);
				if(compare==0){
					*pos=pivot;
					free(temp_word);
					return 1; //exact match
				}
				//printf("compare is %d\n",compare);	
				*pos=(compare<0)? pivot+1:pivot; //lower+1:lower
				free(temp_word);
				return 0; //not exact match
				}
			else {
				pivot=(upper+lower)/2;
				temp_word=get_i_word(&(bucket->children[pivot]),0,temp_word,&temp_word_size);
				int compare=strcmp(temp_word,word); // equal=0 children[i]<word: compare<0 children>word : compare>0
				//printf("compare is %d\n",compare);
				if(compare==0) {
					*pos=pivot;
					free(temp_word);
					return 1; //exact match
				}
				else if(compare>0) upper=pivot-1;
				else lower=pivot+1;
			}
		}
}

char* get_i_word(static_trie_node *node,int i,char* temp_word,int *temp_word_size){
	if(node->number_of_words<=i) return NULL ; //error
	int len=abs(node->is_final[i]);
	int start=0;
	int j;
	for(j=0;j<i;j++) start+=abs(node->is_final[j]);
	//memmove(temp_word,&(node->word[start]),len*sizeof(char));
	if(*temp_word_size<=len){
		while(len> *temp_word_size)*temp_word_size=(*temp_word_size)<<1;
		temp_word=realloc(temp_word,(*temp_word_size)*sizeof(char));	
	}
	strncpy(temp_word,&(node->word[start]),len);
	temp_word[len]='\0';
	return temp_word; //no error
}

static_trie_node* add_to_static_backet(static_hash_layer *hash,int hash_val,char *word,char is_final){
	printf("In add to bucket %s\n",word);
	
	
	static_hash_bucket *bucket=&(hash->buckets[hash_val]);
	
	int *last=&(bucket->children_number);
	static_trie_node *node;


	if(*last==bucket->capacity){ //initializing overflow bucket
		bucket->children=realloc(bucket->children,2*bucket->children_number*sizeof(static_trie_node));
		if(bucket->children==NULL) return NULL;
		bucket->capacity=bucket->capacity*2;
		//printf("I made an overflow at bucket %d\n",hash_val);
	} 

	int pos;
	int exists=check_exists_in_static_bucket(bucket,word,&pos);

	node=&(bucket->children[pos]);
	printf("pos is %d and hash_val = %d\n",pos,hash_val);
	if(exists==1) return node;
	//memove nodes to the right
	static_trie_node *backup=bucket->children;
	if(backup==NULL) printf("backup is NULL\n");
	memmove(bucket->children,backup,pos*sizeof(static_trie_node));
	memmove(bucket->children+(pos+1),backup+(pos),(bucket->children_number-pos)*sizeof(static_trie_node));//
	init_static_node_with_children(node,word,is_final,CHILD_NUM);

	*last=*last+1;
	hash->total_children++;
	//int resize_error;
	//printf("children now %d\n",*last);
	return node;
}

int resize_static_hash(static_hash_layer *hash){
	static_hash_bucket *bucket;
	//printf("attempting to expand buckets\n");
	hash->buckets=realloc(hash->buckets,(hash->buckets_number+1)*sizeof(static_hash_bucket)); //add bucket lineat
	if(hash->buckets==NULL){
		printf("error in realloc\n");
		return ERROR;
	}
	hash->buckets_number++;
	//initialize bucket
	//initialize_bucket(&(hash->buckets[hash->buckets_number-1]),hash->bucket_capacity);
	int i;
	static_hash_bucket *new_bucket=&(hash->buckets[hash->buckets_number-1]); //pointer to the new bucket
	bucket=&(hash->buckets[hash->bucket_to_split]); //re arranging bucket to split
	int new_hash_val=-1;
	stack *stack_=init_stack();
	//print_stack(stack_);

	if(bucket->children_number==0){
		//printf("Bucket to split is %d and split round %d empty\n",hash->bucket_to_split,hash->split_round);
		hash->bucket_to_split=(hash->bucket_to_split+1)%(C*(int)pow(2,hash->split_round));//
		if(hash->bucket_to_split==0 && hash->buckets_number>C) hash->split_round++;
		stack_destroy(stack_); 
		return SUCCESS;
	} // no need for rearranging bucket
	
	
	int previous=hash->bucket_to_split;
	hash->bucket_to_split=(hash->bucket_to_split+1)%(C*(int)pow(2,hash->split_round));//
	for(i=0;i<bucket->children_number;i++){

		new_hash_val=static_hash_function(hash,bucket->children[i].word);
		//printf("new hash val is %d \n",new_hash_val);
		if(new_hash_val==previous) continue; //if hash value is the same then no need to change bucket
		if(new_bucket->children_number==new_bucket->capacity){	//if new bucket fills then create an overflow bucket
			new_bucket->children=realloc(new_bucket->children,bucket->capacity*2*sizeof(static_trie_node));
			if(new_bucket->children==NULL){
				stack_destroy(stack_); 
				return ERROR;
			}
			new_bucket->capacity=new_bucket->capacity*2;	
		}
		memmove(&(new_bucket->children[new_bucket->children_number]),&(bucket->children[i]),sizeof(static_trie_node)); //copy nod
		push(stack_,i);		
		new_bucket->children_number++;	
	}

	//print_stack(stack_);
	shrink_static_buckets(&(hash->buckets[previous]),stack_);
	//hash->bucket_to_split=(hash->bucket_to_split+1)%(C*(int)pow(2,hash->split_round));//
	if(hash->bucket_to_split==0 && hash->buckets_number>C) hash->split_round++;
	stack_destroy(stack_); 
	return SUCCESS;
}

void shrink_static_buckets(static_hash_bucket *bucket,stack *stack_){
	int i;
	int pos;
	int *total=&(bucket->children_number);
	for(i=stack_->top-1;i>=0;i--){
		pos=get_stack_elements(stack_,i);
		//printf("shrink_buckets pos %d\n",pos);
		
		memmove(&(bucket->children[pos]),&(bucket->children[pos+1]),(*total-(pos+1))*sizeof(static_trie_node));
		//printf("INSIDE\n");
		*total=*total-1;
		//printf("Now children left on the bucket are %d\n",bucket->children_number);
		}
	/*if(*total<bucket->capacity-hash->bucket_capacity){ //freeing the realloced extra tries
			bucket->children=realloc(bucket->children,hash*sizeof(trie_node));
			}  */
}




int lookup_static_TrieNode(static_hash_layer *hash,char **words,int number_of_words,topk_threads *top,int Q_number){
//printf("Inside search,number of words is %d\n",number_of_words);
	
	size_t bloomfilterbits =bloomfiltersize(number_of_words); 
	size_t bloomfilterbytes = bloomfilterbits/8;
	int * bloomfilter = malloc(bloomfilterbytes);
	bloomfilter_init(bloomfilter,bloomfilterbytes);


	char *temp_word=malloc(WORD_SIZE*sizeof(char));
	int temp_word_size=WORD_SIZE;
	int word_number;
	int exists;
	int pos;
	char *str;
	int str_size;
	static_trie_node *node;
	int start=0;
	int node_word;
	int ngrams_found=0;
	while(start!=number_of_words+1) {
		//printf("\nstart with %s\n",words[start]);
		
		str=malloc(20*sizeof(char));
		strcpy(str,"");
		str_size=20;		
		
		word_number=start;
		
		int hash_val=static_hash_function(hash,words[start]);

		static_hash_bucket *bucket=&(hash->buckets[hash_val]);
		exists=check_exists_in_static_bucket(bucket,words[start],&pos);
		//printf("exists is %d\n",exists);
		if(exists==0){ 
			start++;
			free(str);
			continue;
		}
		node_word=0;
		node=&(bucket->children[pos]);

		stat_myappend_pan(&str,&str_size,words[start]);

		while(node->number_of_childs!=0 || node_word!=node->number_of_words) {
			
			if(node->is_final[node_word]<0) { //is final

				if(bloomfilter_check(str,bloomfilter,bloomfilterbytes)==0){
						bloomfilter_add(str,bloomfilter,bloomfilterbytes);
						top=add_top_threads(top,str,Q_number);
						//top=add_top(top,str);
						ngrams_found++;						
					}

				}
			//printf("word_number is %d\n",word_number);
			node_word++;
			word_number++;
			if(word_number>number_of_words) break;
	
			if(node->number_of_words==node_word){
				//printf("change node\n");
				exists=check_exists_in_static_children(node,words[word_number],&pos);
				node_word=0;
				if(exists==0) break;
				node=&(node->children[pos]);
				stat_myappend_pan_with_space(&str,&str_size,words[word_number]);
			}
			else{
				//printf("same node\n");
				//printf("node word %d\n",node_word);
				temp_word=get_i_word(node,node_word,temp_word,&temp_word_size);
				exists=0;
				if(strcmp(temp_word,words[word_number])==0) exists=1;
				//printf("temp word is %s , words[word_number]: %s\n",temp_word,words[word_number]);
				if(exists==0) break;
				stat_myappend_pan_with_space(&str,&str_size,words[word_number]);
			}
		}
		if(exists==1 && word_number<=number_of_words) {
			if(bloomfilter_check(str,bloomfilter,bloomfilterbytes)==0){
						//printf("%s|",str); 
						bloomfilter_add(str,bloomfilter,bloomfilterbytes);
						top=add_top_threads(top,str,Q_number);
						//top=add_top(top,str);
						ngrams_found++;						
					}
		}

		start++;
		free(str);
	}
	//end_gram_table(top,ngrams_found);
	int found=SUCCESS;
	free(bloomfilter);	
	free(temp_word);	
	return found;
	if(exists==0) return ERROR;
	
	return SUCCESS;	

}

int lookup_static_TrieNode_threads(void ** arguments){//static_hash_layer *hash,char **words,int number_of_words,topk *top){
//printf("Inside search,number of words is %d\n",number_of_words);
	q_args_static *data=(q_args_static*)arguments;
	static_hash_layer *hash=data->hash;
	char **words=*(data->words);
	int number_of_words=data->number_of_words;
	topk_threads *top=data->top;
	int section_start=data->start;
	int Q_number=data->Q_number;

	size_t bloomfilterbits =bloomfiltersize(number_of_words); 
	size_t bloomfilterbytes = bloomfilterbits/8;
	int * bloomfilter = malloc(bloomfilterbytes);
	bloomfilter_init(bloomfilter,bloomfilterbytes);


	char *temp_word=malloc(WORD_SIZE*sizeof(char));
	int temp_word_size=WORD_SIZE;
	int word_number;
	int exists;
	int pos;
	char *str;
	int str_size;
	static_trie_node *node;
	int start=section_start;
	int node_word;
	int ngrams_found=0;
	while(start!=section_start+number_of_words+1) {
		//printf("\nstart with %s\n",words[start]);
		
		str=malloc(20*sizeof(char));
		strcpy(str,"");
		str_size=20;		
		
		word_number=start;
		
		int hash_val=static_hash_function(hash,words[start]);

		static_hash_bucket *bucket=&(hash->buckets[hash_val]);
		exists=check_exists_in_static_bucket(bucket,words[start],&pos);
		//printf("exists is %d\n",exists);
		if(exists==0){ 
			start++;
			free(str);
			continue;
		}
		node_word=0;
		node=&(bucket->children[pos]);

		stat_myappend_pan(&str,&str_size,words[start]);

		while(node->number_of_childs!=0 || node_word!=node->number_of_words) {
			
			if(node->is_final[node_word]<0) { //is final

				if(bloomfilter_check(str,bloomfilter,bloomfilterbytes)==0){
						bloomfilter_add(str,bloomfilter,bloomfilterbytes);
						top=add_top_threads(top,str,Q_number);
						ngrams_found++;						
					}

				}
			//printf("word_number is %d\n",word_number);
			node_word++;
			word_number++;
			if(word_number>number_of_words+section_start) break;
	
			if(node->number_of_words==node_word){
				//printf("change node\n");
				exists=check_exists_in_static_children(node,words[word_number],&pos);
				node_word=0;
				if(exists==0) break;
				node=&(node->children[pos]);
				stat_myappend_pan_with_space(&str,&str_size,words[word_number]);
			}
			else{
				//printf("same node\n");
				//printf("node word %d\n",node_word);
				temp_word=get_i_word(node,node_word,temp_word,&temp_word_size);
				exists=0;
				if(strcmp(temp_word,words[word_number])==0) exists=1;
				//printf("temp word is %s , words[word_number]: %s\n",temp_word,words[word_number]);
				if(exists==0) break;
				stat_myappend_pan_with_space(&str,&str_size,words[word_number]);
			}
		}
		if(exists==1 && word_number<=section_start+number_of_words) {
			if(bloomfilter_check(str,bloomfilter,bloomfilterbytes)==0){
						//printf("%s|",str); 
						bloomfilter_add(str,bloomfilter,bloomfilterbytes);
						top=add_top_threads(top,str,Q_number);
						ngrams_found++;						
					}
		}

		start++;
		free(str);
	}
	//end_gram_table(top,ngrams_found);
	int found=SUCCESS;
	free(bloomfilter);	
	free(temp_word);	
	return found;
	if(exists==0) return ERROR;
	
	return SUCCESS;	

}

void print_nodes_from_static_hash(static_hash_layer *hash,stack *stack_){
	print_stack(stack_);
	
	int number=get_stack_number(stack_);
	char *temp_word=malloc(WORD_SIZE*sizeof(char));
	int temp_word_size=WORD_SIZE;
	int i ,pos;//,prev_pos;
	static_trie_node *node;
	printf("Found N gram: , number %d\n",number);
	pos=get_stack_elements(stack_,0);
	static_hash_bucket *bucket=&(hash->buckets[pos]);
	pos=get_stack_elements(stack_,1);
	node=&(bucket->children[pos]);
	pos=get_stack_elements(stack_,2);
	for(i=0;i<=pos;i++){
		temp_word=get_i_word(node,pos,temp_word,&temp_word_size);
		printf("%s ",temp_word);
	}
	if(number==3){
		printf("|");
		free(temp_word);
		return;
	}
	printf("%s ",temp_word);
	//prev_pos=pos;
	int j;
	for(i=3;i<number-1;i++){
		pos=get_stack_elements(stack_,i);
		node=&(bucket->children[pos]);
		i++;
		pos=get_stack_elements(stack_,i);
		for(j=0;j<=pos;j++){
		temp_word=get_i_word(node,pos,temp_word,&temp_word_size);
		printf("%s ",temp_word);
		}
	}
	
	printf("|");
	free(temp_word);
	printf("\n");
}

void  stat_myappend_pan(char **string,int *str_size, char * word){
	if((*str_size)<=strlen(*string)+strlen(word)+1){
		(*str_size)=(*str_size)*2;
		while(*str_size<=strlen(*string)+strlen(word)+1) *str_size=(*str_size)*2;
		*string=realloc(*string, (*str_size)*sizeof(char));
	}
	strcat(*string,word);
}

void  stat_myappend_pan_with_space(char **string,int *str_size, char * word){
	//printf("str len str: %d\n",strlen(string));
	if((*str_size)<=strlen(*string)+strlen(word)+1){
		(*str_size)=(*str_size)*2;
		while(*str_size<=strlen(*string)+strlen(word)+1) *str_size=(*str_size)*2;
		*string=realloc(*string, (*str_size)*sizeof(char));
	}
	strcat(*string," ");
	strcat(*string,word);
} 

