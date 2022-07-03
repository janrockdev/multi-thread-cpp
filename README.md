# Multi-thread CPP (Pthread)

## What are Pthreads?
POSIX Threads, or Pthreads, is a POSIX standard for threads. The standard, POSIX.1c, Threads extensions (IEEE Std 1003.1c-1995), defines an API for creating and manipulating threads.
Implementations of the API are available on many Unix-like POSIX systems such as FreeBSD, NetBSD, GNU/Linux, Mac OS X and Solaris, but Microsoft Windows implementations also exist. For example, the pthreads-w32 is available and supports a subset of the Pthread API for the Windows 32-bit platform.
The POSIX standard has continued to evolve and undergo revisions, including the Pthreads specification. The latest version is known as IEEE Std 1003.1, 2004 Edition.
Pthreads are defined as a set of C language programming types and procedure calls, implemented with a pthread.h header file. In GNU/Linux, the pthread functions are not included in the standard C library. They are in libpthrea, therefore, we should add -lpthread to link our program.

## The Pthread API
Pthreads API can be grouped into four:

### Thread management:
Routines that work directly on threads - creating, detaching, joining, etc. They also include functions to set/query thread attributes such as joinable, scheduling etc.

### Mutexes:
Routines that deal with synchronization, called a "mutex", which is an abbreviation for "mutual exclusion". Mutex functions provide for creating, destroying, locking and unlocking mutexes. These are supplemented by mutex attribute functions that set or modify attributes associated with mutexes.

### Condition variables:
Routines that address communications between threads that share a mutex. Based upon programmer specified conditions. This group includes functions to create, destroy, wait and signal based upon specified variable values. Functions to set/query condition variable attributes are also included.

### Synchronization:
Routines that manage read/write locks and barriers.

## Creating Threads

1) Our main() program is a single, default thread. All other threads must be explicitly created by the programmer.

2) pthread_create creates a new thread and makes it executable. This routine can be called any number of times from anywhere within our code.

3) pthread_create (pthread_t *thread, pthread_attr_t *attr, void *(*start_routine)(void *), void *arg) arguments: 

a) thread: 
An identifier for the new thread returned by the subroutine. This is a pointer to pthread_t structure. When a thread is created, an identifier is written to the memory location to which this variable points. This identifier enables us to refer to the thread.

b) attr: 
An attribute object that may be used to set thread attributes. We can specify a thread attributes object, or NULL for the default values.

c) start_routine: 
The routine that the thread will execute once it is created.

```c++
void *(*start_routine)(void *)
```
        
We should pass the address of a function taking a pointer to void as a parameter and the function will return a pointer to void. So, we can pass any type of single argument and return a pointer to any type. 
While using fork() causes execution to continue in the same location with a different return code, using a new thread explicitly provides a pointer to a function where the new thread should start executing.

d) arg: 
A single argument that may be passed to start_routine. It must be passed as a void pointer. NULL may be used if no argument is to be passed.

4) The maximum number of threads that may be created by a process is implementation dependent.

5) Once created, threads are peers, and may create other threads. There is no implied hierarchy or dependency between threads.

6) Here is a sample of creating a child thread:

```c++
//file: thread0.cpp
//compile: multi-thread-cpp$ gcc -o thread0 thread0.cpp -lpthread

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *worker_thread(void *arg)
{
        printf("This is worker_thread()\n");
        pthread_exit(NULL);
}

int main()
{
        pthread_t my_thread;
        int ret;

        printf("In main: creating thread\n");
        ret =  pthread_create(&my_thread, NULL, &worker_thread, NULL);
        if(ret != 0) {
                printf("Error: pthread_create() failed\n");
                exit(EXIT_FAILURE);
        }

        pthread_exit(NULL);
}
```

