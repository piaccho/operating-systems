#ifndef COMMON_H
#define COMMON_H

#include <time.h>

#define MAX_CLIENTS 10
#define MAX_MESSAGES 10;
#define QUEUE_PERMISSIONS 0666
#define MAX_MESSAGE_LENGTH 200
#define QUEUE_NAME_LENGTH 20
#define SERVER_QUEUE_NAME "/serverqueue"

#define MTYPE_INIT 1
#define MTYPE_LIST 2
#define MTYPE_TO_ALL 3
#define MTYPE_TO_ONE 4
#define MTYPE_STOP 5

#define SKIP 1
#define NOSKIP 0

typedef struct message_t
{
    int mtype;
    char text[MAX_MESSAGE_LENGTH];
    char queue_name[QUEUE_NAME_LENGTH]; 
    int client_id;
    int target_id;
    time_t timestamp;
} message_t;

void send_message(int mtype, char *text, int target_id, char *sender_queue_name, int client_id, mqd_t target_queue) {
    struct message_t msg;
    unsigned int msg_prio;
    
    switch(mtype) {
        case MTYPE_LIST: {
            msg_prio = 4;
            break;
        }
        case MTYPE_STOP: {
            msg_prio = 5;
            break;
        }
        default:
        {
            msg_prio = 1;
            break;
        }
    }

    msg.mtype = mtype;
    strcpy(msg.text, text);
    strcpy(msg.queue_name, sender_queue_name);
    msg.client_id = client_id;
    msg.timestamp = time(NULL);
    msg.target_id = target_id;

    // printf("Sending message from %s:\n\tpriority: %d\n\tmtype: %d\n\ttarget id: %d\n\ttext: '%s'\n\tat: %s\tclient id: %d\n", msg.queue_name, msg_prio, msg.mtype, msg.target_id, msg.text, ctime(&msg.timestamp), msg.client_id);

    if((mq_send(target_queue, (char *)&msg, sizeof(struct message_t), msg_prio)) == -1) {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }
}

#endif
