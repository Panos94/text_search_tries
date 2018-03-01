#include "top.h"

 int table_ngram_size = 200;

#define RED "\x1b[31m"


int  hash_gram(hashtable *hash_table, char *word)
{	
	int hash_value;
    unsigned long hash_int = 5381;
    int c;

    while (c = *word++)
        hash_int = ((hash_int << 5) + hash_int) + c; /* hash * 33 + c */
	
	hash_value=hash_int%(C2<<hash_table->split_round);

	int temp=hash_int%(C2<<(hash_table->split_round+1));

	if(temp<hash_table->number_of_buckets) return temp;

    return hash_value;
}


void swap(int a, int b){
	int tmp = a;
	a = b;
	b = tmp;
}

//__________________________________________ngram table
topk *  create_top(topk * top){
	int i;
	top =malloc(sizeof(topk));
	top->kf = malloc(sizeof(kframes));
	top->kf->ngrams = malloc(table_ngram_size*sizeof(char *));
	top->kf->k = malloc(table_ngram_size*sizeof(int));
	top->kf->capacity = table_ngram_size;
	top->kf->occupied = 0;
	top->kf->q = 0;
	top->kf->ends = malloc(sizeof(int));
	top->kf->ngrams_to_free=0;

	top->fr = malloc((sizeof(freq)));
	top->fr->frequency = malloc(table_ngram_size*sizeof(int));
	top->fr->ngram = malloc(table_ngram_size*sizeof(int));
	top->fr->unique=0;

	top->hash_table=malloc(sizeof(hashtable));
	top->hash_table->total_frames=0;
	top->hash_table->bucket_to_split=0;
	top->hash_table->split_round=0;
	top->hash_table->load_factor=0.9;
	top->hash_table->bucket_capacity=10;
	top->hash_table->number_of_buckets=C2;
	top->hash_table->buckets_to_free=C2;
	top->hash_table->buckets=malloc(top->hash_table->number_of_buckets*sizeof(bucket));
	bucket *hash_bucket;
	for(i=0;i<top->hash_table->number_of_buckets;i++){
		hash_bucket=&(top->hash_table->buckets[i]);
		hash_bucket->number_of_children=0;
		hash_bucket->capacity=top->hash_table->bucket_capacity;
		hash_bucket->children=malloc(hash_bucket->capacity*sizeof(int));
	}
	
	return top;
}


topk *  init_top(topk* top){
	int i;
	top->kf->occupied = 0;
	top->kf->q = 0;
	top->fr->unique=0;
	top->hash_table->total_frames=0;
	top->hash_table->bucket_to_split=0;
	top->hash_table->split_round=0;
	top->hash_table->number_of_buckets=C2;
	bucket *hash_bucket;
	for(i=0;i<top->hash_table->number_of_buckets;i++){
		 hash_bucket=&(top->hash_table->buckets[i]);
		 hash_bucket->number_of_children=0;
	}
	

	return top;
	}


topk *  extend_top(topk * top){
	table_ngram_size*=2;

	top->kf->capacity = table_ngram_size;
	top->kf->ngrams = (char **)realloc(top->kf->ngrams,top->kf->capacity*sizeof(char *));
	top->kf->k = realloc(top->kf->k,top->kf->capacity*sizeof(int));

	top->fr->frequency = realloc(top->fr->frequency,table_ngram_size*sizeof(int));
	top->fr->ngram = realloc(top->fr->ngram,table_ngram_size*sizeof(int));

	return top;
}