Profiler:
```
dev@localhost:~/multi-thread-cpp$ valgrind --tool=helgrind -s ./thread0
==621010== Helgrind, a thread error detector
==621010== Copyright (C) 2007-2017, and GNU GPL'd, by OpenWorks LLP et al.
==621010== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
==621010== Command: ./thread0
==621010== 
In main: creating thread
This is worker_thread()
==621010== 
==621010== Use --history-level=approx or =none to gain increased speed, at
==621010== the cost of reduced accuracy of conflicting-access information
==621010== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 54 from 28)
--621010-- 
--621010-- used_suppression:     54 helgrind---...-*Unwind*-*pthread_unwind* /usr/lib/x86_64-linux-gnu/valgrind/default.supp:1107
==621010== 
==621010== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 54 from 28)
```

In the code, the main thread will create a second thread to execute worker_thread(), which will print out its message while main thread prints another. The call to create the thread has a NULL value for the attributes, which gives the thread default attributes. The call also passes the address of a my_thread variable for the worker_thread() to store a handle to the thread. The return value from the pthread_create() call will be zero if it's successful, otherwise, it returns an error.

We can create several child threads:

```c++
//file: thread1.cpp
//compile: multi-thread-cpp$ gcc -o thread1 thread1.cpp -lpthread

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5

void *worker_thread(void *arg)
{
        printf("This is worker_thread #%ld\n", (long)arg);
        pthread_exit(NULL);
}

int main()
{
        pthread_t my_thread[N];

        long id;
        for(id = 1; id <= N; id++) {
                int ret =  pthread_create(&my_thread[id], NULL, &worker_thread, (void*)id);
                if(ret != 0) {
                        printf("Error: pthread_create() failed\n");
                        exit(EXIT_FAILURE);
                }
        }

        pthread_exit(NULL);
}
```

Output is:

```
$ ./thread01
This is worker_thread #5
This is worker_thread #4
This is worker_thread #3
This is worker_thread #2
This is worker_thread #1
```

Note that, in the code, we pass the parameter (thread id) to the child thread.

If we do (void*)&id, it's a wrong way of passing data to the child thread. It passes the address of variable id, which is shared memory space and visible to all threads. As the loop iterates, the value of this memory location changes, possibly before the created threads can access it.

## Attributes of Threads

1) By default, a thread is created with certain attributes. Some of these attributes can be changed by the programmer via the thread attribute object.

2) pthread_attr_init() and pthread_attr_destroy() are used to initialize/destroy the thread attribute object.

3) Other routines are then used to query/set specific attributes in the thread attribute object.

## Terminating Threads

1) There are several ways in which a Pthread may be terminated:
a)The thread returns from its starting routine (the main routine for the initial thread).
b)The thread makes a call to the pthread_exit subroutine.
c) The thread is canceled by another thread via the pthread_cancel routine
d) The entire process is terminated due to a call to either the exec or exit subroutines.

2) pthread_exit is used to explicitly exit a thread. Typically, the pthread_exit() routine is called after a thread has completed its work and is no longer required to exist. If main() finishes before the threads it has created, and exits with pthread_exit(), the other threads will continue to execute. Otherwise, they will be automatically terminated when main() finishes. 
So, if we comment out the line pthread_exit() in main() in the thread01.c of the previous example code, the threads created may not have a chance to execute their work before being terminated.
3) The programmer may optionally specify a termination status, which is stored as a void pointer for any thread that may join the calling thread.
4) Cleanup: the pthread_exit() routine does not close files; any files opened inside the thread will remain open after the thread is terminated.

## Join

1) int pthread_join (pthread_t th, void **thread_return)
The first parameter is the thread for which to wait, the identified that pthread_create filled in for us. The second argument is a pointer to a pointer that itself points to the return value from the thread. This function returns zero for success and an error code on failure.
2) When a thread is created, one of its attributes defines whether the thread is joinable or detached. Only threads that are created as joinable can be joined. If a thread is created as detached, it can never be joined.
3) The final draft of the POSIX standard specifies that threads should be created as joinable.
4) To explicitly create a thread as joinable or detached, the attr argument in the pthread_create() routine is used. The typical 4 step process is:
a) Declare a pthread attribute variable of the pthread_attr_t data type.
b) Initialize the attribute variable with pthread_attr_init().
c) Set the attribute detached status with pthread_attr_setdetachstate()
d) When done, free library resources used by the attribute with pthread_attr_destroy()
5) Here is the summary for the join related functions:
a) pthread_join (threadid,status)
b) pthread_detach (threadid)
c) pthread_attr_setdetachstate (attr,detachstate)
d) pthread_attr_getdetachstate (attr,detachstate)

