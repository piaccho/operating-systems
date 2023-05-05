#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <mqueue.h>
#include "common.h"

mqd_t client_queue;
mqd_t server_queue;

char client_queue_name[QUEUE_NAME_LENGTH];
int client_id = -1;


void send_stop_message()
{
    send_message(MTYPE_STOP, "STOP", -1, client_queue_name, client_id, server_queue);

    mq_close(client_queue);
    mq_unlink(client_queue_name);
    printf("\n------- CLIENT QUEUE CLOSED -------\n");
    exit(0);
}

void handle_server_message(int flag) {
    if(flag == SKIP) {
        struct mq_attr attr;
        mq_getattr(client_queue, &attr);
        if (attr.mq_curmsgs == 0 )
        {
            return;
        }
    }

    struct message_t msg;
    unsigned int msg_prio;
    mq_receive(client_queue, (char *)&msg, sizeof(struct message_t), &msg_prio);

    switch (msg.mtype)
    {
    case MTYPE_INIT:
        printf("-- CLIENT RECEIVING INIT\n");
        if ((client_id = msg.client_id) == -1)
        {
            printf("NO IDs AVAILABLE\n");
            mq_close(client_queue);
            mq_unlink(client_queue_name);
            printf("\n------- CLIENT QUEUE CLOSED -------\n");
            exit(0);
        }
        else
        {
            printf("SUCCESSFULY ASSIGNED CLIENT ID: %d\n\n", client_id);
        }
        break;
    case MTYPE_LIST:
        printf("-- CLIENT RECEIVING LIST\n");
        printf("Current active clients:\n%s", msg.text);
        break;
    case MTYPE_TO_ALL:
        printf("-- CLIENT RECEIVING 2ALL\n");
        printf("\tMESSAGE: %s\n\tFROM ID: %d\n\tTIME: %s\n", msg.text, msg.client_id, ctime(&msg.timestamp));
        break;
    case MTYPE_TO_ONE:
        printf("-- CLIENT RECEIVING 2ONE:\n");
        printf("\tMESSAGE: %s\n\tFROM ID: %d\n\tTIME: %s\n", msg.text, msg.client_id, ctime(&msg.timestamp));
        break;
    case MTYPE_STOP:
        printf("-- CLIENT RECEIVING STOP:\n");
        send_stop_message();
        handle_server_message(NOSKIP);
        break;
    default:
        break;
    }
}

void handle_input_command(char *input) {
    if (strcmp(input, "") == 0)
    {
        return;
    }

    char *token = strtok(input, " ");

    if (strcmp(token, "LIST") == 0)
    {
        printf("-- CLIENT SENDING LIST MSG\n");
        send_message(MTYPE_LIST, "LIST", -1, client_queue_name, client_id, server_queue);
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

        printf("-- CLIENT SENDING 2ALL MSG\n");
        send_message(MTYPE_TO_ALL, token, -1, client_queue_name, client_id, server_queue);
    }
    else if (strcmp(token, "2ONE") == 0)
    {
        long target_id;
        token = strtok(NULL, " ");
        char *end;
        target_id = strtol(token, &end, 10);
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
        printf("-- CLIENT SENDING 2ONE MSG\n");
        send_message(MTYPE_TO_ONE, token, (int)target_id, client_queue_name, client_id, server_queue);
    }
    else if (strcmp(token, "STOP") == 0)
    {
        printf("-- CLIENT SENDING STOP MSG\n");
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

    printf("####################################\n");
    printf("#          CLIENT STARTED          #\n");
    printf("####################################\n\n");

    srand(time(NULL));
    sprintf(client_queue_name, "/client_q_%d", rand() % 255 + 1);

    struct mq_attr attr;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = sizeof(struct message_t);

    // open client queue
    mq_unlink(client_queue_name);
    client_queue = mq_open(client_queue_name, O_CREAT | O_RDWR, QUEUE_PERMISSIONS, &attr);
    if (client_queue == -1) // error handler
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    printf("CLIENT QUEUE OPENED - %s\n", client_queue_name);

    server_queue = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (server_queue == -1) // error handler
    {
        perror("mq_open");
        send_stop_message();
    }
    printf("SERVER QUEUE OPENED\n\n");

    printf("-- CLIENT SENDING INIT MSG\n");
    send_message(MTYPE_INIT, "Hello server!", -1, client_queue_name, client_id, server_queue);
    handle_server_message(NOSKIP);
    
    printf("ENTER COMMANDS:\n");
    while (1)
    {
        handle_server_message(SKIP);
        printf("> ");
        char *input = NULL;
        size_t len = 0;
        ssize_t inputSize = 0;
        inputSize = getline(&input, &len, stdin);
        input[inputSize - 1] = '\0';
        handle_input_command(input);
    }

    return 0;
}