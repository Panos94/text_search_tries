#include "deletion.h"


void init_ngrams_to_delete(ngrams_to_delete *d_grams){
	int i;
	d_grams->nodes_capacity=16; // capacity of 1 d array
	d_grams->length_capacity=16;	
	d_grams->pos=0;
	d_grams->word_size=25;
	d_grams->length=malloc(d_grams->length_capacity*sizeof(int)); // in each deletion 
	memset(d_grams->length,0,d_grams->length_capacity*sizeof(int));
	d_grams->nodes_to_delete=malloc(d_grams->nodes_capacity*sizeof(char*));
	for(i=0;i<d_grams->nodes_capacity;i++){
		d_grams->nodes_to_delete[i]=malloc(d_grams->word_size*sizeof(char));
	}
}

void add_ngram_to_delete(ngrams_to_delete *d_grams,char *node,int ngram){
	if(ngram==d_grams->length_capacity){
		d_grams->length=realloc(d_grams->length,d_grams->length_capacity*2*sizeof(int)); // in each deletion 
		memset(&(d_grams->length[d_grams->length_capacity]),0,d_grams->length_capacity*sizeof(int));	
		d_grams->length_capacity*=2;				
	}
	d_grams->length[ngram]++;
	if(d_grams->nodes_capacity==d_grams->pos){
		int i;
		d_grams->nodes_to_delete=realloc(d_grams->nodes_to_delete,d_grams->nodes_capacity*2*sizeof(char*));
		for(i=d_grams->nodes_capacity;i<d_grams->nodes_capacity*2;i++){
			d_grams->nodes_to_delete[i]=malloc(d_grams->word_size*sizeof(char));
		}
		d_grams->nodes_capacity*=2;
	}
	int word_len=strlen(node);
	if(word_len+1>d_grams->word_size) d_grams->nodes_to_delete[d_grams->pos]=realloc( d_grams->nodes_to_delete[d_grams->pos],word_len+1*sizeof(char));
	strcpy(d_grams->nodes_to_delete[d_grams->pos],node);
	d_grams->pos++;
}

void reset_ngrams_to_delete(ngrams_to_delete *d_grams){
	d_grams->pos=0;
	memset(d_grams->length,0,d_grams->length_capacity*sizeof(int));
}



void destroy_ngrams_to_delete(ngrams_to_delete *d_grams){
	free(d_grams->length);
	int i;
	for(i=0;i<d_grams->nodes_capacity;i++){
		free(d_grams->nodes_to_delete[i]);
	}
	free(d_grams->nodes_to_delete);
}

void print_ngrams_to_delete(ngrams_to_delete *d_grams){
	int i;
	printf("printing_ngrams to delete\n");
	int start=0;
	int j=0;
	while(d_grams->length[j]!=0){
	
		for(i=start;i<start+d_grams->length[j];i++){
			printf("ngram: %s in batch %d\n",d_grams->nodes_to_delete[i],j);	
		}
	start+=d_grams->length[j];
	j++;
	}
}

