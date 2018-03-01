#ifndef _SCHEDULEH_
#define _SCHEDULEH_
#include <pthread.h>
#include <errno.h>
#include "libraries.h"
#ifndef  _FUNCTIONSH_
#include "functions.h"
#endif
#include <sys/types.h>

typedef void(*Operation)(void*); //a pointer to a function returning void, and taking a string as param

void * get_a_job(void* queue);

typedef struct Job{
	
	Operation opt;
	void * arguments;
}Job;

typedef struct q_args{
	struct hash_layer *hash;
	char ***words;
	int number_of_words;
	topk_threads *top;
	int version;
	int start;
	int Q_number;
}q_args;

typedef struct q_args_static{
	struct static_hash_layer *hash;
	char ***words;
	int number_of_words;
	topk_threads *top;
	int start;
	int Q_number;
}q_args_static;

typedef struct Queue{
	int queue_capacity;
	int queue_used;
	int queue_ptr;
	int jobs_remain;
	Job ** jobs;
}Queue;


typedef struct JobScheduler{
int execution_threads; // number of execution threads
Queue* q; // a queue that holds submitted jobs / tasks
pthread_t* tids; // execution threads
}JobScheduler;

pthread_mutex_t pool_mutex; //stop other threads
pthread_mutex_t jobs_remain_mutex; //stop other threads

pthread_cond_t pool_is_empty;
pthread_cond_t pool_is_full;
pthread_cond_t jobs_are_done;

JobScheduler* initialize_scheduler( int execution_threads);
void submit_job( JobScheduler* sch, Job* j);
void execute_all_jobs( JobScheduler* sch);
void wait_all_tasks_finish(JobScheduler* sch); //waits all submitted tasks to finish
int destroy_scheduler( JobScheduler* sch);
void * get_a_job(void* queue);
#endif
