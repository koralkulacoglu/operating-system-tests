#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void cleanup(void* mem) {
    printf("Memory with value %d cleaned!\n", *(int*)mem);
    free(mem);
}

void* run1(void* arg) {
    int* val = arg;
    pthread_cleanup_push(cleanup, val);

    printf("Thread 1 ran!\n");

    pthread_cleanup_pop(1);  // 1 = run cleanup, 0 = don't run cleanup (in case
                             // we already freed memory at this point)

    pthread_exit(NULL);
}

void* run2(void* arg) {
    int* val = arg;
    pthread_cleanup_push(cleanup, val);

    printf("Thread 2 ran!\n");

    // we can free it in the function and remove the cleanup call since we don't
    // need it anymore
    printf("Memory with value %d cleaned!\n", *(int*)arg);
    free(arg);
    pthread_cleanup_pop(0);

    pthread_exit(NULL);
}

int main() {
    pthread_t thread;

    int* arg1 = malloc(sizeof(int));
    *arg1 = 1;

    pthread_create(&thread, NULL, run1, arg1);

    pthread_join(thread, NULL);  // null since we have no return values

    int* arg2 = malloc(sizeof(int));
    *arg2 = 2;

    pthread_create(&thread, NULL, run2, arg2);

    pthread_join(thread, NULL);  // null since we have no return values

    // we don't need to do this anymore since it's handled in the cleanup func
    // free(arg1);
    // free(arg2);

    return 0;
}
