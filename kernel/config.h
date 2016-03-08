#ifndef CONFIG_H
#define CONFIG_H

/// The number of tasks supported by the kernel
#define TASK_COUNT          64

/// Number of priorities in the kernel
#define PRIORITY_COUNT      32

/// Length of each priority queue
#define PRIORITY_LENGTH     16

/// Length of task send queues
#define SEND_QUEUE_LENGTH   8


/// The memory block size in words (16,384 bytes = 16kb)
#define MEMORY_BLOCK_SIZE    16384

/// The memoryblock count
#define MEMORY_BLOCK_COUNT  TASK_COUNT


/// The terminal i/o buffer size
#define TERM_BUFFER         8

/// The train i/o buffer size
#define TRAIN_BUFFER        8

#endif
