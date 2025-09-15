#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <pthread.h>

#include <threadpool.h>

void testfunction(volatile int *a)
{
	printf("%d\n", *a);
}

int main(void)
{
	ThreadpoolState *tps = create_threadpool_state(4);

	int count = 16;
	
	int *ints = malloc(count * sizeof(int));
	
	for (int i = 0; i < count; i++) {
		ints[i] = i;
	}
	
	for (int i = 0; i < count; i++) {
		enqueue_task(tps, (TaskFunction)testfunction, &ints[i]);
	}

	stop_pool(tps);
}

