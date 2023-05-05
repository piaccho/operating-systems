#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

#include <mqueue.h>
#include "common.h"

mqd_t server_queue;
mqd_t client_queues[MAX_CLIENTS];
key_t queue_key;
int queue_id;

void close_queues()
{
    printf("-- CLOSING SERVER QUEUE\n");
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_queues[i] != -1)
        {
            send_message(MTYPE_STOP, "STOP", -1, SERVER_QUEUE_NAME, i, client_queues[i]);
            struct message_t msg;
            unsigned int msg_prio;
            while(msg.mtype != MTYPE_STOP || msg.client_id != i) {
                mq_receive(server_queue, (char *)&msg, sizeof(struct message_t), &msg_prio);
            }
            printf("SERVER RECIVED STOP FROM ID: %d\n", i);
        }
    }
    mq_close(server_queue);
    mq_unlink(SERVER_QUEUE_NAME);
    printf("-- SERVER QUEUE CLOSED\n");
    exit(0);
}

void mtype_init_handler(message_t *received_msg)
{
    printf("-- SERVER RECEIVED INIT MESSAGE");
    char *client_queue_name = received_msg->queue_name;

    int free_index = 0;
    int client_id_to_assign;
    for (; free_index < MAX_CLIENTS; free_index++)
    {
        if (client_queues[free_index] == -1)
            break;
    }
    if (free_index == MAX_CLIENTS)
    {
        client_id_to_assign = -1;
    }
    else
    {
        client_id_to_assign = free_index;
        client_queues[client_id_to_assign] = mq_open(client_queue_name, O_WRONLY);
        if (client_queues[client_id_to_assign] == -1) // error handler
        {
            perror("mq_open");
            return;
        }
    }

    printf("\n-- SERVER SENDING INIT THROUGH %s\n\n", client_queue_name);
    send_message(MTYPE_INIT, "Hello client!", -1, SERVER_QUEUE_NAME, client_id_to_assign, client_queues[client_id_to_assign]);
}

void mtype_list_handler(message_t *received_msg) {
    printf("-- SERVER RECEIVED LIST MESSAGE\n");
    int client_id = received_msg->client_id;

    char msg_body[MAX_MESSAGE_LENGTH];
    char *out = msg_body;
    // printf("Current active clients:\n");
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_queues[i] != -1)
        {
            // printf("\tCLIENT ID: %d\n", i);
            out += sprintf(out, "\tCLIENT ID: %d\n", i);
        }
    }

    printf("-- SERVER SENDING LIST TO ID: %d\n\n", client_id);
    send_message(MTYPE_LIST, msg_body, -1, SERVER_QUEUE_NAME, received_msg->client_id, client_queues[client_id]);
}

void mtype_toall_handler(message_t *received_msg)
{
    printf("-- SERVER RECEIVED 2ALL MESSAGE\n");
    printf("-- SERVER SENDING 2ALL\n");
    int sender_client_id = received_msg->client_id;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_queues[i] != -1 && i != sender_client_id)
        {
            printf("-- SERVER SENDING 2ALL TO ID: %d\n", i);
            send_message(MTYPE_TO_ALL, received_msg->text, i, SERVER_QUEUE_NAME, sender_client_id, client_queues[i]);
        }
    }
    printf("\n");
}

void mtype_toone_handler(message_t *received_msg)
{
    printf("-- SERVER RECEIVED 2ONE MESSAGE \n");
    int sender_client_id = received_msg->client_id;
    int target_id = received_msg->target_id;

    if (client_queues[target_id] != -1)
    {
        printf("-- SERVER SENDING 2ONE TO ID: %d\n\n", target_id);
        send_message(MTYPE_TO_ONE, received_msg->text, target_id, SERVER_QUEUE_NAME, sender_client_id, client_queues[target_id]);
    }
}

void mtype_stop_handler(message_t *received_msg)
{
    printf("-- SERVER RECEIVED STOP MESSAGE\n");
    int client_id = received_msg->client_id;

    send_message(MTYPE_STOP, "STOP", -1, SERVER_QUEUE_NAME, client_id, client_queues[client_id]);
    client_queues[client_id] = -1;
}

void save_to_log(message_t *received_msg) {
    char *filename = "logs.txt";
    FILE *logs = fopen(filename, "a");
    if (logs == NULL)
    {
        printf("Nie udało się otworzyć pliku %s\n", filename);
    } else {
        fprintf(logs, "TIME: %s\nCLIENT ID: %d\nCONTENT: %s\n\n", ctime(&received_msg->timestamp), received_msg->client_id, received_msg->text);
    }
    fclose(logs);
}

int main()
{
    printf("####################################\n");
    printf("#          SERVER STARTED          #\n");
    printf("####################################\n\n");

    signal(SIGINT, &close_queues);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_queues[i] = -1;
    }

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = sizeof(struct message_t);

    mq_unlink(SERVER_QUEUE_NAME);
    server_queue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDWR, QUEUE_PERMISSIONS, &attr);
    if (server_queue == -1) // error handler
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    printf("SERVER QUEUE OPENED - %s\n\n", SERVER_QUEUE_NAME);

    while (1)
    {    
        struct message_t msg;
        unsigned int msg_prio;
        mq_receive(server_queue, (char *)&msg, sizeof(struct message_t), &msg_prio);

        // printf("\nReceiving message:\n\tpriority: %d\n\tmtype: %d\n\ttarget id: %d\n\ttext: '%s'\n\tat: %s\tclient id: %d\n", msg_prio, msg.mtype, msg.target_id, msg.text, ctime(&msg.timestamp), msg.client_id);

        switch (msg.mtype)
        {
        case MTYPE_INIT:
            save_to_log(&msg);
            mtype_init_handler(&msg);
            break;
        case MTYPE_LIST:
            save_to_log(&msg);
            mtype_list_handler(&msg);
            break;
        case MTYPE_TO_ALL:
            save_to_log(&msg);
            mtype_toall_handler(&msg);
            break;
        case MTYPE_TO_ONE:
            save_to_log(&msg);
            mtype_toone_handler(&msg);
            break;
        case MTYPE_STOP:
            save_to_log(&msg);
            mtype_stop_handler(&msg);
            break;
        default:
            break;
        }
    }

    mq_close(server_queue);
    mq_unlink(SERVER_QUEUE_NAME);
    return 0;
}