#pragma once

typedef void(*TaskFunction)(void*);
typedef struct Task Task;
typedef struct ThreadpoolState ThreadpoolState;

ThreadpoolState *create_threadpool_state(int thread_amount);

void enqueue_task(ThreadpoolState *tps, TaskFunction function, void *userdata);

int has_tasks(ThreadpoolState *tps);

void run_tasks_in_pool(ThreadpoolState *tps);

void stop_pool(ThreadpoolState *tps);
