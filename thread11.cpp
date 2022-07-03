// #include <pthread.h>
// #include <stdio.h>

// void *f1()
// {
//         printf("thread 1\n");
// }

// void *f2()
// {
//         printf("thread 2\n");
// }

// int main()
// {
//         pthread_t myThread[2];
//         pthread_create(&myThread[0], 0, f1, 0);
//         pthread_create(&myThread[1], 0, f2, 0);
//         pthread_join(myThread[0], 0);
//         pthread_join(myThread[1], 0);
//         return 0;
// }

#include <pthread.h>
#include <cstdio>

void *f1()
{
    printf("thread 1\n");
}

void *f2()
{
    printf("thread 2\n");
}

int main()
{
    pthread_t myThread[2];
    pthread_create(&myThread[0], 0, reinterpret_cast<void *(*)(void *)>(f1), 0);
    pthread_create(&myThread[1], 0, reinterpret_cast<void *(*)(void *)>(f2), 0);
    pthread_join(myThread[0], 0);
    pthread_join(myThread[1], 0);
    return 0;
}