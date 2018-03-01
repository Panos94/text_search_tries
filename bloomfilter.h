#ifndef _BLOOMFILTERH_
#define _BLOOMFILTERH_

#define M 8192 //size of  bitvector bloomfilter in bits, should be good for ut to 8000 words
#include <stdint.h>
#include <math.h>
#ifndef VAR
#define VAR
#include "libraries.h"
#endif
#include "murmur3.h"

#define  log2 0.6931
//#define  logp 7 //the log(1/p) when p is 0.001
#define  logp 5 //the log(1/p) when p is 0.01
//#define  logp 3 //the log(1/p) when p is 0.1
#define P 0.001 //probability of false positive

int bloomfiltersize(int input_size);
void SetBit(int *A, int k);
void ClearBit(int *A,int k);
int TestBit(int *A,int k);
void bloomfilter_init(int * bloom,size_t bloom_size);
int mymod(char * input,int mod);

void bloomfilter_add(char * message,int *bloom,size_t bloom_size);
int bloomfilter_check(char * message,int *bloom,size_t bloom_size);
int TestAllBits(int *bloom,size_t bloom_size);
void hash2(const void *in_string, int *ptr, int key,size_t bloom_size);
unsigned long hash( char *str,int key,size_t bloom_size);
#endif

