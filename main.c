#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "threadpool.h"

void testfunction(int *a)
{
	printf("%d\n", *a);
}

int main(void)
{
	ThreadpoolState *tps = create_threadpool_state(4);
	
	int a = 3;
	int b = 2;
	
	enqueue_task(tps, (TaskFunction)testfunction, &a);
	enqueue_task(tps, (TaskFunction)testfunction, &b);

	stop_pool(tps);
}

