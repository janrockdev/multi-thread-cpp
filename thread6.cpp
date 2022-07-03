 #include <pthread.h>

pthread_spinlock_t slock;

void splock()
{
    int i = 100;
    while(i>0) {
        pthread_spin_lock(&slock);
        i--;
        pthread_spin_unlock(&slock);
    }
}

int main()
{
    pthread_spin_init(&slock, PTHREAD_PROCESS_PRIVATE);
    splock();
    pthread_spin_destroy(&slock);
    return 0;
}