topk * add_top(topk * top,char * ngram){ //prosthiki enos n gram stous pinakes
	
	if(top->kf->occupied==(top->kf->capacity)){
		extend_top(top);
	}
	//printf("in add to top \"%s\"\n",ngram);
	int ngram_len=strlen(ngram)+1;
	int last_position=top->kf->occupied;

	if(last_position+1>top->kf->ngrams_to_free){
		top->kf->ngrams[last_position] = malloc(ngram_len*sizeof(char));
		top->kf->ngrams_to_free+=1;
	}
	else{ 
		top->kf->ngrams[last_position] = realloc(top->kf->ngrams[last_position],ngram_len*sizeof(char));
	}

	memmove(top->kf->ngrams[last_position],ngram,ngram_len);


	//top->fr->frequency[last_position] = 1;
	//top->fr->ngram[last_position] =last_position;
	//printf("I added :%s: in frames\n",ngram);
	top->kf->occupied++;
	
	
	//add in hash table if its new
	hashtable *hash_=top->hash_table;

	if((hash_->total_frames/((float)hash_->number_of_buckets*hash_->bucket_capacity)) > hash_->load_factor){
		int resize_error=resize_hash_for_top(hash_,top->kf,top->fr);
		if(resize_error==-1) return NULL;
	}	
	
	int j=0;
	int hash_value=hash_gram(top->hash_table,ngram);
	int last_in_bucket,pos;

	bucket *hash_bucket=&(top->hash_table->buckets[hash_value]);
	last_in_bucket=hash_bucket->number_of_children;
	//
	int pos2;
	//print_hash_table(top->hash_table,top);
	
	for(j=0;j<last_in_bucket;j++){
		pos=hash_bucket->children[j];
		pos2=top->fr->ngram[pos];
		if(strcmp(top->kf->ngrams[pos2],ngram)==0)
		{
			top->fr->frequency[pos]++;
			return top;
		}
	}
	
	top->fr->ngram[top->fr->unique] =last_position;
	top->fr->frequency[top->fr->unique] = 1;
	top->fr->unique++;
	if(last_in_bucket==hash_bucket->capacity){ 	//overflow bucket
		hash_bucket->children=realloc(hash_bucket->children,hash_bucket->capacity*2*sizeof(int));
		hash_bucket->capacity*=2;
	}

	hash_bucket->children[last_in_bucket]=top->fr->unique-1;//last_position;
	top->hash_table->total_frames++;
	hash_bucket->number_of_children++;

	return top;
}

void initialize_bucket_for_top(bucket *bucket_,int m){
	bucket_->children=malloc(m*sizeof(int));
	bucket_->number_of_children=0;
	bucket_->capacity=m;
}

int resize_hash_for_top(hashtable *hash_,kframes *kf,freq *fr){
	bucket *hash_bucket;
	//printf("free buckets %d , used %d\n",hash_->buckets_to_free,hash_->number_of_buckets);
	if(hash_->buckets_to_free==hash_->number_of_buckets){
		hash_->buckets=realloc(hash_->buckets,(hash_->number_of_buckets+1)*sizeof(bucket)); //add bucket lineat
		if(hash_->buckets==NULL){
			return -1;
		}
		initialize_bucket_for_top(&(hash_->buckets[hash_->number_of_buckets]),hash_->bucket_capacity);
		hash_->buckets_to_free++;
	}
	hash_->number_of_buckets++;
	
	int i;
	bucket *new_bucket=&(hash_->buckets[hash_->number_of_buckets-1]); //pointer to the new bucket
	new_bucket->number_of_children=0;
	new_bucket->capacity=hash_->bucket_capacity;

	hash_bucket=&(hash_->buckets[hash_->bucket_to_split]); //re arranging bucket to split
	int new_hash_val=-1;
	stack *stack_=init_stack();

	if(hash_bucket->number_of_children==0){
		hash_->bucket_to_split=(hash_->bucket_to_split+1)%(C2<<hash_->split_round);//without pow
		if(hash_->bucket_to_split==0 && hash_->number_of_buckets>C2) hash_->split_round++;
		stack_destroy(stack_); 
		return 1;
	} // no need for rearranging bucket
	
	int previous=hash_->bucket_to_split;
	hash_->bucket_to_split=(hash_->bucket_to_split+1)%(C2<<hash_->split_round);// without pow
	int pos,pos2;

	for(i=0;i<hash_bucket->number_of_children;i++){

		pos=hash_bucket->children[i];
		pos2=fr->ngram[pos];
		new_hash_val=hash_gram(hash_,kf->ngrams[pos2]);
		//printf("new hash val for \' %s\' is %d\n",kf->ngrams[pos2],new_hash_val);

		if(new_hash_val==previous) continue; //if hash value is the same then no need to change bucket
		if(new_bucket->number_of_children==new_bucket->capacity){	//if new bucket fills then create an overflow bucket
			new_bucket->children=realloc(new_bucket->children,hash_bucket->capacity*2*sizeof(int));
			if(new_bucket->children==NULL){
				stack_destroy(stack_); 
				return -1;
			}
			new_bucket->capacity=new_bucket->capacity*2;	
		}
		memmove(&(new_bucket->children[new_bucket->number_of_children]),&(hash_bucket->children[i]),sizeof(int)); //copy nod
		push(stack_,i);		
		new_bucket->number_of_children++;	
	}

	shrink_buckets_for_top(&(hash_->buckets[previous]),stack_);
	if(hash_->bucket_to_split==0 && hash_->number_of_buckets>C2) hash_->split_round++;
	stack_destroy(stack_); 
	return 1;
}


