//
//  main.c
//  techtalks
//
//  Created by Mehmet İNAÇ on 20/10/16.
//  Copyright © 2016 Mehmet İNAÇ. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#define TRUE  1
#define FALSE 0
#define MESSAGE_SIZE 2000
#define LISTEN_PORT 8888
#define GET "GET"
#define POST "POST"
#define NOT_ALLOWED "HTTP/1.1 405\n"

void signal_handler(int sig) {
    puts("Handle sig");
    exit(0);
}

void exit_with_message(char message[]) {
    puts(message);
    exit(-1);
}

// TODO: puts log into file
void put_log(char log_message[]) {
    puts(log_message);
}

void time_consuming_job() {
    int i;
    for(i = 0; i < 100000000; i++) {}
}

void *mock_endpoint(int *client_fd_ptr) {
    int client_fd;
    char client_message[MESSAGE_SIZE];
    client_fd = *(int *) client_fd_ptr;
    
    recv(client_fd , client_message , MESSAGE_SIZE - 1 , 0);
    
    // Mock time consuming job
    time_consuming_job();
    
    write(client_fd, "HTTP/1.1 200\r\n\r\nOK\r\n", strlen("HTTP/1.1 200\r\n\r\nOK\r\n"));
    close(client_fd);
    
    return NULL;
}

int main(int argc, const char * argv[]) {
    int listen_fd, client_fd, c;
    struct sockaddr_in server, client;
    pthread_t thread;

    // Register signal handler for graceful shutdown
    signal(SIGINT, signal_handler);

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (listen_fd == -1) {
        exit_with_message("Could not create socket!");
    }
    put_log("Socket has been created.");

    // Prepare server
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port        = htons(LISTEN_PORT);

    if (bind(listen_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        exit_with_message("Binding failure!");
    }
    put_log("Binding done.");

    listen(listen_fd, 100);
    put_log("Listening incoming request with backlog option set to 100.");

    // Accept all incoming requests
    while (TRUE) {
        client_fd = accept(listen_fd, (struct sockaddr *)&client, (socklen_t *)&c);

        if (client_fd < 0) {
            put_log("Could not accept connection!");
        } else {
            pthread_create(&thread, NULL, mock_endpoint, &client_fd);
            // mock_endpoint(&client_fd);
        }
    }

    return 0;
}
