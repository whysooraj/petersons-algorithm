# Peterson's Algorithm Implementation in C

Implementation of Peterson's algorithm for two-process mutual exclusion in C using POSIX threads (`pthreads`) and C11 atomics (`stdatomic.h`).

## Overview

Peterson's algorithm is a concurrent programming algorithm that allows two processes (or threads) to share a single-use resource without conflict, using only shared memory for communication.

### Critical Section Requirements Satisfied

1. Mutual Exclusion: If thread 0 is executing in its critical section, thread 1 cannot enter, and vice versa.
2. Progress: If no thread is executing in its critical section and some threads want to enter, only those threads not executing in their remainder sections participate in deciding which will enter next.
3. Bounded Waiting: There exists a bound on the number of times other threads are allowed to enter their critical sections after a thread has requested entry.

## Algorithm Details

### Shared Variables

- `flag[2]`: Boolean array where `flag[i] = true` indicates thread `i` wants to enter the critical section.
- `turn`: Integer variable indicating whose turn it is if both threads attempt entry simultaneously.

### Structure

```c
// Entry section
flag[self] = true;
turn = other;
while (flag[other] && turn == other) {
    // Busy wait
}

// Critical section
// ... shared resource access ...

// Exit section
flag[self] = false;
```

### Modern Multi-Core & Hardware Reordering

In standard theoretical formulations, variables are accessed sequentially. However, modern out-of-order CPUs (x86, ARM) and optimizing compilers can reorder stores and loads.

Without explicit sequential consistency:
- The store to `turn` can be reordered before `flag[self] = true`.
- Both threads could observe `turn` as set to the other and enter simultaneously.

This implementation uses C11 atomics (`atomic_store_explicit`, `atomic_load_explicit`) with `memory_order_seq_cst` to enforce sequential consistency across CPU cores.

## Build and Run

### Prerequisites

- GCC or Clang
- POSIX Threads library (`-pthread`)
- GNU Make

### Instructions

Compile:
```bash
make
```

Run demonstration:
```bash
make run
```

Clean build artifacts:
```bash
make clean
```

## Verification

The program creates two concurrent worker threads, each executing 1,000,000 increments on a shared counter. It uses an atomic tracker `in_cs` to assert that never more than 1 thread occupies the critical section, and verifies that the final count equals 2,000,000.