Picture from https://computing.llnl.gov/tutorials/pthreads/


A thread can execute a thread join to wait until the other thread terminates. In our case, you - the main thread - should execute a thread join waiting for your colleague - a child thread - to terminate. In general, thread join is for a parent (P) to join with one of its child threads (C). Thread join has the following activities, assuming that a parent thread P wants to join with one of its child threads C:

When P executes a thread join in order to join with C, which is still running, P is suspended until C terminates. Once C terminates, P resumes.
When P executes a thread join and C has already terminated, P continues as if no such thread join has ever executed (i.e., join has no effect).
A parent thread may join with many child threads created by the parent. Or, a parent only join with some of its child threads, and ignore other child threads. In this case, those child threads that are ignored by the parent will be terminated when the parent terminates.

The pthread_join() subroutine blocks the calling thread until the specified thread terminates.
The programmer is able to obtain the target thread's termination return status if it was specified in the target thread's call to pthread_exit() as show here:

```c++
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
```

A joining thread can match one pthread_join() call. It is a logical error to attempt multiple joins on the same thread.

```c++
thread3.cpp
```

## Detaching
There are cases we have to resynchronize our threads using pthread_join() before allowing the program to exit. We need to do this if we want to allow one thread to return data to the thread that created it. However, sometimes we neither need the second thread to return information to the main thread nor want the main thread to wait for it.

Suppose we create a second thread to spool a backup copy of a data file that is being edited while the main thread continues to service the user. When the backup has finished, the second thread can just terminate, and there is no need for it to join the main thread.

We can create threads that have this behavior. They are called detached threads, and we can create them by modifying the thread attributes or by calling pthread_detach().

The pthread_detach() routine can be used to explicitly detach a thread even though it was created as joinable.
There is no converse routine.

## pthread_join() sample code
The example below is using pthread_join() to wait for it to finish. The newly created thread is sharing global variable with the original thread. It modifies the variable.

```c++
// file: thread4.cpp
// gcc -o thread4 thread4.cpp -lpthread

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void *thread_fnc(void * arg);

char thread_msg[] ="Hello Thread!";

char exit_msg[] = "Exit from thread";

int main()
{
        int ret;
        pthread_t my_thread;
        void *ret_join;

        ret =  pthread_create(&my_thread, NULL, thread_fnc, (void*) thread_msg);
        if(ret != 0) {
                perror("pthread_create failed\n");
                exit(EXIT_FAILURE);
        }
        printf("Waiting for thread to finish...\n");
        ret = pthread_join(my_thread, &ret_join);
        if(ret != 0) {
                perror("pthread_join failed");
                exit(EXIT_FAILURE);
        }
        printf("Thread joined, it returned: %s\n", (char *) ret_join);
        printf("New thread message: %s\n",thread_msg);
        exit(EXIT_SUCCESS);
}

void *thread_fnc(void *arg)
{
        printf("This is thread_fnc(), arg is: %s\n", (char*) arg);
        strcpy(thread_msg,"Bye!");
        pthread_exit(exit_msg); //"'Exit from thread'"
}
```

Output is:
```
Waiting for thread to finish...
This is thread_fnc(), arg is: Hello Thread!
Thread joined, it returned: Exit from thread
New thread message: Bye!
```

We declare a prototype for the function that the thread calls when we create it:

```
void *thread_fnc(void * arg);
```

It takes a pointer to void as its argument and returns a pointer to void, which is required by pthread_create().

In main(), we call pthread_create() to start running our new thread:

```
ret =  pthread_create(&my;_thread, NULL, thread_fnc, (void*) thread_msg);
```

