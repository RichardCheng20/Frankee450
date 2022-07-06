/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define TCP_PORT "33546"  // the port client will be connecting to
#define UDP_PORT "34546"  // the port client will be connecting to
#define IP_ADDRESS "127.0.0.1" // local host address
#define MAXBUFLEN 100

#define BACKLOG 10	 // how many pending connections queue will hold

void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// scheduler connect with client through TCP
int tcp_with_client(char* tcp_port) {
    int sockfd;  // listen on sock_fd
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    int yes=1;

    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, TCP_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    return sockfd;

}

//scheduler connect with hospital through UDP
//scheduler as server, hostpital as client
int udp_with_hospital(char* udp_port) {
    int udp_sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(IP_ADDRESS, udp_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((udp_sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(udp_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(udp_sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("listener: waiting to recvfrom...\n");
    return udp_sockfd;
}

int main(void)
{

//    struct sockaddr_storage their_addr;
//    int  udp_sockfd;
//    udp_sockfd = udp_with_hospital(UDP_PORT);
//    char buf[MAXBUFLEN]; // buf to receive message from hospital
//    //scheduler receive message from hospital
//    socklen_t addr_len;
//    char s[INET6_ADDRSTRLEN];
//    addr_len = sizeof their_addr;
//    int numbytes;
//    if ((numbytes = recvfrom(udp_sockfd, buf, MAXBUFLEN-1 , 0,
//                             (struct sockaddr *)&their_addr, &addr_len)) == -1) {
//        perror("recvfrom");
//        exit(1);
//    }
//    printf("listener: got packet from %s\n",
//           inet_ntop(their_addr.ss_family,
//                     get_in_addr((struct sockaddr *)&their_addr),
//                     s, sizeof s));
//    buf[numbytes] = '\0';
//    printf("scheduler: received '%s'\n",buf);
//    close(udp_sockfd);

        struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];
    char cli_msg_buf[100]; //receive message from client
    int new_fd, sockfd, numbytes;
    sockfd = tcp_with_client(TCP_PORT);
    while(1) {
        /*
         * code for tcp connection with client
         */
        // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener

            //scheduler receives message from client
            if((numbytes = recv(new_fd, cli_msg_buf, sizeof cli_msg_buf, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            cli_msg_buf[numbytes] = '\0';
            printf("Hi, the scheduler received message from client is: %s\n", cli_msg_buf);

            //scheduler sends message to client
            if (send(new_fd, "Hello, world!yiyi5211", 25, 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}