void shrink_buckets_for_top(bucket *bucket_,stack *stack_){
	int i;
	int pos;
	int *total=&(bucket_->number_of_children);
	for(i=stack_->top-1;i>=0;i--){
		pos=get_stack_elements(stack_,i);
		
		memmove(&(bucket_->children[pos]),&(bucket_->children[pos+1]),(*total-(pos+1))*sizeof(int));
		*total=*total-1;
		}
}

void print_hash_table(hashtable *hash_,topk *top){
	int i;
	bucket *hash_bucket;
	int j,pos,pos2;
	printf("in print\n");
	for(i=0;i<=hash_->number_of_buckets;i++){
		hash_bucket=&(hash_->buckets[i]);
		printf("bucket[%d]-> ",i);
		for(j=0;j<hash_bucket->number_of_children;j++){
			pos=hash_bucket->children[j];
			pos2=top->fr->ngram[pos];
			printf("\"%s\" (%d)->\n",top->kf->ngrams[pos2],top->fr->frequency[pos]);
		}
		printf("\n");
	}
}

topk *  erase_top(topk * top){
	int i;
	bucket *hash_bucket;
	for(i=0;i<top->hash_table->buckets_to_free;i++){
		hash_bucket=&(top->hash_table->buckets[i]);
		free(hash_bucket->children);
	}
	free(top->hash_table->buckets);
	free(top->hash_table);

	free(top->fr->frequency);
	free(top->fr->ngram);
	free(top->fr);
	//printf("to free are %d\n",top->kf->ngrams_to_free);
	for(i=0;i<top->kf->ngrams_to_free;i++) free(top->kf->ngrams[i]);

	free(top->kf->ngrams);
	free(top->kf->ends);
	free(top->kf->k);  
	free(top->kf);
	free(top);
	return top;
}


void print_print(topk * top){ //ektypwnei ola ta ngrams me
	int i;
	int j=0;
	int previous=0;
	//printf("occupied are %d\n",top->kf->occupied);
	
	for(i=0;i<top->kf->q;i++){
		//printf("I am gonna show %d frames \n",top->kf->ends[i]);
		if(top->kf->ends[i]==0){
			printf("-1\n");
			continue;
		}
		for(j=previous;j<previous+top->kf->ends[i]-1;j++){
			printf("%s|",top->kf->ngrams[j]);
		}
		j=previous+top->kf->ends[i]-1;
		printf("%s\n",top->kf->ngrams[j]);
		previous=previous+top->kf->ends[i];		
	}
}


void print_top(topk*top,int k){ //ektypwnei ola ta ngrams me
	int i;
	int max=k;
	if(top->fr->unique<max) max=top->fr->unique;//top->kf->occupied;
	if(max==0) return;
	quickSort(top->fr->frequency,top->fr->ngram,0,top->fr->unique-1,top->kf->ngrams);
	sort_in_alphabet(top->fr->frequency,top->fr->ngram,0,max,top->kf->ngrams);
	//print_frequencies(top);
	printf("Top: ");
	for(i=0;i<max;i++){
		printf("%s",top->kf->ngrams[top->fr->ngram[i]]);
		if(i==k-1)	
			break;
		printf("|");
		}
printf("\n");
}