We are passing the address of a pthread_t object that we can use to refer to the thread later. For the thread attribute, we pass NULL since we do not want to modify the default values.

If the call succeeds, two threads will be running. The original thread (main) continues and execute the code after pthread_create(), and a new thread starts executing in the thread_fnc().

The original thread checks if the new thread has started, and then calls pthread_join():

```
ret = pthread_join(my_thread, &ret;_join);
```

We pass the identifier of the thread that we are waiting to join and a pointer to a result. This function will wait until the other thread terminates before it returns. Then, it prints the return value from the thread.

The new thread starts executing at the start of thread_fnc(), which updates global variable, returning a string to the main thread.

## Synchronization with Mutex

The mutual exclusion lock is the simplest and most primitive synchronization variable. It provides a single, absolute owner for the section of code (aka a critical section) that it brackets between the calls to pthread_mutex_lock() and pthread_mutex_unlock(). The first thread that locks the mutex gets ownership, and any subsequent attempts to lock it will fail, causing the calling thread to go to sleep. When the owner unlocks it, one of the sleepers will be awakened, made runnable, and given the chance to obtain ownership.

## Synchronization Pthread Example - Mutexes 1
A mutex lock is a mechanism that can be acquired by only one thread at a time. For other threads to get the same mutex, they must wait until it is released by the current owner of the mutex.

The key advantage of multithreading code is that all threads see the same memory. So, data is already shared between threads. But the failure of coordinating the access to the data can lead to incorrect results due to the reason such as data reaces. The mutex lock is one of ways of synchronizing data sharing methods.

```c++
// file: thread5a.cpp
// gcc -o thread5a thread5a.cpp -lpthread
// ignore warnings - just example
#include <stdio.h>
#include <pthread.h>

volatile int counter = 0;
pthread_mutex_t myMutex;

void *mutex_testing(void *param)
{
        int i;
        for(i = 0; i < 5; i++) {
                //pthread_mutex_lock(&myMutex);
                counter++;
                printf("thread %d counter = %d\n", (long)param,  counter);
                //pthread_mutex_unlock(&myMutex);
        }
}

int main()
{
        int one = 1, two = 2, three = 3;
        pthread_t thread1, thread2, thread3;
        pthread_mutex_init(&myMutex,0);
        pthread_create(&thread1, 0, mutex_testing, (void*)one);
        pthread_create(&thread2, 0, mutex_testing, (void*)two);
        pthread_create(&thread3, 0, mutex_testing, (void*)three);
        pthread_join(thread1, 0);
        pthread_join(thread2, 0);
        pthread_join(thread3, 0);
        pthread_mutex_destroy(&myMutex);
        return 0;
}
```

The code shows a mutex lock protecting the variable count against simulation access by multiple threads. Note that the count is declared as volatile to ensure that it is read from memory at each access and written back to memory after each access. There would be a data race between the threads if we're not using mutex lock.

Output is:
```
thread 1 counter = 1
thread 1 counter = 4
thread 1 counter = 5
thread 1 counter = 6
thread 1 counter = 7
thread 2 counter = 2
thread 2 counter = 8
thread 2 counter = 9
thread 2 counter = 10
thread 2 counter = 11
thread 3 counter = 3
thread 3 counter = 12
thread 3 counter = 13
thread 3 counter = 14
thread 3 counter = 15
```

If we do not use the lock and there are some more steps after incrementing counter. We may get the different results. In the code below, we used usleep(1), 1 ms sleep to represents other steps.

```c++
// file: thread5b.cpp
// gcc -o thread5b thread5b.cpp -lpthread
// ignore warnings - just example
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

volatile int counter = 0;
pthread_mutex_t myMutex;

void *mutex_testing(void *param)
{
        int i;
        for(i = 0; i < 5; i++) {
                //pthread_mutex_lock(&myMutex;);
                counter++ ;
                usleep(1);
                printf("thread %d counter = %d\n", (long)param,  counter);
                //pthread_mutex_unlock(&myMutex;);
        }
}

int main()
{
        int one = 1, two = 2, three = 3;
        pthread_t thread1, thread2, thread3;
        pthread_mutex_init(&myMutex,0);
        pthread_create(&thread1, 0, mutex_testing, (void*)one);
        pthread_create(&thread2, 0, mutex_testing, (void*)two);
        pthread_create(&thread3, 0, mutex_testing, (void*)three);
        pthread_join(thread1, 0);
        pthread_join(thread2, 0);
        pthread_join(thread3, 0);
        pthread_mutex_destroy(&myMutex);
        return 0;
}
```

