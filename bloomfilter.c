#include "bloomfilter.h"

#include <stdio.h>
#include <stdlib.h>

int bloomfiltersize(int input_size){
	float m;
	m = (float)input_size*logp;
	m/=((log2)*(log2));
	int temp = ceil(m);
	int ret = temp/8;

	
	ret = (ret+1)<<3;// *8;
	return (ret<<2);
}



int hashfunctionssize(int input_size){
	float k = bloomfiltersize(input_size)*log2;
	k/=(float)input_size;
	int ret = k;

	if(k>8) {
		return 8;
	}
	return k;
}


void SetBit(int *A, int k){	// 32 is sizeof(int)
	int i =k/32;//k>>5;            // i = array index (use: A[i])
	int pos = k%32;          // pos = bit position in A[i]
	unsigned int flag = 1;   // flag = 0000.....00001
	flag = flag << pos;      // flag = 0000...010...000   (shifted k positions)
	A[i] = A[i] | flag;      // Set the bit at the k-th position in A[i]
}

void ClearBit(int *A,int k){
	int i = k/32;//>>5;///32;
	int pos = k%32;
	unsigned int flag = 1;  // flag = 0000.....00001
	flag = flag << pos;     // flag = 0000...010...000   (shifted k positions)
	flag = ~flag;           // flag = 1111...101..111
	A[i] = A[i] & flag;     // RESET the bit at the k-th position in A[i]
}


int TestBit(int *A,int k){
	int i = k/32;//>>5; ///32;
	int pos = k%32;
	unsigned int flag = 1;  // flag = 0000.....00001
	flag = flag << pos;     // flag = 0000...010...000   (shifted k positions)
	if ( A[i] & flag )      // Test the bit at the k-th position in A[i]
		return 1;
	else
		return 0;
}

int TestAllBits(int *bloom,size_t bloom_size){
	int i;
	int a=bloom_size;
	a/=8;
	a/=sizeof(int);
	for (i=0;i<bloom_size;i++){
		if(TestBit(bloom,i)!=0) {printf("Problem Houston");
			return 1; }//paizei kai na uparxei
	}
return 0; //sigoura den uparxei
}





void bloomfilter_init(int * bloom,size_t bloom_bytes){
	memset(bloom,0,bloom_bytes);
}

int mymod(char * input,int mod)
{
	int a = 0;
	int len = strlen(input);
	int ret = 0;
	int c;
	int dec = 0;

	for(a=0;a<len;a++){
		if(input[a]>57)
			c = (input[a]-'a');
		else 
			c = (input[a]-'0');
		
		ret += (c<<4); //*16
		ret = ret % mod;
		dec +=(c<<4); //*16
printf("%d %s %d %d\n",ret, input, dec, mod);
	}

return ret;
}


void hash2(const void *in_string, int *ptr, int key,size_t bloom_size){

	uint64_t * hash =  malloc(2*sizeof(uint64_t));

	MurmurHash3_x64_128(in_string,strlen(in_string),key,hash);
	//uint64_t var = hash[0]%M;
	ptr[0] = hash[0]%bloom_size;
	ptr[1] = hash[1]%bloom_size;
	free(hash);


}



void bloomfilter_add(char * message,int *bloom,size_t bloom_size){

	int lim = hashfunctionssize(bloom_size);

	int *hashvalue1 = malloc(16*sizeof(int));

		hash2(message,hashvalue1,41,bloom_size);
		hash2(message,hashvalue1+2,283,bloom_size);
		hash2(message,hashvalue1+4,131,bloom_size);
		hash2(message,hashvalue1+6,907,bloom_size);
		if(lim>8){
			hash2(message,hashvalue1+8,211,bloom_size);
			hash2(message,hashvalue1+10,853,bloom_size);
			hash2(message,hashvalue1+12,59,bloom_size);
			hash2(message,hashvalue1+14,997,bloom_size);
		}
int i;
for(i=0;i<lim;i++){
	SetBit(bloom,hashvalue1[i]);
}
	free(hashvalue1);
}


int bloomfilter_check(char * message,int *bloom,size_t bloom_size){

	int lim = hashfunctionssize(bloom_size);
	int *hashvalue1 = malloc(16*sizeof(int));

		hash2(message,hashvalue1,41,bloom_size);
		hash2(message,hashvalue1+2,283,bloom_size);
		hash2(message,hashvalue1+4,131,bloom_size);
		hash2(message,hashvalue1+6,907,bloom_size);
		if(lim>8){
			hash2(message,hashvalue1+8,211,bloom_size);
			hash2(message,hashvalue1+10,853,bloom_size);
			hash2(message,hashvalue1+12,59,bloom_size);
			hash2(message,hashvalue1+14,997,bloom_size);
		}
int i;
for(i=lim-1; i>=0; i--){
	if(TestBit(bloom,hashvalue1[i])==0){
		free(hashvalue1);
	return 0;
	}

}
	free(hashvalue1);
	return 1;


}

