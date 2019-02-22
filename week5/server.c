//
// Created by ilya on 2/14/19.
//

#include "common.h"
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <zconf.h>

#define SERVER_PORT     2000

char             data_buffer[1024];
struct student_t student;

int process_connection(int sockfd) {
    struct sockaddr_in client_addr;
    ssize_t            sent_recv_bytes = 0;
    socklen_t          addr_len        = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));
    while (1) {
        printf("Server ready to service client msgs.\n");
        memset(data_buffer, 0, sizeof(data_buffer));
        sent_recv_bytes = recvfrom(sockfd, (char *) &student, sizeof(struct student_t), 0,
                                   (struct sockaddr *) &client_addr, &addr_len);
        printf("Server recvd %zi bytes from client %s:%u\n", sent_recv_bytes,
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        if (sent_recv_bytes == 0) {
            close(sockfd);
            break;
        }
        //change student...
        bzero(student.group, 16);
        strcpy(student.group, "Dropped");
        //send him back
        sent_recv_bytes = sendto(sockfd, (char *) &student, sizeof(struct student_t), 0,
                                 (struct sockaddr *) &client_addr, addr_len);
        printf("Server sent %zi bytes in reply to client\n", sent_recv_bytes);
    }
}

int main(int argc, char **argv) {
    int                sockfd = 0;
    struct sockaddr_in server_addr;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket creation failed\n");
        exit(1);
    }
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
               (const void *)&optval, sizeof(int));
    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = SERVER_PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("socket bind failed\n");
        return 0;
    }
    while (1) {
        process_connection(sockfd);
    }
    return 0;
}