Output is:
```
thread 1 counter = 3
thread 1 counter = 4
thread 1 counter = 5
thread 1 counter = 6
thread 2 counter = 3
thread 3 counter = 3
thread 3 counter = 8
thread 3 counter = 9
thread 3 counter = 10
thread 3 counter = 11
thread 2 counter = 8
thread 2 counter = 9
thread 2 counter = 10
thread 2 counter = 11
thread 1 counter = 11
```

As we can see, any thread can increment the counter variable at any time. Also we see from the values of the counter, it may have been incremented by other thread while trying to print the value.

## Mutex Attributes
Though mutexes, by default, are private to a process, they can be shared between multiple processes. To create a mutex that can be shared between processes, we need to set up the attributes for pthread_mutex_init():

```c++
#include <pthread.h>

int main()
{
        pthread_mutex_t myMutex;
        pthread_mutexattr_t myMutexAttr;
        pthread_mutexattr_init(&myMutexAttr;);
        pthread_mutexattr_setpshared(&myMutexAttr;, PTHREAD_PROCESS_SHARED);

        pthread_mutex_init(&myMutex;, &myMutexAttr;);
        //...

        pthread_mutexattr_destroy(&myMutexAttr;);
        pthread_mutex_destroy(&myMutex;);
        return 0;
}
```

pthread_mutexattr_setpshared() with a pointer to the attribute structure and the value PTHREAD_PROCESS_SHARED sets the attributes to cause a shared mutex to be created.

Mutexes are not shared between processes by default. Calling pthread_mutexattr_setpshared() with the value PTHREAD_PROCESS_PRIVATE restores the attribute to the default.

These attributes are passed into the call to pthread_mutexattr_init() to set the attributes of the initialized mutex. Once the attributes have been used, they can be disposed of by a call to pthread_mutexattr_destroy().

## Spin Locks
Spin locks are essentially mutex locks.

A spin lock polls its lock condition repeatedly until that condition becomes true. Spin locks are most often used on multiprocessor systems where the expected wait time for a lock is small. In these situations, it is often more efficient to poll than to block the thread, which involves a Context switch and the updating of thread data structures.

The difference between a mutex lock and a spin lock is that a thread waiting to acquire a spin lock will keep trying to acquire the lock without sleeping and consuming processor resources until it finally acquires the lock. On the contrary, a mutex lock may sleep if it is unable to acquire the lock. But under normal mutex implementation, multex locks will immediately put a thread to sleep when it cannot get the mutex.

The advantage of using spin locks is that they will acquire the lock as soon as it is released, while a mutex lock will need to be woken by the OS before it can get the lock. The disadvantage is that a spin lock will spin on a virtual CPU monopolizing that resource, but a mutex lock will sleep and free the CPU for another thread to use. So, in practice, mutex locks are often implemented to be a hybrid of a spin locks and more traditional mutex locks. This kind of mutx is called adaptive mutex lock.

The call pthread_spin_init(). initializes a spin lock. A spin lock can be shared between processes or private to the process that created it. 
By passing the value PTHREAD_PROCESS_PRIVATE to the pthread_spin_init(), a spin lock is not shareable. 
To make it sharable, we need to pass the value PTHREAD_PROCESS_SHARED. The default is set to be private.

The call to the pthread_spin_lock() will spin until the lock is acquired, and the call to the pthread_spin_unlock() will release the lock. The call pthread_spin_destroy() releases any resources used by the lock.

