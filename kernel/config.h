#ifndef CONFIG_H
#define CONFIG_H

/// The number of tasks supported by the kernel
#define TASK_COUNT          64

/// Number of priorities in the kernel
#define PRIORITY_COUNT      32

/// Length of each priority queue
#define PRIORITY_LENGTH     8

/// Length of task send queues
#define SEND_QUEUE_LENGTH   8


/// The stack block size in words
#define STACK_BLOCK_SIZE    1024

/// Small stack, equal to one stack block (4096 bytes = 4kb)
#define STACK_SIZE_SMALL    1

/// Medium stack, equal to 4 stack blocks (16,384 bytes = 16kb)
#define STACK_SIZE_MEDIUM   4

/// Large stack, equal to 64 stack blocks (262,144 bytes = 256 kb)
#define STACK_SIZE_LARGE    64

#endif
