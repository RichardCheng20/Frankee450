/*
** scheduler.cpp
 * code modified from Beej’s socket programming tutorial,A Simple Stream Server(server.c) Datagram Sockets(listener.c and talker.c)
 * Name: Chengzhi Fu
 * USC NetID: 8898807546
 * ee 450 project
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

#include<string>
#include <sstream>
#include <vector>
#include <iostream>

#define UDP_PORT "33546"  // the udp port client will be connecting to
#define TCP_PORT "34546"  // the tcp port client will be connecting to

#define SERVERPORTA "30546" //the udp port scheduler will be connecting to hospital A
#define SERVERPORTB "31546"//the udp port scheduler will be connecting to hospital B
#define SERVERPORTC "32546"//the udp port scheduler will be connecting to hospital C
#define IP_ADDRESS "127.0.0.1" // local host address
#define MAXBUFLEN 100
#define NONE "None"
#define NO_REPLY "no_reply" //if hospital is not available(capacity < occupancy), then no reply about distance

#define BACKLOG 10     // how many pending connections queue will hold


using namespace std;

//code modefied from Beej
void sigchld_handler(int s) {
    (void) s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
//code modefied from Beej
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}


/*
 * scheduler connect with client through TCP, here scheduler is a TCP sever
 * tcp_port is port number of scheduler
 * code modified from Beej’s socket programming tutorial: A Simple Stream Server
 */
