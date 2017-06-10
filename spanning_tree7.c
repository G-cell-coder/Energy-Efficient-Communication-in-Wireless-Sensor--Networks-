// A C / C++ program for Prim's Minimum Spanning Tree (MST) algorithm.
// The program is for adjacency matrix representation of the graph
 
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <math.h>


// Number of vertices in the graph
#define V 20 
#define TREE 1
#define REQUEST 2
#define REPLY 3


struct node_addr {
    int  id;
    char addr[32];
    int port;
    int child;
};

struct node_addr my_addr;
struct node_addr *neighbors[10] = {0,};
int my_children[V] = {-1,};
int children_temp[V] = {-1,};
int my_parent;
int parents[V] = {-1,};

void sock_conn(int node_id, double graph[V][V])
{

    struct sockaddr_in servaddr, cliaddr;
    int sockfd, n, i, j;
    socklen_t len;
    char buff[1000];
    int cliport;
    char addr[32];
    char hello[] = "Hello Radhika";
    int span_sent = 0;
    int count = 0;
    int avg = 0;
    
    sockfd = socket (AF_INET,SOCK_DGRAM,0);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons (my_addr.port);
    bind (sockfd,(struct sockaddr *) &servaddr, sizeof(servaddr));

    printf("My port is : %d\n", my_addr.port);
    if (!node_id) {
        //find who are their nearest ppl and send the graph info to them one by one in the for loop
        for (i = 0; i < V && neighbors[i]; i++) {
            //TODO:separate the string in neighbors[i] into ip addr and port and fill up following
            bzero(&cliaddr, sizeof(cliaddr));
            cliaddr.sin_family = AF_INET;
            cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
            cliaddr.sin_port = htons(neighbors[i]->port);
            // TODO:Now using, cli send.
            printf("Sending spanning tree to port %d\n", neighbors[i]->port);
            bzero(buff, sizeof(buff));
            buff[0] = TREE;
            buff[1] = V;
            for (j = 0; j < V; j++) {
                buff[j+2] = parents[j];
            }
            sendto(sockfd, buff, V+3, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
            span_sent = 1;
        }
        printf("Node : %d, port %d, Sent spanning tree!\n", node_id, my_addr.port);

        sleep(20);

        for (i = 0; i < V && neighbors[i]; i++) {
            //TODO:separate the string in neighbors[i] into ip addr and port and fill up following
            bzero(&cliaddr, sizeof(cliaddr));
            cliaddr.sin_family = AF_INET;
            cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
            cliaddr.sin_port = htons(neighbors[i]->port);
            // TODO:Now using, cli send.
            printf("Sending spanning tree to port %d\n", neighbors[i]->port);
            bzero(buff, sizeof(buff));
            buff[0] = REQUEST;
            sendto(sockfd, buff, 2, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
            span_sent = 1;
        }

    }
    
    for (;;)
    {
        len = sizeof(cliaddr);
        n = recvfrom (sockfd, buff, 1000, 0, (struct sockaddr *) &cliaddr, &len);
        printf("-----------------------------------------------------\n");
        printf ("received the following from %d: \n", ntohs(cliaddr.sin_port));
        printf ("%s\n", buff);
        printf ("====================================================\n");
        if (span_sent && buff[0] == TREE) {
            printf("got the same thing again!\n");
            continue;
        }

	    if (buff[0] == TREE) {
            for (i = 0; i < V; i++) {
                if ( buff[i+2] == node_id) {
                    my_children[count] = i;
                    printf("My child : %d\n", my_children[count]);
                    count++;
                }
            }
            my_parent = buff[node_id+2];
            printf("My parent : %d\n", my_parent);
        }

        if (buff[0] == TREE || buff[0] == REQUEST) {
            if (buff[0] == REQUEST && my_children[0] == -1) {
                // No children for me. I wil lsend back reply to my parent
                for (i = 0; neighbors[i] && neighbors[i]->id != my_parent; i++) {
                    continue;
                }
                bzero(&cliaddr, sizeof(cliaddr));
                cliaddr.sin_family = AF_INET;
                cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
                cliaddr.sin_port = htons(neighbors[i]->port);
                bzero(buff, sizeof(buff));
                buff[0] = REPLY;
                buff[1] = rand() % 100;
                // TODO:Now using, cli send.
                printf("Sending spanning tree to port %d\n", neighbors[i]->port);
                sendto(sockfd, buff, 1000, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
                continue;
            }
            for (i = 0; i < V && neighbors[i]; i++) {
                // Dont send the request to my parent again to save power.
                if (neighbors[i]->id == my_parent) {
                    continue;
                }

                bzero(&cliaddr, sizeof(cliaddr));
                cliaddr.sin_family = AF_INET;
                cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
                cliaddr.sin_port = htons(neighbors[i]->port);
                // TODO:Now using, cli send.
                printf("Sending spanning tree to port %d\n", neighbors[i]->port);
                sendto(sockfd, buff, 1000, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
                span_sent = 1;
            }
        } else if (buff[0] == REPLY) {
            //collect the children temperature
            for (i = 0; i < V; i++) {
                if (children_temp[i] != -1) 
                    continue;
                children_temp[i] = buff[1];
            }
            count = 0;
            for (j = 0; neighbors[j]; j++) {
                count++;
            }
            if (i == count) {
                for (j = 0; j < V; j++) {
                    avg = (avg + children_temp[j])/2;
                }
                    
                if (node_id) {
                    //send to parent
                    bzero(&cliaddr, sizeof(cliaddr));
                    cliaddr.sin_family = AF_INET;
                    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
                    cliaddr.sin_port = htons(neighbors[i]->port);
                    bzero(buff, sizeof(buff));
                    buff[0] = REPLY;
                    buff[1] = (avg + (rand()%100))/2;
                    // TODO:Now using, cli send.
                    printf("Sending spanning tree to port %d\n", neighbors[i]->port);
                    sendto(sockfd, buff, 1000, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
                } else {
                    // I am root node. Print temp.
                    printf("Average Temp : %d\n", (avg + (rand()%100))/2);
                }
            }
        }
    }

    close(sockfd);
}

int topologyfile_parser(int node_id, double graph[V][V]);
 
// A utility function to find the vertex with minimum key value, from
// the set of vertices not yet included in MST
int minKey(int key[], bool mstSet[])
{
   // Initialize min value
   printf("...in minKey()\n");
   int min = INT_MAX, min_index;
   int v = 0;
    
   for (v = 0; v < V; v++)
     if (mstSet[v] == false && key[v] < min)
         min = key[v], min_index = v;
 
   return min_index;
}
 
// A utility function to print the constructed MST stored in parent[]
void printMST(int node_id, int *parent, int n, double graph[V][V])
{
   
   printf("...in printMST()\n");
   FILE *fp;
   printf("Edge   Weight\n");


   int i = 1,count=0;
 
   fp = fopen ("parentfile.txt", "w");
   if (fp == NULL) {
       printf("Couldnt open topofile.txt\n");
       return;
   }
   // printf( " child");
   for (i = 1; i < V; i++) {
      printf("i : %d, parent[i] : %d\n", i, parent[i]);
      if ( parent[i] == node_id) {
          my_children[count]=i;
	  count++;
      }
      fprintf(fp, "%d - %d    %d \n", parent[i], i, graph[i][parent[i]]);
   }

   my_parent=parent[node_id];
  /* for ( i= 1; i< V ; i++) {
   if(parent[i]==i)
   printf("child of %d is %d\n", parent [i] == i);
   }*/
   fclose(fp);
 
}
 
// Function to construct and print MST for a graph represented using adjacency
// matrix representation
void primMST(int node_id,double graph[V][V])
{
     printf("...in primMSTminKey()\n");
     int key[V];   // Key values used to pick minimum weight edge in cut
     bool mstSet[V];  // To represent set of vertices not yet included in MST
     int i = 0; 
     // Initialize all keys as INFINITE
     for (i = 0; i < V; i++) {
        key[i] = INT_MAX;
        mstSet[i] = false;
     }
 
     // Always include first 1st vertex in MST.
     key[0] = 0;     // Make key 0 so that this vertex is picked as first vertex
     parents[0] = -1; // First node is always root of MST
     int count = 0;
  
     // The MST will have V vertices
     for (count = 0; count < V-1; count++)
     {
        // Pick thd minimum key vertex from the set of vertices
        // not yet included in MST
        int u = minKey(key, mstSet);
        int v = 0; 
        // Add the picked vertex to the MST Set
        mstSet[u] = true;
 
        // Update key value and parent index of the adjacent vertices of
        // the picked vertex. Consider only those vertices which are not yet
        // included in 
        for (v = 0; v < V; v++)
 
           // graph[u][v] is non zero only for adjacent vertices of m
           // mstSet[v] is false for vertices not yet included in MST
           // Update the key only if graph[u][v] is smaller than key[v]
          if (graph[u][v] && mstSet[v] == false && graph[u][v] <  key[v]) {
              parents[v]  = u;
              key[v] = graph[u][v];
              printf("v = %d, parents[v] = %d\n", v, parents[v]);
          }
     }
 
     // print the constructed MST
     printMST(node_id, parents, V, graph);
}

double distance(int *cx, int *cy, int i, int j)
{
    double dist = 0;

    //printf("i %d, j %d, cx[i] %d, cx[j] %d, cy[i] %d, cy[j] %d\t", i, j, cx[i], cx[j], cy[i], cy[j]);
    dist = sqrt(((cx[j] - cx[i])*(cx[j] - cx[i])) + ((cy[j] - cy[i])*(cy[j] - cy[i])));

    if (dist <= 5) {
        return dist;
    } else {
        return 0;
    }

}

int topologyfile_parser(int my_id, double graph[V][V]) 
{

    printf("...in topologyfile_parser()\n");
    FILE *fp;
    int i, j;
    int index = 0;
    char line[60];
    struct node_addr *addr_arr[V] = {NULL,};
    char node_id = 0;
    int count = 0;
    int co_ord_x[V] = {0,}, co_ord_y[V] = {0,}, port = 0;
    char addr[32] = {0,};
    int num1 = 0, num2 = 0, num3 = 0;
    
    fp = fopen ("Topology.txt", "r");
   
    if (!fp) {
        return 1;
    }

    while (fgets(line, 60, fp) != NULL)
    {
        sscanf(line, "%c, %d, %d, %s %d, %d, %d, %d", &node_id, &co_ord_x[index], &co_ord_y[index], addr, &port, &num1, &num2, &num3);
        printf("node_id %c, co_ord_x %d, co_ord_y %d, addr %s, port %d, num1 %d, num2 %d, num3 %d\n",
               node_id, co_ord_x[index], co_ord_y[index], addr, port, num1, num2, num3);
        addr_arr[index] = malloc(sizeof(struct node_addr));
        memcpy(addr_arr[index]->addr, addr, strlen(addr));
        addr_arr[index]->port = port;
        addr_arr[index]->id = node_id - 65;
        if (index == my_id) {
            memcpy(my_addr.addr, addr, strlen(addr));
            my_addr.port = port;
	        my_addr.id = my_id;
        }
        index++;
    }

    for (i = 0; i < V; i++) {
        for (j = 0; j < V; j++) {
            graph[i][j] = distance(co_ord_x, co_ord_y, i, j);
            printf("%.2f\t", graph[i][j]);
        }
        printf("\n");
    }

    for (i = 0; i < V; i++) {
        if (graph[my_id][i] != 0) {
            neighbors[count] = addr_arr[i];
            printf("neighbors[%d] : %d\n", count, neighbors[count]->id);
            count++;
        } else {
            free(addr_arr[i]);
            addr_arr[i] = NULL;
        }
    }

    /*b1=atoi(b);
    c1=atoi(c);
    e1=atoi(e);
    f1=atoi(f);
    
    printf("%c %d %d", a, b1, c1);*/
    
    fclose(fp);
    
    return 0;

}

// Client side of socket programming 
int main(int argc, const char* argv[])
{
    int sockfd, i;
    int node_id = -1;
    // struct sockaddr_in servaddr;
    int port;
    char recvline[1024];
    int n;
    double graph[V][V] = {0,};

    if (argc > 1 && *argv[1] >= 65 && *argv[1] <= 84) {
        node_id = *argv[1] - 65;
    }

    /*int graph[V][V] = {{0, 4.24, 4.47,4.47, 5, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {4.24, 0, 0, 0, 0,0,0,0,0,0,0,0,0,4.12,5,0,0,0},
                      {4.47, 0, 0, 0, 0,0,4.12,5,0,0,0,0,0,0,0,0,0,0,0,0},
                      {4.47, 0, 0, 0, 0,0,0,0,5,5,0,0,0,0,0,0,0,0,0,0},
                      {5, 0, 0, 0, 0,5,0,0,0,0,4.12,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,5,0,0,0,0,0,0,4,4,0,0,0,0,0,0,0},
                      {0,0,4.12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,4.12,0,0,0,0,0.0},
                      {0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,4.12,0,0,0,0,0,0,0,0,0},
                      {0,4.12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4.12,0},
                      {5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4.12,3.16,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4.12,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3.16,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,4.12,0,0,0,0,3},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3}
                      };*/

    topologyfile_parser(node_id, graph);
    if (node_id == 0) {
        // Print the solution
        primMST(node_id, graph);
    }

    sock_conn(node_id, graph);

    for(i = 0; i < V && neighbors[i]; i++) {
        free(neighbors[i]);
    }

    return 0;

}
