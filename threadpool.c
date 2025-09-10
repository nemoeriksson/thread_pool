#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "threadpool.h"

typedef struct Task 
{
	TaskFunction function;
	void *userdata;
} Task;

typedef struct ThreadpoolState
{
	Task *tasks;
	int head; // where you write
	int tail; // where you read
	unsigned int mask;
	int running;
	int thread_count;
	pthread_t *threads;
	pthread_mutex_t mutex;
}ThreadpoolState;

void sleep_ns(unsigned long long ns)
{
	const struct timespec spec = (struct timespec){
		0, ns	
	};
	
	nanosleep(&spec, NULL);
}

ThreadpoolState *create_threadpool_state(int thread_amount)
{
	ThreadpoolState *tps = malloc(sizeof(ThreadpoolState));

	tps->running = 1;
	tps->mask = 65536-1;
	tps->tasks = calloc(tps->mask+1, sizeof(Task));
	tps->head = 0;
	tps->tail = 0;
	tps->thread_count = thread_amount;
	tps->threads = calloc(tps->thread_count, sizeof(pthread_t));

	for (int p_index = 0; p_index < tps->thread_count; p_index++)
		pthread_create(&tps->threads[p_index], NULL, (void*)run_tasks_in_pool, tps);

	pthread_mutex_init(&tps->mutex, NULL);

	return tps;
}

void enqueue_task(ThreadpoolState *tps, TaskFunction function, void *userdata)
{
	pthread_mutex_lock(&tps->mutex);

	Task *task = &tps->tasks[tps->head & tps->mask];
	task->function = function;
	task->userdata = userdata;
	tps->head++;

	pthread_mutex_unlock(&tps->mutex);
}

static int has_tasks(ThreadpoolState *tps)
{
	return tps->head != tps->tail;
}

bool dequeue_task(ThreadpoolState *tps, Task *out)
{
	pthread_mutex_lock(&tps->mutex);
	if (!has_tasks(tps)) {
		pthread_mutex_unlock(&tps->mutex);
		return false;
	}
	*out = tps->tasks[tps->tail++ & tps->mask];
	pthread_mutex_unlock(&tps->mutex);
	return true;
}

void run_tasks_in_pool(ThreadpoolState *tps)
{
	int running_loaded;
	do {
		Task task;
		while (dequeue_task(tps, &task))
		{
			task.function(task.userdata);
		}
		
		__atomic_load(&tps->running, &running_loaded, __ATOMIC_RELAXED);
		
		sleep_ns(65536);
	}
	while (running_loaded);
}

void stop_pool(ThreadpoolState *tps)
{
	tps->running = 0;

	for (int p_index = 0; p_index < tps->thread_count; p_index++)
		pthread_join(tps->threads[p_index], NULL);
}