void print_frequencies(topk*top){ //ektypwnei ola ta ngrams me
	int i,pos;
	for(i=0;i<15;i++){//top->fr->unique gia na ta ektyposei ola
		pos=top->fr->ngram[i];
		printf("word: \"%s\" with freq: %d \n",top->kf->ngrams[pos],top->fr->frequency[i]);
		}

}

void sort_in_alphabet(int *frequency,int *ngram,int l,int max,char **ngrams){
	int i,j,k;
	int compare,pos,pos2,temp,end;

	int start=0;
	for(i=0;i<=max;i++){
		start=i;
		while(frequency[i]==frequency[i+1]) i++;			
		
		end=i;
		if(start!=end){ //sort from start to end
  			 for (j=0;j<=(end-start)-1;j++){      
       			for (k=0;k<=(end-start)-j-1;k++){ 
					pos=ngram[k+start];
					pos2=ngram[k+start+1];
					compare=strcmp(ngrams[pos],ngrams[pos2]);
          			if (compare>0){
						temp = frequency[k+start];
						frequency[k+start] = frequency[k+start+1];
						frequency[k+start+1] = temp;
	
						temp = ngram[k+start];
						ngram[k+start] = ngram[k+start+1];
						ngram[k+start+1] = temp; 
					}
					}
				}
			}
		}
	}


void quickSort( int *frequency,int *ngram, int l, int r,char **ngrams)
{
	int j;
	//printf("Low : %d , High %d\n",l,r);
	if(l<r){
   	// divide and conquer
		j = partition(frequency,ngram,l,r,ngrams);
     	quickSort(frequency,ngram,l,j-1,ngrams);
     	quickSort(frequency,ngram,j+1,r,ngrams);
	}
}



int partition (int *frequency,int *ngram,int l,int r,char **ngrams){
    // pivot (Element to be placed at left position)
	int i,j,temp;
    int pivot = frequency[r];
    i =(l-1);  // Index of smaller element
    for (j=l;j<=r-1;j++)
    {
        // If current element is bigger than or
        // equal to pivot
        if (frequency[j]>=pivot)
        {
            i++;    // increment index of smaller element
			temp = frequency[i];
			frequency[i] = frequency[j];
			frequency[j] = temp;

			temp = ngram[i];
			ngram[i] = ngram[j];
			ngram[j] = temp; 
        }
    }
	
	temp = frequency[i+1];
	frequency[i+1] = frequency[r];
	frequency[r] = temp;

	temp = ngram[i+1];
	ngram[i+1] = ngram[r];
	ngram[r] = temp;
    return (i+1);
}




topk * increase_frequency(topk* top, char * ngram){
	int hash_value = hash_gram(top->hash_table,ngram);
	int i = 0;
	int position,position2;
	bucket *hash_bucket=&(top->hash_table->buckets[hash_value]);
	printf("I am looking for ngram %s\n",ngram);
	for(i=0;i<hash_bucket->number_of_children;i++){
		position=hash_bucket->children[i];
		position2=top->fr->ngram[position];
		if(strcmp(top->kf->ngrams[position2],ngram)==0){
			printf("found it in pos %d , %s\n",position2,top->kf->ngrams[position2]);
			top->fr->frequency[position]++;
			return top;
		}
	}
	
	printf(RED"Frequency not updated\n"RESET);
	return top;
}

topk *  end_gram_table(topk * top,int ngrams_found){ //simeiwnoume oti edw teleiwnei to Q, ara prepei stin ektypwsi na valoume allagi grammis
	//top->kf->ends[top->kf->q] = top->kf->occupied-1;
	top->kf->ends[top->kf->q] = ngrams_found;
	top->kf->ends = realloc(top->kf->ends,((top->kf->q)+1)*(sizeof(int*)));
	top->kf->q++;
	top->kf->ends[top->kf->q] = -1;
	return top;
}


//----------------------------------topk for threads----------------------------------------------------//

