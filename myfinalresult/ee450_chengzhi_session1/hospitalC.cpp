/*
** hospitalC.cpp
 * code modified from Beej’s socket programming tutorial, Datagram Sockets(listener.c and talker.c)
 * dijkstras algorithm is modified from https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
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
#include <arpa/inet.h>
#include <netdb.h>
#include <string>

#define IP_ADDRESS "127.0.0.1"  // local host address
#define HC_PORT "32546"    // the UDP port of hospital C to connect with scheduler
#define SCHEDULER_UDP_PORT "33546" //udp port of scheduler
#define MAXBUFLEN 100

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <stdio.h>

#define MAP_LOC "./map.txt" //file location of map
#define NONE "None"


using namespace std;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

//create a client socket to send message to scheduler
int create_udp_sock(char *port, const void *msg) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(IP_ADDRESS, port, &hints, &servinfo)) != 0) {
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
    //printf("!!!talker: sent %d bytes to %s\n", numbytes, IP_ADDRESS);
    close(sockfd);
    return sockfd;
}


//reindex the original matrix to create a reindex graph
//use unordered_map where key is location in original map.txt and value is reindexed location
vector<vector<double>> reindex_graph(vector<vector<string>> matrix, unordered_map<string, int> umap, int num) {
    vector<vector<double>> graph(num, vector<double>(num));
    for (int i = 0; i < matrix.size(); i++) {
        int src, des;
        double distance;
        src = umap.at(matrix[i][0]);
        des = umap.at(matrix[i][1]);
        distance = atof(matrix[i][2].c_str());
        graph[src][des] = distance;
        graph[des][src] = distance;
    }
    return graph;
}

//read map.txt
vector<vector<string>> readfile(char *loc) {
    ifstream file(loc);
    if (!file)
        throw runtime_error("open file error");
    vector<vector<string>> matrix;
    string line, x, y, z;

    while (getline(file, line)) {
        vector<string> v;
        stringstream ss(line);
        ss >> x >> y >> z;
        v.push_back(x);
        v.push_back(y);
        v.push_back(z);
        matrix.push_back(v);
    }
    file.close();
    return matrix;
}


// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
// code modified from geeksforgeeks
int minDistance(double dist[], bool sptSet[], int num_vertex) {
    // Initialize min value
    int min = DBL_MAX, min_index;

    for (int v = 0; v < num_vertex; v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;

    return min_index;
}


// Function that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
// code modified from geeksforgeeks
double dijkstra(vector<vector<double>> graph, int src, int des, int num_vertex) {
    double dist[num_vertex]; // The output array. dist[i] will hold the shortest
    // distance from src to i

    bool sptSet[num_vertex]; // sptSet[i] will be true if vertex i is included in shortest
    // path tree or shortest distance from src to i is finalized

    // Initialize all distances as INFINITE and stpSet[] as false
    for (int i = 0; i < num_vertex; i++)
        dist[i] = DBL_MAX, sptSet[i] = false;

    // Distance of source vertex from itself is always 0
    dist[src] = 0;

    // Find shortest path for all vertices
    for (int count = 0; count < num_vertex - 1; count++) {
        // Pick the minimum distance vertex from the set of vertices not
        // yet processed. u is always equal to src in the first iteration.
        int u = minDistance(dist, sptSet, num_vertex);

        // Mark the picked vertex as processed
        sptSet[u] = true;


        if (u == des) {
            break;
        }

        // Update dist value of the adjacent vertices of the picked vertex.
        for (int v = 0; v < num_vertex; v++)

            // Update dist[v] only if is not in sptSet, there is an edge from
            // u to v, and total weight of path from src to v through u is
            // smaller than current value of dist[v]
            if (!sptSet[v] && graph[u][v] && dist[u] != DBL_MAX
                && dist[u] + graph[u][v] < dist[v])
                dist[v] = dist[u] + graph[u][v];
    }
    return dist[des];
}

//re-indexing the input nodes, then use dijkstra algorithm to get the shortest distance
//shortest distance is stored in map_msg
//return l_not_in to check if the location is in map or not
bool short_dist(char *loc_src, char *loc_des, char *map_msg, bool l_not_in) {

    vector<vector<string>> matrix = readfile(MAP_LOC);
    //re-index, use unordered_map to traverse all the vertex and assign a range of numbers as key-value pairs
    unordered_map<string, int> umap;
    int num_vertex = 0; //num_vertex is number of vertex
    for (int i = 0; i < matrix.size(); i++) {
        for (int j = 0; j < matrix[i].size() - 1; j++) {
            // if key is not present
            if (umap.find(matrix[i][j]) == umap.end()) {
                umap.insert({matrix[i][j], num_vertex++});
            }
        }
    }
    vector<vector<double>> graph;
    graph = reindex_graph(matrix, umap, num_vertex);
    l_not_in = (umap.find(loc_src) == umap.end());

    if (strcmp(loc_src, loc_des) == 0 || l_not_in) {
        //L is the same as hospital location (d=0): the distance is None or L is not in the map
        strcpy(map_msg, NONE);
    } else {
        //L is not the same as hospital location: the distance is the value of shortest path
        int src = umap.at(loc_src);  // location of client
        int des = umap.at(loc_des);   // 4
        double min_dist = dijkstra(graph, src, des, num_vertex);
        sprintf(map_msg, "%f", min_dist);
    }
    return l_not_in;
}
//calculate availability
double availability_result(double capacity, double occupation) {
    return (capacity - occupation) / capacity;
}
//calculate score
double score_result(double distance, double availability) {
    return 1 / (distance * (1.1 - availability));
}

int main(int argc, char *argv[]) {
//    argv[1] = "2";
//    argv[2] = "10";
//    argv[3] = "8";
    int udp_sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;

    socklen_t addr_len;
   
    //make sure enough arguments
    if (argc != 4) {
        fprintf(stderr, "usage: hospital initialization requires 3 arguments\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;// use my IP

    if ((rv = getaddrinfo(IP_ADDRESS, HC_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((udp_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
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

    //message 1
    printf("Hospital C is up and running using UDP on port %s.\n", HC_PORT);
    printf("Hospital C has total capacity %s and initial occupancy %s.\n", argv[2], argv[3]);
    //printf("listener: waiting to recvfrom...\n");

    addr_len = sizeof their_addr;
    //________________________send initialization information to scheduler____________________________________________________________________________
    char arg_send[MAXBUFLEN];
    sprintf(arg_send, "%s %s %s", argv[1], argv[2], argv[3]);
    int clientSock = create_udp_sock(SCHEDULER_UDP_PORT, arg_send);


    double capacity_num = atof(argv[2]);
    double occupation_num = atof(argv[3]);
    double score_num;
    double availability_num = availability_result(capacity_num, occupation_num);
    char availability[MAXBUFLEN];
    char score[MAXBUFLEN];


    while (1) {
        //-----------------------receive message from scheduler---------------
        //here message about client location
        char client_loc_char[MAXBUFLEN];
        if ((numbytes = recvfrom(udp_sockfd, client_loc_char, MAXBUFLEN - 1, 0,
                                 (struct sockaddr *) &their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        client_loc_char[numbytes] = '\0';

        char *loc_src = client_loc_char;     //location of client as source
        char *loc_des = argv[1];  //location of destination
        char distance_char[MAXBUFLEN];
        bool l_not_in;
        double distance_num;
        //if both src and des location are the same,which means client location is the same as hospital
        if (strcmp(loc_src, loc_des) == 0) {
            sprintf(distance_char, "%s", NONE);
        } else {
            //not the same, check if src is in map or not
            l_not_in = short_dist(loc_src, loc_des, distance_char, l_not_in);
            //if src is not in the map, need to avoid NONE
            if (strcmp(distance_char, NONE) != 0) {
                distance_num = atof(distance_char);
            }
        }

        if (strcmp(client_loc_char, "C") != 0) {
            //// message 2----------------------------------------
            printf("Hospital C has received input from client at location %s\n", client_loc_char);
            ////for location finding, no location found
            //// message 3----------------------------------------
            if (l_not_in) {
                printf("Hospital C does not have the location %s in map\n", client_loc_char);
                char *not_found = "location not found";
                if ((numbytes = sendto(udp_sockfd, not_found, strlen(not_found), 0, (struct sockaddr *) &their_addr,
                                       addr_len)) == -1) {
                    perror("listener: sendto");
                    exit(1);
                }
                //// message 4----------------------------------------
                printf("Hospital C has sent \"location not found\" to the Scheduler\n");
            }

            if (availability_num < 0 || availability_num > 1) {
                sprintf(availability, "%s", NONE);
            } else {
                sprintf(availability, "%f", availability_num);
            }
            //// message 5 -----------------------------
            //printf("Hospital C has capacity = %s, occupation= %s, availability = %s\n", argv[2], argv[3], availability);
            printf("Hospital C has capacity = %d, occupation= %d, availability = %s\n", (int)capacity_num, (int)occupation_num, availability);
            //// message 6 -----------------------------
            printf("Hospital C has found the shortest path to client, distance = %s\n", distance_char);

            if (strcmp(distance_char, NONE) == 0 || strcmp(availability, NONE) == 0) {
                sprintf(score, "%s", NONE);
            } else {
                score_num = score_result(distance_num, availability_num);
                sprintf(score, "%f", score_num);
            }
            //// message 7 -----------------------------
            printf("Hospital C has the score = %s\n", score);

            //For graph finding, after sending to the Scheduler:-----------------------------------------------------------------
            char score_distance[MAXBUFLEN];
            sprintf(score_distance, "%s %s", score, distance_char);
            if ((numbytes = sendto(udp_sockfd, score_distance, strlen(score_distance), 0,
                                   (struct sockaddr *) &their_addr, addr_len)) == -1) {
                perror("listener: sendto");
                exit(1);
            }
            //// message 8 -----------------------------
            printf("Hospital C has sent score = %s and distance = %s to the Scheduler\n", score, distance_char);


        } else {
            ////---------------------if Hospital C is chosen!!----------------------
            //cout << "debug: " << client_loc_char << endl;
            occupation_num++;
            availability_num = availability_result(capacity_num, occupation_num);
            printf("Hospital C has been assigned to a client, occupation is updated to %d, availability is updated to %f\n",
                   (int)occupation_num, availability_num);
        }
        //close(udp_sockfd);
    }

    return 0;
}




