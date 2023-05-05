#ifndef COMMON_H
#define COMMON_H

#include <time.h>

#define MAX_CLIENTS 10
#define MAX_MESSAGE_LENGTH 256
#define MAX_CONTENT_LENGTH 512
#define HOME_PATH getenv("HOME")
#define SERVER_Q_KEY_GEN_NUM 1

#define INIT_MTYPE 1
#define LIST_MTYPE 2
#define TO_ALL_MTYPE 3
#define TO_ONE_MTYPE 4
#define STOP_MTYPE 5

typedef struct message_t
{
    long mtype;
    char text[MAX_MESSAGE_LENGTH];
    int queue_key;
    int client_id;
    char timestamp[30];
    int target_id;
} message_t;

#endif