topk_threads *  create_top_threads(topk_threads * top){
	int i;
	top =malloc(sizeof(topk_threads));
	top->Q_capacity=32;
	top->kf = malloc(top->Q_capacity*sizeof(kframes_threads));
	for(i=0;i<top->Q_capacity;i++){
		//printf("table ngram size is %d\n",table_ngram_size);
		top->kf[i].ngrams = malloc(table_ngram_size*sizeof(char *));
		top->kf[i].capacity = table_ngram_size;
		top->kf[i].occupied = 0;
		top->kf[i].ngrams_to_free=0;
	}
	
	return top;
}

topk_threads *  init_top_threads(topk_threads* top){
	int i;
	for(i=0;i<top->Q_capacity;i++)
		top->kf[i].occupied = 0;
	/*top->fr->unique=0;
	top->hash_table->total_frames=0;
	top->hash_table->bucket_to_split=0;
	top->hash_table->split_round=0;
	top->hash_table->number_of_buckets=C2;
	/bucket *hash_bucket;
	for(i=0;i<top->hash_table->number_of_buckets;i++){
		 hash_bucket=&(top->hash_table->buckets[i]);
		 hash_bucket->number_of_children=0;
	}
	*/

	return top;
	}

topk_threads * add_top_threads(topk_threads * top,char * ngram,int Q_number){ //prosthiki enos n gram stous pinakes
	kframes_threads *kf=&(top->kf[Q_number]);
	if(kf->occupied==(kf->capacity)){
		//printf("Double in Q number %d\n",Q_number);
		kf=extend_top_kf_threads(kf); // check this
	}
	//printf("in add to top \"%s\"\n",ngram);
	int ngram_len=strlen(ngram)+1;
	int last_position=kf->occupied;

	if(last_position+1>kf->ngrams_to_free){
		kf->ngrams[last_position] = malloc(ngram_len*sizeof(char));
		kf->ngrams_to_free+=1;
	}
	else{ 
		kf->ngrams[last_position] = realloc(kf->ngrams[last_position],ngram_len*sizeof(char));
	}

	memmove(kf->ngrams[last_position],ngram,ngram_len);

	kf->occupied++;
	
	return top;
}

kframes_threads * extend_top_kf_threads(kframes_threads *kf){
	
	kf->capacity =kf->capacity<<1 ;
	kf->ngrams = (char **)realloc(kf->ngrams,kf->capacity*sizeof(char *));

	return kf;
}

void print_print_threads(topk_threads * top,int Q_used){ //ektypwnei ola ta ngrams me
	int i;
	int j;
	//printf("occupied are %d\n",top->kf->occupied);
	for(j=0;j<Q_used;j++){
		//printf("Q has %d grams and capacity %d\n",top->kf[j].occupied,top->kf[j].capacity);
	if(top->kf[j].occupied==0){
		printf("-1\n");
		continue;
	}
	for(i=0;i<top->kf[j].occupied-1;i++){
		//printf("%d %d: ",i,j);
		printf("%s|",top->kf[j].ngrams[i]);
	}
	i=top->kf[j].occupied-1;
	printf("%s\n",top->kf[j].ngrams[i]);
	}
}

void extend_top_threads(topk_threads *top,int new_capacity){
	int i;
	top->kf = realloc(top->kf,new_capacity*sizeof(kframes_threads));
	for(i=top->Q_capacity;i<new_capacity;i++){
		//printf("table_ngram_size extend is %d\n",table_ngram_size);
		top->kf[i].ngrams = malloc(table_ngram_size*sizeof(char *));
		top->kf[i].capacity = table_ngram_size;
		top->kf[i].occupied = 0;
		top->kf[i].ngrams_to_free=0;
	}
	top->Q_capacity=new_capacity;
	//printf("I can hold %d now\n",top->Q_capacity);
}

topk_threads *  erase_top_threads(topk_threads * top){
	int i,j;
	for(j=0;j<top->Q_capacity;j++){
		for(i=0;i<top->kf[j].ngrams_to_free;i++) free(top->kf[j].ngrams[i]);
		free(top->kf[j].ngrams);
	}
	free(top->kf);
	free(top);
	return top;
}

char ** merge_kframes_threads(topk_threads *top,int Q_used,int total,char **merged_ngrams){
	int i,j;
	int pos=0;
	for(i=0;i<Q_used;i++){
		for(j=0;j<top->kf[i].occupied;j++){
			merged_ngrams[pos]=top->kf[i].ngrams[j];
			pos++;
		}
	}
	return merged_ngrams; //delete it later
	
}


