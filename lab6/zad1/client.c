#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#include "common.h"

key_t client_queue_key;
int client_queue_id;

key_t server_queue_key;
int server_queue_id;

int client_id = -1;

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

void send_init_message()
{
    message_t msg;
    msg.mtype = INIT_MTYPE;
    strcpy(msg.text, "INIT");
    msg.queue_key = client_queue_key;
    msg.client_id = client_id;
    char *timestamp = get_current_time();
    strcpy(msg.timestamp, timestamp);

    printf("CLIENT SENDING INIT\n");

    if (msgsnd(server_queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }

    message_t received_msg;
    if (msgrcv(client_queue_id, &received_msg, sizeof(received_msg) - sizeof(long), INIT_MTYPE, 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }

    printf("CLIENT RECEIVING INIT\n");

    if ((client_id = received_msg.client_id) == -1)
    {
        printf("NO IDs AVAILABLE\n");
        exit(0);
    }
    else
    {
        printf("SUCCESSFULY ASSIGNED CLIENT ID: %d\n\n", client_id);
    }
}

void send_list_message()
{
    message_t msg;
    msg.mtype = LIST_MTYPE;
    strcpy(msg.text, "LIST");
    msg.queue_key = client_queue_key;
    msg.client_id = client_id;
    char *timestamp = get_current_time();
    strcpy(msg.timestamp, timestamp);

    printf("CLIENT SENDING LIST\n");

    if (msgsnd(server_queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }
}

void send_toall_message(char *msg_body)
{
    message_t msg;
    msg.mtype = TO_ALL_MTYPE;
    strcpy(msg.text, msg_body);
    msg.queue_key = client_queue_key;
    msg.client_id = client_id;
    char *timestamp = get_current_time();
    strcpy(msg.timestamp, timestamp);

    printf("CLIENT SENDING 2ALL: %s\n", msg.text);

    if (msgsnd(server_queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }
}

void send_toone_message(char *msg_body, int target_id)
{
    message_t msg;
    msg.mtype = TO_ONE_MTYPE;
    strcpy(msg.text, msg_body);
    msg.queue_key = client_queue_key;
    msg.client_id = client_id;
    char *timestamp = get_current_time();
    strcpy(msg.timestamp, timestamp);
    msg.target_id = target_id;

    printf("\nCLIENT SENDING 2ONE TO ID: %d - %s\n", msg.target_id, msg.text);

    if (msgsnd(server_queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }
}

void send_stop_message()
{
    message_t msg;
    msg.mtype = STOP_MTYPE;
    strcpy(msg.text, "STOP");
    msg.queue_key = client_queue_key;
    msg.client_id = client_id;
    char *timestamp = get_current_time();
    strcpy(msg.timestamp, timestamp);

    printf("CLIENT SENDING STOP\n");

    if (msgsnd(server_queue_id, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }

    msgctl(client_queue_id, IPC_RMID, NULL);
    printf("CLIENT QUEUE CLOSED\n");
    exit(0);
}

void handle_server_message() {
    message_t received_msg;
    if (msgrcv(client_queue_id, &received_msg, sizeof(received_msg) - sizeof(long), STOP_MTYPE, IPC_NOWAIT) != -1)
    {
        send_stop_message();
    }
    if (msgrcv(client_queue_id, &received_msg, sizeof(received_msg) - sizeof(long), LIST_MTYPE, IPC_NOWAIT) != -1)
    {
        printf("\nCLIENT RECEIVING LIST\n");
        printf("\nCurrent active clients:\n%s", received_msg.text);
    };
    if (msgrcv(client_queue_id, &received_msg, sizeof(received_msg) - sizeof(long), TO_ALL_MTYPE, IPC_NOWAIT) != -1)
    {
        printf("\nCLIENT RECEIVING 2ALL\n");
        printf("\tMESSAGE: %s\n\tFROM ID: %d\n\tTIME: %s\n", received_msg.text, received_msg.client_id, received_msg.timestamp);
    };
    if (msgrcv(client_queue_id, &received_msg, sizeof(received_msg) - sizeof(long), TO_ONE_MTYPE, IPC_NOWAIT) != -1)
    {
        printf("\nCLIENT RECEIVING 2ONE:\n");
        printf("\tMESSAGE: %s\n\tFROM ID: %d\n\tTIME: %s\n", received_msg.text, received_msg.client_id, received_msg.timestamp);
    };
}

void handle_input_command(char *input) {
    if (strcmp(input, "") == 0)
    {
        return;
    }

    char *token = strtok(input, " ");

    if (strcmp(token, "LIST") == 0)
    {
        send_list_message();
    }
    else if (strcmp(token, "2ALL") == 0)
    {
        token = strtok(NULL, "\"");
        if (token == NULL)
        {
            printf("INVALID ARGUMENTS - USAGE:\n\t2ALL \"TEXT\"\n");
            return;
        }
        if (strlen(token) > MAX_MESSAGE_LENGTH)
        {
            printf("TEXT TOO LONG - MAX LENGTH OF MSG: %d\n", MAX_MESSAGE_LENGTH);
            return;
        }
        // printf("2ALL:\n\tstring: %s\n", token);
        send_toall_message(token);
    }
    else if (strcmp(token, "2ONE") == 0)
    {
        long target_id;
        token = strtok(NULL, " ");
        char *end;
        target_id = strtol(token, &end, 10);
        // printf("END: %s\nSTRTOL: %ld", end, target_id);
        if (token == NULL)
        {
            printf("INVALID ARGUMENTS - USAGE:\n\t2ONE <CLIENT_ID> \"TEXT\"\n");
            return;
        }
        token = strtok(NULL, "\"");
        if (token == NULL)
        {
            printf("INVALID ARGUMENTS - USAGE:\n\t2ONE <CLIENT_ID> \"TEXT\"\n");
            return;
        }
        if (strlen(token) > MAX_MESSAGE_LENGTH)
        {
            printf("TEXT TOO LONG - MAX LENGTH OF MSG: %d\n", MAX_MESSAGE_LENGTH);
            return;
        }
        // printf("2ONE:\n\tclient_id: %d\n\tstring: %s\n", target_id, token);
        send_toone_message(token, (int)target_id);
    }
    else if (strcmp(token, "STOP") == 0)
    {
        // printf("STOP ACTIVATED\n");
        send_stop_message();
    }
    else
    {
        printf("UNKNOWN COMMAND - USAGE:\n\tLIST\n\t2ALL \"TEXT\"\n\t2ONE <CLIENT_ID> \"TEXT\"\n\tSTOP\n");
    }
}

int main()
{
    signal(SIGINT, &send_stop_message);

    srand(time(NULL));
    client_queue_key = ftok(HOME_PATH, rand() % 255 + 1);
    client_queue_id = msgget(client_queue_key, IPC_CREAT|0666); 
    if (client_queue_id == -1)                                  // error handler
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    
    printf("####################################\n");
    printf("#          CLIENT STARTED          #\n");
    printf("####################################\n");
    printf("# QUEUE KEY: %d\n# QUEUE ID: %d\n", client_queue_key, client_queue_id);

    server_queue_key = ftok(HOME_PATH, SERVER_Q_KEY_GEN_NUM);
    server_queue_id = msgget(server_queue_key, 0);
    if (server_queue_id == -1) // error handler
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    printf("# SERVER QUEUE KEY: %d\n# SERVER QUEUE ID: %d\n", client_queue_key, client_queue_id);
    printf("####################################\n\n");

    send_init_message();
    printf("ENTER COMMANDS:\n");

    while (1)
    {
        printf("> ");

        char *input = NULL;
        size_t len = 0;
        ssize_t inputSize = 0;
        inputSize = getline(&input, &len, stdin);
        input[inputSize - 1] = '\0';

        handle_input_command(input);
        handle_server_message();
    }

    return 0;
}