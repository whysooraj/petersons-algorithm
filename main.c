#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#include <assert.h>

#define NUM_ITERATIONS 1000000

/*
 * Peterson's Algorithm Shared State:
 * flag[i] = true indicates thread i wants to enter critical section.
 * turn = j indicates thread j has priority if both contend.
 *
 * Modern multi-core CPUs reorder memory operations.
 * Sequential consistency (memory_order_seq_cst) ensures
 * Peterson's correctness on x86/ARM/modern architectures.
 */
static atomic_bool flag[2] = {false, false};
static atomic_int turn = 0;

static long shared_counter = 0;
static atomic_int in_cs = 0; /* Tracks active threads in critical section */

static void lock_peterson(int self) {
    int other = 1 - self;
    atomic_store_explicit(&flag[self], true, memory_order_seq_cst);
    atomic_store_explicit(&turn, other, memory_order_seq_cst);

    while (atomic_load_explicit(&flag[other], memory_order_seq_cst) &&
           atomic_load_explicit(&turn, memory_order_seq_cst) == other) {
        /* busy wait */
    }
}

static void unlock_peterson(int self) {
    atomic_store_explicit(&flag[self], false, memory_order_seq_cst);
}

static void *worker_thread(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        lock_peterson(id);

        /* --- Critical Section Start --- */
        int active = atomic_fetch_add_explicit(&in_cs, 1, memory_order_relaxed) + 1;
        if (active != 1) {
            fprintf(stderr, "Mutual exclusion violation! active=%d\n", active);
            exit(EXIT_FAILURE);
        }

        shared_counter++;

        atomic_fetch_sub_explicit(&in_cs, 1, memory_order_relaxed);
        /* --- Critical Section End --- */

        unlock_peterson(id);
    }

    return NULL;
}

int main(void) {
    pthread_t t0, t1;
    int id0 = 0, id1 = 1;

    printf("Starting Peterson's Algorithm demonstration...\n");
    printf("Iterations per thread: %d\n", NUM_ITERATIONS);
    printf("Expected final counter: %ld\n", (long)NUM_ITERATIONS * 2);

    if (pthread_create(&t0, NULL, worker_thread, &id0) != 0 ||
        pthread_create(&t1, NULL, worker_thread, &id1) != 0) {
        perror("pthread_create failed");
        return EXIT_FAILURE;
    }

    pthread_join(t0, NULL);
    pthread_join(t1, NULL);

    printf("Final counter value: %ld\n", shared_counter);

    /* Verification check */
    assert(shared_counter == (long)NUM_ITERATIONS * 2);
    printf("Verification passed: mutual exclusion maintained.\n");

    return EXIT_SUCCESS;
}