void print_merged(char **merged,int total){
	int i;	
	for(i=0;i<total;i++){
		printf("%s|",merged[i]);
	}
	printf("\n");
}

int get_total_ngrams(topk_threads *top,int Q_used){
	int i;
	int total_ngrams=0;
	for(i=0;i<Q_used;i++){
		total_ngrams+=top->kf[i].occupied;
	}
	return total_ngrams;
}

void create_threads_hashtable(topk_threads *top,int total_ngrams){
	int i;
	top->fr = malloc((sizeof(freq)));
	top->fr->frequency = malloc(total_ngrams*sizeof(int));
	top->fr->ngram = malloc(total_ngrams*sizeof(int));
	top->fr->unique=0;

	top->hash_table=malloc(sizeof(hashtable));
	top->hash_table->total_frames=0;
	top->hash_table->bucket_to_split=0;
	top->hash_table->split_round=0;
	top->hash_table->load_factor=0.9;
	top->hash_table->bucket_capacity=10;
	top->hash_table->number_of_buckets=C2;
	top->hash_table->buckets_to_free=C2;
	top->hash_table->buckets=malloc(top->hash_table->number_of_buckets*sizeof(bucket));
	bucket *hash_bucket;
	for(i=0;i<top->hash_table->number_of_buckets;i++){
		hash_bucket=&(top->hash_table->buckets[i]);
		hash_bucket->number_of_children=0;
		hash_bucket->capacity=top->hash_table->bucket_capacity;
		hash_bucket->children=malloc(hash_bucket->capacity*sizeof(int));
	}
	return ;

}

void built_hashtable(topk_threads *top, char **merged_ngrams,int total_ngrams){
	int i,j,pos,hash_value;
	int pos2,last_in_bucket,found;
	char *ngram;
	hashtable *hash_=top->hash_table;
	for(i=0;i<total_ngrams;i++){
		ngram=merged_ngrams[i];
		//printf("%s\n",ngram);

		if((hash_->total_frames/((float)hash_->number_of_buckets*hash_->bucket_capacity)) > hash_->load_factor){
			int resize_error=resize_hash_for_top_threads(hash_,merged_ngrams,top->fr);
			if(resize_error==-1) return ;
		}	
		
		hash_value=hash_gram(top->hash_table,ngram);

		bucket *hash_bucket=&(top->hash_table->buckets[hash_value]);
		last_in_bucket=hash_bucket->number_of_children;
	//
		//print_hash_table(top->hash_table,top);
		found=0;
		for(j=0;j<last_in_bucket;j++){
			pos=hash_bucket->children[j];
			pos2=top->fr->ngram[pos];
			if(strcmp(merged_ngrams[pos2],ngram)==0)
			{	
				top->fr->frequency[pos]++;
				found=1;
				break;
			}
		}
		if(found==1) continue;		
		
		top->fr->ngram[top->fr->unique] =i;
		top->fr->frequency[top->fr->unique] = 1;
		top->fr->unique++;
		if(last_in_bucket==hash_bucket->capacity){ 	//overflow bucket
			hash_bucket->capacity=hash_bucket->capacity<<1;
			hash_bucket->children=realloc(hash_bucket->children,hash_bucket->capacity*sizeof(int));
			//hash_bucket->capacity*=2;
		}
	
		hash_bucket->children[last_in_bucket]=top->fr->unique-1;//last_position;
		top->hash_table->total_frames++;
		hash_bucket->number_of_children++;
	
	}
}

void erase_hashtable_from_top(topk_threads *top){
	bucket *hash_bucket;
	int i;
	for(i=0;i<top->hash_table->buckets_to_free;i++){
		hash_bucket=&(top->hash_table->buckets[i]);
		free(hash_bucket->children);
	}
	free(top->hash_table->buckets);
	free(top->hash_table);

	free(top->fr->frequency);
	free(top->fr->ngram);
	free(top->fr);
	//printf("to free are %d\n",top->kf->ngrams_to_free);
	return;
}

