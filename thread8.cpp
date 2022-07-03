#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void *thread_fnc(void * arg);

pthread_mutex_t my_mutex;
char my_string[100];  /* shared variable */
int time_to_exit = 0;

int main()
{
        int ret;
        pthread_t my_thread;
        void *ret_join;

        ret = pthread_mutex_init(&my_mutex, NULL);
        if(ret != 0) {
                perror("mutex init failed\n");
                exit(EXIT_FAILURE);
        }

        ret =  pthread_create(&my_thread, NULL, thread_fnc, NULL);
        if(ret != 0) {
                perror("pthread_create failed\n");
                exit(EXIT_FAILURE);
        }
        pthread_mutex_lock(&my_mutex);
        printf("Type in some characters. Enter 'quit' to finish\n");
        while(!time_to_exit) {
                fgets(my_string, 100, stdin);
                pthread_mutex_unlock(&my_mutex);
                while(1) {
                        if(my_string[0] != '\0') {
                                pthread_mutex_unlock(&my_mutex);
                                sleep(1);
                        }
                        else
                                break;
                }
        }
        pthread_mutex_unlock(&my_mutex);

        printf("Waiting for thread to finish...\n");
        ret = pthread_join(my_thread, &ret_join);
        if(ret != 0) {
                perror("pthread_join failed");
                exit(EXIT_FAILURE);
        }
        printf("Thread joined\n");
        pthread_mutex_destroy(&my_mutex);
        exit(EXIT_SUCCESS);
}

void *thread_fnc(void *arg)
{
        sleep(1);
        pthread_mutex_lock(&my_mutex);
        while(strncmp("quit", my_string, 4) != 0) {
                printf("You typed in %d characters\n",strlen(my_string)-1);
                my_string[0]='\0';
                pthread_mutex_unlock(&my_mutex);
                sleep(1);
                pthread_mutex_lock(&my_mutex);
                if(my_string[0] != '\0') {
                        pthread_mutex_unlock(&my_mutex);
                        sleep(1);
                        pthread_mutex_lock(&my_mutex);
                }
        }
        time_to_exit = 1;
        my_string[0] = '\0';
        pthread_mutex_unlock(&my_mutex);
        pthread_exit(NULL);
}