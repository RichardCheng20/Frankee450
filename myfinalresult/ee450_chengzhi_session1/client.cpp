/*
** client.cpp
 * code modified from Beej’s socket programming tutorial,A Simple Stream Client(client.c)
 * Name: Chengzhi Fu
 * USC NetID: 8898807546
 * ee 450 project
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define TCP_PORT "34546"  // the port client will be connecting to the scheduler

#define IP_ADDRESS "127.0.0.1" // local host address

#define MAXDATASIZE 100 // max number of bytes we can get at once

#define NONE "None" // errors, received message None

#define LOCATION_NOT_FOUND "location not found" //errors, no location

//code modefied from Beej A Simple Stream Client
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
//    argv[1] = "3"; //input debug
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client location\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(IP_ADDRESS, TCP_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("The client is up and running\n");
    //printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    //client send message to scheduler
    char arg_send[MAXDATASIZE];
    sprintf(arg_send, "%s", argv[1]);
    if ((numbytes = send(sockfd, arg_send, strlen(arg_send), 0)) == -1) {
        perror("send");
        exit(1);
    }
    printf("The client has sent query to Scheduler using TCP: client location %s\n", arg_send);

    //receive message from scheduler
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';

    //print out the final result
    if (strcmp(buf, LOCATION_NOT_FOUND) == 0) { // location not found 
        printf("The client has received results from the Scheduler: assigned to Hospital None\n");
        printf("Location %s not found\n", arg_send);
    } else if (strcmp(buf, NONE) != 0) { //for any possible assignment 
        printf("The client has received results from the Scheduler: assigned to Hospital %s\n",buf);
    } else if (strcmp(buf, NONE) == 0){ //None
        printf("The client has received results from the Scheduler: assigned to Hospital None\n");
        printf("Score = None, No assignment\n");
    }

    close(sockfd);

    return 0;
}

