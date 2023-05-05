#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include "common.h"

int client_queues_keys[MAX_CLIENTS];
int free_index = 0;
key_t queue_key;
int queue_id;

char *get_current_time()
{
    time_t current_time;
    struct tm *time_info;
    char *timestamp = (char *)malloc(30 * sizeof(char));
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(timestamp, 30, "%d %b %Y - %T", time_info);
    return timestamp;
}

void close_queues()
{
    printf("\n------- CLOSING SERVER QUEUE -------\n");
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_queues_keys[i] != -1)
        {
            int client_queue_id = msgget(client_queues_keys[i], 0);
            message_t msg;
            msg.mtype = STOP_MTYPE;

            printf("\nSERVER SENDING STOP TO ID: %d\n", i);
            if (msgsnd(client_queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
            {
                perror("msgsnd");
                continue;
            }
            message_t received_msg;
            msgrcv(queue_id, &received_msg, sizeof(received_msg) - sizeof(long), STOP_MTYPE, 0);
            printf("\nSERVER RECIVED STOP FROM ID: %d\n", i);
        }
    }
    msgctl(queue_id, IPC_RMID, NULL);
    printf("\n------- SERVER QUEUE CLOSED -------\n");
    exit(0);
}

void init_mtype_handler(message_t *received_msg)
{
    printf("\n------- SERVER RECEIVED INIT MESSAGE -------\n");
    int client_queue_key = received_msg->queue_key;

    int client_id_to_assign;
    if (client_queues_keys[free_index] != -1 && free_index == MAX_CLIENTS - 1)
    {
        client_id_to_assign = -1;
    }
    else
    {
        client_id_to_assign = free_index;
        client_queues_keys[free_index] = client_queue_key;
        if (free_index < MAX_CLIENTS - 1)
        {
            free_index++;
        }
    }

    received_msg->client_id = client_id_to_assign;
    int client_queue_id = msgget(client_queue_key, 0);
    if (client_queue_id == -1) // error handler
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    message_t msg;
    msg.mtype = INIT_MTYPE;
    strcpy(msg.text, "INIT");
    msg.queue_key = client_queue_key;
    msg.client_id = client_id_to_assign;
    char *timestamp = get_current_time();
    strcpy(msg.timestamp, timestamp);

    printf("\nSERVER SENDING INIT THROUGH %d\n", client_queue_id);

    if (msgsnd(client_queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }
}

void list_mtype_handler(message_t *received_msg) {
    printf("\n------- SERVER RECEIVED LIST MESSAGE -------\n");
    int client_queue_key = received_msg->queue_key;

    char msg_body[MAX_MESSAGE_LENGTH];
    char *out = msg_body;
    printf("Current active clients:\n");
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_queues_keys[i] != -1)
        {
            printf("\tCLIENT ID: %d, CLIENT QUEUE KEY: %d\n", i, client_queues_keys[i]);
            out += sprintf(out, "\tCLIENT ID: %d\n", i);
        }
    }

    int client_queue_id = msgget(client_queue_key, 0);
    if (client_queue_id == -1) // error handler
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    message_t msg;
    msg.mtype = LIST_MTYPE;
    strcpy(msg.text, msg_body);
    msg.queue_key = client_queue_key;
    char *timestamp = get_current_time();
    strcpy(msg.timestamp, timestamp);

    printf("\nSERVER SENDING LIST THROUGH %d\n", client_queue_id);

    if (msgsnd(client_queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }
}

