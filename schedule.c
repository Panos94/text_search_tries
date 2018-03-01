#include "schedule.h"
#include <unistd.h>
#include <sys/wait.h>

extern int threads_quantity;

//στην function.c tha metraw posa Q exw kai tha kanw malloc Q*sizeof Job mia fora stin functions. tha pernaw ayton ton pinaka stin submit jobs kai ola cool.

void * get_a_job(void* queue) {
Queue * q;
q = (Queue*)queue;
int my_job;
void *args;

	while(1)
	{		
			//printf("waiting to be full %d\n",self);
			pthread_mutex_lock(&pool_mutex);
			while(q->queue_used==0){
				//printf("q->used =0 \n");
				q->queue_ptr=0;

				pthread_mutex_lock(&jobs_remain_mutex);
				if(q->jobs_remain==0) pthread_cond_signal(&pool_is_empty);
				pthread_mutex_unlock(&jobs_remain_mutex);

				pthread_cond_wait(&pool_is_full,&pool_mutex);
			}
			//printf("I got the lock with q->used = %d , %d\n",q->queue_used,self);
			if(q->queue_used>0){
				my_job=q->queue_ptr;
				//printf("Queue used %d\n",q->queue_used);
				//printf("i took job %d , %d\n",my_job,self);
				q->queue_used--;
				q->queue_ptr++;
				//printf("i unlocked the mutex %d\n",self);
				pthread_mutex_unlock(&pool_mutex);
				pthread_cond_signal(&pool_is_full);
			 	args=(void *)q->jobs[my_job]->arguments;
				q->jobs[my_job]->opt(args);
				free(q->jobs[my_job]->arguments);
				free(q->jobs[my_job]);

				pthread_mutex_lock(&jobs_remain_mutex);
				q->jobs_remain--;
				//printf("Done job = %d\n",my_job);
				pthread_mutex_unlock(&jobs_remain_mutex);		
				}
			else if(q->queue_used==-1){
				//printf("Thread dies %d\n",self);
				//fflush(stdout);
				//printf("%d unlocked pool\n",self);	 
				pthread_mutex_unlock(&pool_mutex);
				pthread_cond_signal(&pool_is_full);
				pthread_exit(0);
				return 0;
				}
			else {printf("ODD with %d\n",q->queue_used);}

		

	}
}




JobScheduler* initialize_scheduler(int execution_threads){
	int i;

	pthread_mutex_init(&pool_mutex, NULL);
	pthread_mutex_init(&jobs_remain_mutex, NULL);
	pthread_cond_init(&pool_is_empty, NULL);
	pthread_cond_init(&pool_is_full, NULL);
	pthread_cond_init(&jobs_are_done,NULL);

	JobScheduler * Scheduler = malloc(sizeof(JobScheduler));
	Scheduler->execution_threads = execution_threads;
	Scheduler->q = malloc(sizeof(Queue));
	Scheduler->q->queue_capacity = 20;
	Scheduler->q->queue_used = 0;
	Scheduler->q->queue_ptr = 0;
	Scheduler->q->jobs_remain = 0;
	Scheduler->q->jobs = malloc(Scheduler->q->queue_capacity*sizeof(Job * ));
	Scheduler->tids = malloc(execution_threads*sizeof(pthread_t*));

	pthread_mutex_lock(&pool_mutex);
	for(i=0;i<execution_threads;i++){
		pthread_create(&Scheduler->tids[i], NULL, get_a_job, Scheduler->q);
			
	}
	//printf("End JS init\n");
	return Scheduler;
}

void extend_queue(Queue * q){
	q->queue_capacity *=2;
	q->jobs = realloc(q->jobs,q->queue_capacity*sizeof(Job ));
}

void submit_job(JobScheduler* sch, Job* j){
	//printf("Submit Job init\n");
	if(sch->q->queue_capacity==sch->q->queue_used)
		extend_queue(sch->q);
//	sch->q->jobs[sch->q->queue_used] = malloc(sizeof(Job));
	Job *temp_job=malloc(sizeof(Job));
	memcpy(temp_job,j,sizeof(Job));
	sch->q->jobs[sch->q->queue_used]=temp_job;

	sch->q->queue_used++;
	sch->q->jobs_remain++;
	
}

void execute_all_jobs( JobScheduler* sch){
	wait_all_tasks_finish(sch);
	return ;
}

void destroy_threads( JobScheduler* sch){
	
	sch->q->queue_used = -1;
	int i;
	//printf("signal to destroy \n");
	pthread_cond_broadcast(&pool_is_full);
	pthread_mutex_unlock(&pool_mutex);
	//printf("main unlocked pool\n");
	int error;
	for(i=0;i<sch->execution_threads;i++){
		error=pthread_join(sch->tids[i], NULL);
		if(error!=0){
			printf("ERROR %d\n",error);		
			continue;
		}
		//printf("success in waiting thread %d\n",i);
	}
	pthread_mutex_destroy(&pool_mutex);
	pthread_mutex_destroy(&jobs_remain_mutex);
	pthread_cond_destroy(&pool_is_full);
	pthread_cond_destroy(&pool_is_empty);
	pthread_cond_destroy(&jobs_are_done);
	
	return ;
}

void wait_all_tasks_finish(JobScheduler* sch){ //waits all submitted tasks to finish
	//printf("wait for jobs to finish\n");
	while(sch->q->queue_used!=0){
		pthread_cond_broadcast(&pool_is_full);
		pthread_cond_wait(&pool_is_empty,&pool_mutex);
	}
	//printf("jobs finished\n");
}

int destroy_scheduler( JobScheduler* sch){

	destroy_threads(sch);
	free(sch->q->jobs);
	free(sch->q);
	free(sch->tids);
	free(sch);
return 0;
}

