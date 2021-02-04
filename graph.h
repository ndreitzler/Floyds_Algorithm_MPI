/*  Name: Nick Dreitzler
    Username: ndreitz
    CUID: C15564443
    Class: ECE 4730
    Professor: Walt Ligon
    Semester: Fall2020
    Filename: graph.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

typedef int dtype;

#define FREE(x)                                         \
do {                                                    \
    free(x);                                            \
    x = NULL;                                           \
} while(0)

#define CLOSE(x)                                        \
do {                                                    \
    fclose(x);                                          \
    x = NULL;                                           \
} while(0)

#define OPEN_READ(x, y)                                 \
do {                                                    \
    x = fopen(y, "r");                                  \
    if(x == NULL)                                       \
    {                                                   \
        printf("%s failed to open for read\n", y);      \
        exit(-1);                                       \
    }                                                   \
} while(0)

#define OPEN_WRITE(x, y)                                \
do {                                                    \
    x = fopen(y, "w");                                  \
    if(x == NULL)                                       \
    {                                                   \
        printf("%s failed to open for write\n", y);     \
        exit(-1);                                       \
    }                                                   \
} while(0)

#define MIN( a, b ) ( ( a < b) ? a : b ) 

/*Malloc that also checks if malloc failed*/
void *my_malloc_serial(int bytes);

/*Allocates space for graph adjacency matrix
    Inputs: num_nodes: number of nodes in the graph
            A: matrix representation of graph with edge weights
    Return: pointer to data storage
 */
dtype *allocate_graph(int num_nodes, dtype ***A);

/*Writes the given num_node sized graph, A, into file_name
    Inputs: file_name: file to be written to
            num_nodes: number of nodes in the graph
            A: matrix representation of graph with edge weights
                A's data will be freed
 */
void  write_graph(char *file_name, int num_nodes, int ***A);

/*Reads the graph from the given binary file name.
  Function will malloc space for the graph
    Inputs: file_name: binary file to read from 
            num_nodes: number of nodes in the graph
            ***A: will contain pointer to graph data
    return: pointer to data storage
 */
dtype *read_graph(char *file_name, int *num_nodes, int ***A);

/*Will print graph's num_node by num_node adjacency matrix
    Inputs: num_nodes: number of nodes in the graph
            **A: adjacency matrix to print
 */
void print_graph(int num_nodes, int **A);

/*Determines which of (i,j) or (i,k) + (k, j) is the smallest path.
    -1 is treated as inf.
 */
dtype find_min(dtype ij, dtype ik, dtype kj);