int tcp_with_client(char *tcp_port) {
    int sockfd;  // listen on sock_fd
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    int yes = 1;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(IP_ADDRESS, tcp_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
		// 1.创建套接字描述符 客户服务都有
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
		// 2 bind套接字地址和套接字描述符联系起来，只有服务
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

	// 3. 主动套接字转监听套接字，server才有
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

    // printf("server: waiting for connections...\n");
    return sockfd;
}

/*
 * scheduler connect with hospital through UDP, scheduler as a listener
 * code modified from Beej’s socket programming tutorial: Datagram Sockets
 */
int udp_with_hospital(char *udp_port) {
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
    for (p = servinfo; p != NULL; p = p->ai_next) {
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
        fprintf(stderr, "listener: failed to create socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);
    return udp_sockfd;
}

/*
 * used for receiving udp message
 * copy the received message into msg_from_hospital
 * code modified from Beej’s socket programming tutorial: Datagram Sockets
 */
void udp_receive(int udp_sockfd, char *msg_from_hospital) {
    char hospital_data[MAXBUFLEN];
    int numbytes_rec;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    addr_len = sizeof their_addr;
    if ((numbytes_rec = recvfrom(udp_sockfd, hospital_data, sizeof hospital_data, 0, (struct sockaddr *) &their_addr,
                                 &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
    hospital_data[numbytes_rec] = '\0';
    strcpy(msg_from_hospital, hospital_data);
    //close(udp_sockfd);
}

/*
 * used to send client location which is msg_send to hospital, and hospital will reply a message which will be stored in hospital_score_dis
 * server_port is udp port number of different hospitals
 * code modified from Beej’s socket programming tutorial: Datagram Sockets
 */
void udp_send_receive(int udp_sockfd, char *msg_send, char *server_port, char *hospital_score_dis) {
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(IP_ADDRESS, server_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((socket(p->ai_family, p->ai_socktype,
                    p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return;
    }

    if ((numbytes = sendto(udp_sockfd, msg_send, strlen(msg_send), 0,
                           p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }
    udp_receive(udp_sockfd, hospital_score_dis); //store the received message into hospital_score_dis
    //close(udp_sockfd);
}

//create a client socket to send message to hospital
//code modified from Beej’s socket programming tutorial: Datagram Sockets(talker.c)
int create_udp_sock(char *server_port, const void *msg) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(IP_ADDRESS, server_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }

    if ((numbytes = sendto(sockfd, msg, 100, 0,
                           p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);
    //printf("!!!scheduler: sent %d bytes to %s\n", numbytes, IP_ADDRESS);
    close(sockfd);
    return sockfd;
}

//process the received message to print out
vector<string> msg_process(char *msg_from_hospital) {
    vector<string> num_s;
    if (strcmp(msg_from_hospital, "location not found") == 0) { //if location not found, push NONE to the vector
        num_s.push_back(NONE);
        num_s.push_back(NONE);
    }
    string str(msg_from_hospital);
    istringstream iss(str);
    string msg_hospital;
    while (iss >> msg_hospital) {
        num_s.push_back(msg_hospital.c_str());
    }
    return num_s;
}

//compare score and distance between hospital B and hospital C
char *compareBC(double scoreB, double scoreC, double distanceB, double distanceC) {
    if (scoreB > scoreC) {
        return "B";
    } else if (scoreB < scoreC) {
        return "C";
    } else {
        if (distanceB <= distanceC) {
            return "B";
        } else {
            return "C";
        }
    }
}

//compare score and distance between hospital A and hospital C
char *compareAC(double scoreA, double scoreC, double distanceA, double distanceC) {
    if (scoreA > scoreC) {
        return "A";
    } else if (scoreA < scoreC) {
        return "C";
    } else {
        if (distanceA <= distanceC) {
            return "A";
        } else {
            return "C";
        }
    }
}

//compare score and distance between hospital A and hospital B
char *compareAB(double scoreA, double scoreB, double distanceA, double distanceB) {
    if (scoreA > scoreB) {
        return "A";
    } else if (scoreA < scoreB) {
        return "B";
    } else {
        if (distanceA <= distanceB) {
            return "A";
        } else {
            return "B";
        }
    }
}

int main(void) {
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];
    int new_fd, sockfd, numbytes;
    sockfd = tcp_with_client(TCP_PORT);

    int udp_sockfd;
    udp_sockfd = udp_with_hospital(UDP_PORT);
    //// message 1-------------------------------------------------
    printf("The Scheduler is up and running.\n");

    //receive initial data from hospital
    //// message 2.1-------------------------------------------------
    char msg_from_hospitalA[MAXBUFLEN];
    udp_receive(udp_sockfd, msg_from_hospitalA);
    vector<string> num_s_A = msg_process(msg_from_hospitalA);
    cout << "The Scheduler has received information from Hospital A: total capacity is " << num_s_A[1]
         << " and initial occupancy is " << num_s_A[2] << endl;

    //// message 2.2-------------------------------------------------
    char msg_from_hospitalB[MAXBUFLEN];
    udp_receive(udp_sockfd, msg_from_hospitalB);
    vector<string> num_s_B = msg_process(msg_from_hospitalB);
    cout << "The Scheduler has received information from Hospital B: total capacity is " << num_s_B[1]
         << " and initial occupancy is " << num_s_B[2] << endl;

    //// message 2.3-------------------------------------------------
    char msg_from_hospitalC[MAXBUFLEN];
    udp_receive(udp_sockfd, msg_from_hospitalC);
    vector<string> num_s_C = msg_process(msg_from_hospitalC);
    cout << "The Scheduler has received information from Hospital C: total capacity is " << num_s_C[1]
         << " and initial occupancy is " << num_s_C[2] << endl;

    int capacity_iA = atoi(num_s_A[1].c_str());
    int occupancy_iA = atoi(num_s_A[2].c_str());

    int capacity_iB = atoi(num_s_B[1].c_str());
    int occupancy_iB = atoi(num_s_B[2].c_str());

    int capacity_iC = atoi(num_s_C[1].c_str());
    int occupancy_iC = atoi(num_s_C[2].c_str());

    while (1) {
        //tcp connection with client
        char cli_msg_buf[MAXBUFLEN]; //receive message from client
        struct sockaddr_storage their_addr;
        sin_size = sizeof their_addr;
		// 4. accept等待客户端链接请求，返回已连接描述符， their_addr是客户端的套接字地址
        new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof s);
        //printf("server: got connection from %s\n", s);

        //if (!fork()) { // this is the child process
        //close(sockfd); // child doesn't need the listener

        //scheduler receives message from client
        if ((numbytes = recv(new_fd, cli_msg_buf, sizeof cli_msg_buf, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        cli_msg_buf[numbytes] = '\0';
//          printf("scheduler received message from client is: %s\n", cli_msg_buf);
        //// message 3-------------------------------------------------
        printf("The Scheduler has received client at location %s from the client using TCP over port %s\n",
               cli_msg_buf, TCP_PORT);

        //udp
        //scheduler send client location to hospital
        char hospital_score_disA[MAXBUFLEN];
        vector<string> score_disA;
        if (capacity_iA > occupancy_iA) {
            udp_send_receive(udp_sockfd, cli_msg_buf, SERVERPORTA, hospital_score_disA);
            //// message 4.1-------------------------------------------------
            printf("The Scheduler has sent client location to Hospital A using UDP over port %s\n", UDP_PORT);
            score_disA = msg_process(hospital_score_disA);
        } else { //if hospital is not available, then scheduler won't send request to this hospital then score is NONE, and distance is NO_REPLY
            score_disA.push_back(NONE);
            score_disA.push_back(NO_REPLY);
        }
//            //DEBUG
//            printf("check hospital_score_disA = %s \n", hospital_score_disA);
//            cout << "check hospital_score = " << score_disA[0] << " hospital_disA = " << score_disA[1] << endl;

        char hospital_score_disB[MAXBUFLEN];
        vector<string> score_disB;
        if (capacity_iB > occupancy_iB) {
            udp_send_receive(udp_sockfd, cli_msg_buf, SERVERPORTB, hospital_score_disB);
            //// message 4.2-------------------------------------------------
            printf("The Scheduler has sent client location to Hospital B using UDP over port %s\n", UDP_PORT);
            score_disB = msg_process(hospital_score_disB);
        } else { //if hospital is not available, then scheduler won't send request to this hospital then score is NONE, and distance is NONE
            score_disB.push_back(NONE);
            score_disB.push_back(NO_REPLY);
        }


        char hospital_score_disC[MAXBUFLEN];
        vector<string> score_disC;
        if (capacity_iC > occupancy_iC) {
            udp_send_receive(udp_sockfd, cli_msg_buf, SERVERPORTC, hospital_score_disC);
            //// message 4.3-------------------------------------------------
            printf("The Scheduler has sent client location to Hospital C using UDP over port %s\n", UDP_PORT);
            score_disC = msg_process(hospital_score_disC);
        } else { //if hospital is not available, then scheduler won't send request to this hospital then score is NONE, and distance is NONE
            score_disC.push_back(NONE);
            score_disC.push_back(NO_REPLY);
        }


        //// message 5.1-------------------------------------------------
        if (strcmp(score_disA[1].c_str(), NO_REPLY) != 0) {
            cout << "The Scheduler has received map information from Hospital A: the score = " << score_disA[0]
                 << " and the distance = " << score_disA[1] << endl;
        }

        //// message 5.2-------------------------------------------------
        if (strcmp(score_disB[1].c_str(), NO_REPLY) != 0) {
            cout << "The Scheduler has received map information from Hospital B: the score = " << score_disB[0]
                 << " and the distance = " << score_disB[1] << endl;
        }
        //// message 5.3-------------------------------------------------
        if (strcmp(score_disC[1].c_str(), NO_REPLY) != 0) {
            cout << "The Scheduler has received map information from Hospital C: the score = " << score_disC[0]
                 << " and the distance = " << score_disC[1] << endl;
        }

        //store distance and score in string
        string dA = score_disA[1];
        string dB = score_disB[1];
        string dC = score_disC[1];
        string sA = score_disA[0];
        string sB = score_disB[0];
        string sC = score_disC[0];

        double distanceA;
        double distanceB;
        double distanceC;
        //if distance is valid not NONE or NO_REPLY
        if (strcmp(dA.c_str(), NONE) != 0 && strcmp(dA.c_str(), NO_REPLY) != 0) { 
            distanceA = atof(dA.c_str());
        }
        if (strcmp(dB.c_str(), NONE) != 0 && strcmp(dA.c_str(), NO_REPLY) != 0) {
            distanceB = atof(dB.c_str());
        }
        if (strcmp(dC.c_str(), NONE) != 0 && strcmp(dA.c_str(), NO_REPLY) != 0) {
            distanceC = atof(dC.c_str());
        }

        double scoreA;
        double scoreB;
        double scoreC;
        //if the score is not none, change it to double
        if (strcmp(sA.c_str(), NONE) != 0) {
            scoreA = atof(sA.c_str());
        }
        if (strcmp(sB.c_str(), NONE) != 0) {
            scoreB = atof(sB.c_str());
        }
        if (strcmp(sC.c_str(), NONE) != 0) {
            scoreC = atof(sC.c_str());
        }

        //put final compare result in assign_result
        char assign_result[MAXBUFLEN];
        if (strcmp(dA.c_str(), NONE) == 0 || strcmp(dB.c_str(), NONE) == 0 || strcmp(dC.c_str(), NONE) == 0
        || ((strcmp(dA.c_str(), NO_REPLY) == 0) && (strcmp(dB.c_str(), NO_REPLY) == 0) && (strcmp(dC.c_str(), NO_REPLY) == 0))) {
            sprintf(assign_result, "%s", NONE);
        } else {
            if ((strcmp(sA.c_str(), NONE) == 0 || strcmp(dA.c_str(), NO_REPLY) == 0) && (strcmp(sB.c_str(), NONE) != 0 && strcmp(dB.c_str(), NO_REPLY) != 0) &&
                    (strcmp(sC.c_str(), NONE) != 0 && strcmp(dC.c_str(), NO_REPLY) != 0)) {//BC compare
                char *c_resultBC = compareBC(scoreB, scoreC, distanceB, distanceC);
                sprintf(assign_result, "%s", c_resultBC);
            } else if ((strcmp(sB.c_str(), NONE) == 0 || strcmp(dB.c_str(), NO_REPLY) == 0) && (strcmp(sA.c_str(), NONE) != 0 && strcmp(dA.c_str(), NO_REPLY) != 0) &&
                    (strcmp(sC.c_str(), NONE) != 0 && strcmp(dC.c_str(), NO_REPLY) != 0)) {//AC compare
                char *c_resultAC = compareAC(scoreA, scoreC, distanceA, distanceC);
                sprintf(assign_result, "%s", c_resultAC);
            } else if ((strcmp(sC.c_str(), NONE) == 0 || strcmp(dC.c_str(), NO_REPLY) == 0) && (strcmp(sA.c_str(), NONE) != 0 && strcmp(dA.c_str(), NO_REPLY) != 0) &&
                    (strcmp(sB.c_str(), NONE) != 0 && strcmp(dB.c_str(), NO_REPLY) != 0)) { //AB compare
                char *c_resultAB = compareAB(scoreA, scoreB, distanceA, distanceB);
                sprintf(assign_result, "%s", c_resultAB);
            } else if ((strcmp(sA.c_str(), NONE) == 0 || strcmp(dA.c_str(), NO_REPLY) == 0) && (strcmp(sB.c_str(), NONE) == 0 || strcmp(dB.c_str(), NO_REPLY) == 0) &&
                    (strcmp(sC.c_str(), NONE) != 0 && strcmp(dC.c_str(), NO_REPLY) != 0)) { //only C
                sprintf(assign_result, "%s", "C");
            } else if ((strcmp(sA.c_str(), NONE) == 0 || strcmp(dA.c_str(), NO_REPLY) == 0) && (strcmp(sC.c_str(), NONE) == 0 || strcmp(dC.c_str(), NO_REPLY) == 0) &&
                    (strcmp(sB.c_str(), NONE) != 0 && strcmp(dB.c_str(), NO_REPLY) != 0)) { //only B
                sprintf(assign_result, "%s", "B");
            } else if ((strcmp(sB.c_str(), NONE) == 0 || strcmp(dB.c_str(), NO_REPLY) == 0) && (strcmp(sC.c_str(), NONE) == 0 || strcmp(dC.c_str(), NO_REPLY) == 0) &&
                    (strcmp(sA.c_str(), NONE) != 0 && strcmp(dA.c_str(), NO_REPLY) != 0)) { //only A
                sprintf(assign_result, "%s", "A");
            } else if ((strcmp(sB.c_str(), NONE) == 0 || strcmp(dB.c_str(), NO_REPLY) == 0) && (strcmp(sC.c_str(), NONE) == 0 || strcmp(dC.c_str(), NO_REPLY) == 0) &&
                    (strcmp(sA.c_str(), NONE) || strcmp(dA.c_str(), NO_REPLY) == 0) == 0) { //None
                sprintf(assign_result, "%s", NONE);
            } else if ((strcmp(sB.c_str(), NONE) != 0 && strcmp(sB.c_str(), NONE) != 0) && (strcmp(sC.c_str(), NONE) != 0 && strcmp(sC.c_str(), NONE) != 0) &&
                    (strcmp(sA.c_str(), NONE) != 0 && strcmp(sA.c_str(), NONE) != 0)) { //ABC compare
                char *c_resultAB = compareAB(scoreA, scoreB, distanceA, distanceB);
                if (strcmp(c_resultAB, "A") == 0) {
                    char *c_resultAC = compareAC(scoreA, scoreC, distanceA, distanceC);
                    sprintf(assign_result, "%s", c_resultAC);
                } else if (strcmp(c_resultAB, "B") == 0) {
                    char *c_resultBC = compareBC(scoreB, scoreC, distanceB, distanceC);
                    sprintf(assign_result, "%s", c_resultBC);
                }
            }
        }

//            //DEBUG
//            printf("hospital_score_disA = %s \n", hospital_score_disA);
//            printf("assign_result = %s \n", assign_result);
        if (strcmp(assign_result, NONE) != 0) {
            printf("The Scheduler has assigned Hospital %s to the client\n", assign_result);

            if (send(new_fd, assign_result, strlen(assign_result), 0) == -1)
                perror("send");
            printf("The Scheduler has sent the result to client using TCP over port %s\n", TCP_PORT);

            if (strcmp(assign_result, "A") == 0) {
                occupancy_iA++;
                int clientSocked = create_udp_sock(SERVERPORTA, assign_result);
                printf("The Scheduler has sent the result to Hospital %s using UDP over port %s\n", assign_result,
                       UDP_PORT);
            } else if (strcmp(assign_result, "B") == 0) {
                occupancy_iB++;
                int clientSocked = create_udp_sock(SERVERPORTB, assign_result);
                printf("The Scheduler has sent the result to Hospital %s using UDP over port %s\n", assign_result,
                       UDP_PORT);
            } else if (strcmp(assign_result, "C") == 0) {
                occupancy_iC++;
                int clientSocked = create_udp_sock(SERVERPORTC, assign_result);
                printf("The Scheduler has sent the result to Hospital %s using UDP over port %s\n", assign_result,
                       UDP_PORT);
            }
        } else if (strcmp(hospital_score_disA, "location not found") == 0 ||
                   strcmp(hospital_score_disB, "location not found") == 0
                   || strcmp(hospital_score_disC, "location not found") == 0) {
            char *location_err = "location not found";
            if (send(new_fd, location_err, strlen(location_err), 0) == -1)
                perror("send");
            printf("The Scheduler has sent the result to client using TCP over port %s\n", TCP_PORT);
        } else {
            if (send(new_fd, NONE, strlen(NONE), 0) == -1)
                perror("send");
            printf("The Scheduler has sent the result to client using TCP over port %s\n", TCP_PORT);
        }
        close(new_fd);
//            exit(0);
//        }
//        close(new_fd);  // parent doesn't need this
    }

    return 0;
}

