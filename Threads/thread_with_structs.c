#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char* text;
} Arguments;

void* run(void* args) {
    Arguments* arg = (Arguments*)args;
    printf("ID: %d\n", arg->id);
    printf("Text: %s\n", arg->text);

    Arguments* return_val = malloc(sizeof(Arguments));
    return_val->id = 2;
    return_val->text = "Returning from thread";

    pthread_exit(return_val);
}

int main() {
    pthread_t thread;
    void* return_ptr;

    Arguments* args = malloc(sizeof(Arguments));
    args->id = 1;
    args->text = "Calling thread";

    pthread_create(&thread, NULL, run, args);

    pthread_join(thread, &return_ptr);

    Arguments* return_val = (Arguments*)return_ptr;

    printf("ID: %d\n", return_val->id);
    printf("Text: %s\n", return_val->text);

    free(return_ptr);
    free(args);

    return 0;
}
