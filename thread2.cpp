// file: thread2.cpp
// gcc -o thread2 thread2.cpp -lpthread

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *worker_thread(void *arg)
{
    pthread_exit((void*)911);
}

int main()
{
    int i;
    pthread_t thread;
    pthread_create(&thread, NULL, worker_thread, NULL);
    pthread_join(thread, (void **)&i);
    printf("%d\n",i);  // will print out 911
}