void toall_mtype_handler(message_t *received_msg)
{
    printf("\n------- SERVER RECEIVED 2ALL MESSAGE -------\n");
    int sender_client_id = received_msg->client_id;

    message_t msg;
    msg.mtype = TO_ALL_MTYPE;
    strcpy(msg.text, received_msg->text);
    strcpy(msg.timestamp, received_msg->timestamp);
    msg.client_id = sender_client_id;
    

    printf("\nSERVER SENDING 2ALL\n");

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (client_queues_keys[i] != -1 && i != sender_client_id)
        {   
            int client_queue_key = client_queues_keys[i];
            int client_queue_id = msgget(client_queue_key, 0);
            if (client_queue_id == -1) // error handler
            {
                perror("msgget");
                exit(EXIT_FAILURE);
            }

            msg.queue_key = client_queue_key;

            if (msgsnd(client_queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
            {
                perror("msgsnd");
                exit(1);
            }
        }
    }

}

void toone_mtype_handler(message_t *received_msg)
{
    printf("\n------- SERVER RECEIVED 2ONE MESSAGE -------\n");
    int sender_client_id = received_msg->client_id;
    int target_id = received_msg->target_id;

    // char msg_body[MAX_MESSAGE_LENGTH];
    // char *out = msg_body;
    // out += sprintf(out, "%s ", received_msg->text);
    // out += sprintf(out, "FROM ID: %d ", sender_client_id);
    // out += sprintf(out, "TIME: %s ", get_current_time());

    if (client_queues_keys[target_id] != -1)
    {
        int client_queue_key = client_queues_keys[target_id];
        int client_queue_id = msgget(client_queue_key, 0);
        if (client_queue_id == -1) // error handler
        {
            perror("msgget");
            exit(EXIT_FAILURE);
        }

        message_t msg;
        msg.mtype = TO_ONE_MTYPE;
        strcpy(msg.text, received_msg->text);
        strcpy(msg.timestamp, received_msg->timestamp);
        msg.client_id = sender_client_id;
        msg.queue_key = client_queue_key;

        printf("\nSERVER SENDING 2ONE TO ID: %d - %s\n", target_id, msg.text);
        if (msgsnd(client_queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
        {
            perror("msgsnd");
            exit(1);
        }
    }

}

void stop_mtype_handler(message_t *received_msg)
{
    printf("\n------- SERVER RECEIVED STOP MESSAGE -------\n");
    int client_id = received_msg->client_id;

    client_queues_keys[client_id] = -1;

}

void save_to_log(message_t *received_msg) {
    char *filename = "logs.txt";
    FILE *logs = fopen(filename, "a");
    if (logs == NULL)
    {
        printf("Nie udało się otworzyć pliku %s\n", filename);
    } else {
        fprintf(logs, "TIME: %s\nCLIENT ID: %d\nCONTENT: %s\n\n", received_msg->timestamp, received_msg->client_id, received_msg->text);
    }
    fclose(logs);
}

int main()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_queues_keys[i] = -1;
    }

    key_t queue_key = ftok(HOME_PATH, SERVER_Q_KEY_GEN_NUM);
    int queue_id = msgget(queue_key, IPC_CREAT | 0666);
    if (queue_id == -1) // error handler
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }


    printf("QUEUE KEY: %d\n", queue_key);
    printf("QUEUE ID: %d\n", queue_id);

    signal(SIGINT, &close_queues);

    while (1)
    {
        // msgrcv(queue_id, &received_msg, sizeof(received_msg) - sizeof(long), 0, 0);
        message_t received_msg;
        if (msgrcv(queue_id, &received_msg, sizeof(received_msg) - sizeof(long), INIT_MTYPE, IPC_NOWAIT) != -1)
        {
            save_to_log(&received_msg);
            init_mtype_handler(&received_msg);
        }
        if (msgrcv(queue_id, &received_msg, sizeof(received_msg) - sizeof(long), STOP_MTYPE, IPC_NOWAIT) != -1) {
            save_to_log(&received_msg);
            stop_mtype_handler(&received_msg);
        }
        if (msgrcv(queue_id, &received_msg, sizeof(received_msg) - sizeof(long), LIST_MTYPE, IPC_NOWAIT) != -1)
        {
            save_to_log(&received_msg);
            list_mtype_handler(&received_msg);
        };
        if (msgrcv(queue_id, &received_msg, sizeof(received_msg) - sizeof(long), TO_ALL_MTYPE, IPC_NOWAIT) != -1)
        {
            save_to_log(&received_msg);
            toall_mtype_handler(&received_msg);
        };
        if (msgrcv(queue_id, &received_msg, sizeof(received_msg) - sizeof(long), TO_ONE_MTYPE, IPC_NOWAIT) != -1)
        {
            save_to_log(&received_msg);
            toone_mtype_handler(&received_msg);
        };
    }
    return 0;
}