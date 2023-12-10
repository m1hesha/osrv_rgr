#include <iostream>
#include <pthread.h>
#include <sys/neutrino.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <stdio.h>

struct my_msg {
    short type;
    int chid;
    struct sigevent sev[10];
};

#define MY_SERV "my_server_name"
#define MAX_THREADS 10
pthread_t client_tid, server_tid;

void my_thread_function(union sigval sv) {
    int rcvid;
    struct my_msg msg;
    int srv_coid;
    int chid = sv.sival_int;
    rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
    printf("New thread (TID: %lu) received:\n", pthread_self());
}

void* client_thread(void* arg) {
    int srv_coid;
    struct my_msg msg;
    int chid, coid, rcvid;
    chid = ChannelCreate(0);
    coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, 0);
    if ((srv_coid = name_open(MY_SERV, 0)) == -1) {
        fprintf(stderr, "Failed to find the server, error %d\n", errno);
        return NULL;
    }

    printf("Client thread (TID: %lu): Started\n", pthread_self());

    if (MsgSend(srv_coid, &msg, sizeof(msg), NULL, 0) == -1) {
        fprintf(stderr, "Error sending message from client, error %d\n", errno);
    } else {
        printf("Client thread (TID: %lu): Message sent\n", pthread_self());
    }
    return NULL;
}

void* server_thread(void* arg) {
    name_attach_t* attach = name_attach(NULL, MY_SERV, 0);
    int rcvid;
    struct my_msg msg;
    struct _msg_info msginfo;
    printf("Server thread (TID: %lu): Started\n", pthread_self());
    printf("Server thread (TID: %lu): Waiting\n", pthread_self());

    rcvid = MsgReceive(attach->chid, &msg, sizeof(msg.sev), &msginfo);
    MsgReply(rcvid, EOK, NULL, 0);
    sleep(2);
    for (int i = 0; i < MAX_THREADS; i++) {
        SIGEV_THREAD_INIT(&msg.sev[i], my_thread_function, NULL, NULL);
        msg.sev[i].sigev_value.sival_int = msg.chid;
    }
    for (int i = 0; i < MAX_THREADS; i++) {
        if (MsgDeliverEvent(rcvid, &msg.sev[i]) == -1) {
            fprintf(stderr, "Error delivering event from server, error %d\n", errno);
        }
    }
    return NULL;
}

int main() {
    pthread_create(&server_tid, NULL, server_thread, NULL);
    pthread_create(&client_tid, NULL, client_thread, NULL);

    pthread_join(server_tid, NULL);
    pthread_join(client_tid, NULL);
    return 0;
}