void print_top_threads(topk_threads *top, char **merged_ngrams,int total_ngrams,int k){

	int i;
	int max=k;
	if(top->fr->unique<max) max=top->fr->unique;//top->kf->occupied;
	if(max==0) return;
	quickSort(top->fr->frequency,top->fr->ngram,0,top->fr->unique-1,merged_ngrams);
	sort_in_alphabet(top->fr->frequency,top->fr->ngram,0,max,merged_ngrams);
	//print_frequencies_threads(top,merged_ngrams);
	printf("Top: ");
	for(i=0;i<max;i++){
		printf("%s",merged_ngrams[top->fr->ngram[i]]);
		if(i==k-1)	
			break;
		printf("|");
		}
printf("\n");



}

void print_frequencies_threads(topk_threads *top,char **merged){ //ektypwnei ola ta ngrams me
	int i,pos;
	for(i=0;i<20;i++){//top->fr->unique gia na ta ektyposei ola
		pos=top->fr->ngram[i];
		//printf("pos is %d")
		printf("word: \"%s\" with freq: %d \n",merged[pos],top->fr->frequency[i]);
		}

}

int resize_hash_for_top_threads(hashtable *hash_,char **merged_ngrams,freq *fr){
	bucket *hash_bucket;
	//printf("free buckets %d , used %d\n",hash_->buckets_to_free,hash_->number_of_buckets);
	if(hash_->buckets_to_free==hash_->number_of_buckets){
		hash_->buckets=realloc(hash_->buckets,(hash_->number_of_buckets+1)*sizeof(bucket)); //add bucket lineat
		if(hash_->buckets==NULL){
			return -1;
		}
		initialize_bucket_for_top(&(hash_->buckets[hash_->number_of_buckets]),hash_->bucket_capacity);
		hash_->buckets_to_free++;
	}
	hash_->number_of_buckets++;
	
	int i;
	bucket *new_bucket=&(hash_->buckets[hash_->number_of_buckets-1]); //pointer to the new bucket
	new_bucket->number_of_children=0;
	new_bucket->capacity=hash_->bucket_capacity;

	hash_bucket=&(hash_->buckets[hash_->bucket_to_split]); //re arranging bucket to split
	int new_hash_val=-1;
	stack *stack_=init_stack();

	if(hash_bucket->number_of_children==0){
		hash_->bucket_to_split=(hash_->bucket_to_split+1)%(C2<<hash_->split_round);//without pow
		if(hash_->bucket_to_split==0 && hash_->number_of_buckets>C2) hash_->split_round++;
		stack_destroy(stack_); 
		return 1;
	} // no need for rearranging bucket
	
	int previous=hash_->bucket_to_split;
	hash_->bucket_to_split=(hash_->bucket_to_split+1)%(C2<<hash_->split_round);// without pow
	int pos,pos2;

	for(i=0;i<hash_bucket->number_of_children;i++){

		pos=hash_bucket->children[i];
		pos2=fr->ngram[pos];
		new_hash_val=hash_gram(hash_,merged_ngrams[pos2]);
		//printf("new hash val for \' %s\' is %d\n",kf->ngrams[pos2],new_hash_val);

		if(new_hash_val==previous) continue; //if hash value is the same then no need to change bucket
		if(new_bucket->number_of_children==new_bucket->capacity){	//if new bucket fills then create an overflow bucket
			new_bucket->children=realloc(new_bucket->children,hash_bucket->capacity*2*sizeof(int));
			if(new_bucket->children==NULL){
				stack_destroy(stack_); 
				return -1;
			}
			new_bucket->capacity=new_bucket->capacity*2;	
		}
		memmove(&(new_bucket->children[new_bucket->number_of_children]),&(hash_bucket->children[i]),sizeof(int)); //copy nod
		push(stack_,i);		
		new_bucket->number_of_children++;	
	}

	shrink_buckets_for_top(&(hash_->buckets[previous]),stack_);
	if(hash_->bucket_to_split==0 && hash_->number_of_buckets>C2) hash_->split_round++;
	stack_destroy(stack_); 
	return 1;
}