```c++
// file: thread6.cpp
// gcc -o thread6 thread6 .cpp -lpthread
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
```

To use process resources more efficiently, we can use pthread_spin_trylock(). This call will attempt to acquire the lock, however, it will immediately return whether or not the lock is acquired rather than keep spinning:

```c++
void splock()
{
    int i = 0;
    while(i == 0) {
        pthread_spin_trylock(&slock;);
        i++;
        pthread_spin_unlock(&slock;);
    }
}
```

## Barriers
As one of the synchronization methods, a barrier tells a group of threads or processes must stop at the barrier and cannot proceed until all other threads/processes reach this barrier.

Here we'll address the classic barrier that its construct define the set of participating processes/threads statically. This is usually done either at program startup or when a barrier like the Pthreads barrier is instantiated.

Unlike the static barriers, to support more dynamic programming paradigms like fork/join parallelism, the sets of participants have to be dynamic. Thus, the set of processes/threads participating in a barrier operation needs to be able to change over time. But in this section, we will discuss only the static barriers.

We can create a barrier by calling pthread_barrier_init(). It initializes the barrier and it takes three parameters:

A pointer to the barrier to be initialized.
To determine whether the barrier is private or can be shared, it takes an optional attributes structure.
We need feed the number of threads that need to reach the barrier before any threads are released.
Each thread calls pthread_barrier_wait() when it reaches the barrier, and the call will return when the number of threads has reached the barrier. The code below shows how the bbefore aquiring the lock just check for queue count :)arrier force the threads to wait until all the threads have been created:

```c++
#include <pthread.h>
#include <stdio.h>

pthread_barrier_t b;

void task(int param)
{
    int id = (int)param;
    printf("before the barrier %d\n", id);
    pthread_barrier_wait(&b);
    printf("after the barrier %d\n", id);
}

int main()
{
    int nThread = 5;
    int i;

    pthread_t thread[nThread];
    pthread_barrier_init(&b, 0, nThread);
    for(i = 0; i < nThread; i++)
        pthread_create(&thread[i], 0, reinterpret_cast<void *(*)(void *)>(task), (void*)i);
    for(i = 0; i < nThread; i++)
        pthread_join(thread[i], 0);
    pthread_barrier_destroy(&b);
    return 0;
}
```
Output below shows all the threads arrive and leave.

Output is:
```
before the barrier 0
before the barrier 1
before the barrier 2
before the barrier 4
before the barrier 3
after the barrier 3
after the barrier 0
after the barrier 4
after the barrier 2
after the barrier 1
```

If we did not set the barrier, the arrival and leave would have been mixed:

```
//comment line 10
    // pthread_barrier_wait(&b);
```

```
before the barrier 0
after the barrier 0
before the barrier 4
before the barrier 2
after the barrier 4
before the barrier 3
after the barrier 3
after the barrier 2
before the barrier 1
after the barrier 1
```

## Synchronization Pthread Example - Mutexes 2
The code below shows another example of synchronizing access with mutexes. To control access, we lock a mutex before entering the section of the code, and then unlock it when we have finished.

```c++
int pthread_mutex_init(pthread_mutex_t *m_mutex, const pthread_mutexattr_t *mutexattr);
int pthread_mutex_lock(pthread_mutex_t *m_mutex);
int pthread_mutex_unlock(pthread_mutex_t *m_mutex);
int pthread_mutex_destroy(pthread_mutex_t *m_mutex);
```

All of the functions take a pointer to a previously declared object, in this case, pthread_mutex_t. The extra attribute parameter pthread_mutex_init allows us to provide attributes for the mutex, which controls its behavior.

```c++
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
```

Output is:
```
Type in some characters. Enter 'quit' to finish
a
You typed in 1 characters
aa
aaa
You typed in 2 characters
aaa
aaaaYou typed in 3 characters
aa
aa
aa
You typed in 3 characters
aaa
You typed in 4 characters
You typed in 2 characters
You typed in 2 characters
You typed in 2 characters
You typed in 3 characters
quit
Waiting for thread to finish...
Thread joined
```