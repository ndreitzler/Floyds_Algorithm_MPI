/*  Name: Nick Dreitzler
    Username: ndreitz
    CUID: C15564443
    Class: ECE 4730
    Professor: Walt Ligon
    Semester: Fall2020
    Filename: make-graph.c

    Purpose: Various functions to read/write/print graph
 */

#include "graph.h"

/*Malloc that also checks if malloc failed*/
void *my_malloc_serial (int bytes)
{
   void *buffer;
   if ((buffer = malloc ((size_t) bytes)) == NULL) {
      printf ("Error: Malloc failed\n");
      exit(1);
   }
   return buffer;
}

/*Allocates space for graph adjacency matrix
    Inputs: num_nodes: number of nodes in the graph
            A: matrix representation of graph with edge weights
    Return: pointer to data storage
 */
dtype *allocate_graph(int num_nodes, dtype ***A)
{
    dtype *storage;
    dtype **graph;
    int i;

    //Allocate space for graph
    storage = (dtype *)my_malloc_serial(sizeof(dtype)*num_nodes*num_nodes);
    graph = (dtype **)my_malloc_serial(sizeof(dtype *)*num_nodes);
    for(i = 0; i < num_nodes; i++)
        graph[i] = &storage[i*num_nodes];

    *A = graph;
    return(storage);
}

/*Writes the given num_node sized graph, A, into file_name
    Inputs: file_name: file to be written to
            num_nodes: number of nodes in the graph
            A: matrix representation of graph with edge weights
                A's data will be freed
 */
void write_graph(char *file_name, int num_nodes, int ***A)
{
    FILE *fptr;
    dtype **graph = *A;
    int i;
    
    OPEN_WRITE(fptr, file_name);

    //Makes the first integer the total number of nodes
    fwrite(&num_nodes, 1, sizeof(int), fptr);

    //write 1 row at a time
    for (i = 0; i < num_nodes; i++)
    {
        fwrite(graph[i], num_nodes, sizeof(dtype), fptr);
    }

    FREE(*A);
    A = NULL;

    CLOSE(fptr);
}

/*Reads the graph from the given binary file name.
  Function will malloc space for the graph
    Inputs: file_name: binary file to read from 
            num_nodes: number of nodes in the graph
            ***A: will contain pointer to graph data
    return: pointer to data storage
 */
dtype *read_graph(char *file_name, int *num_nodes, int ***A)
{
    FILE *fptr;
    dtype *storage;
    dtype **graph;
    int i;

    OPEN_READ(fptr, file_name);

    //First element in binary file is the number of nodes
    fread(num_nodes, 1, sizeof(int), fptr);

    storage = allocate_graph(*num_nodes, &graph);

    //Read contents
    for (i = 0; i < *num_nodes; i++)
    {
        fread(graph[i], *num_nodes, sizeof(dtype), fptr);
    }

    CLOSE(fptr);

    *A = graph;
    return storage;
}

/*Will print graph's num_node by num_node adjacency matrix
    Inputs: num_nodes: number of nodes in the graph
            **A: adjacency matrix to print
 */
void print_graph(int num_nodes, int **A)
{
    int i, j;

    // Line 1
    printf("%4s", "|");
    for(i = 0; i < num_nodes; i++)
        printf("%4d", i);
    printf("\n");

    //Line 2
    printf("%4s", "|");
    for(i = 0; i < num_nodes; i++)
        printf("%s", "----");
    printf("-\n");

    //The rest
    for(i = 0; i < num_nodes; i++)
    {
        printf("%2d%2s", i, "|");
        for(j = 0; j < num_nodes; j++)
        {
            printf("%4d", A[i][j]);
        }
        printf("\n");
    }
}

/*Determines which of (i,j) or (i,k) + (k, j) is the smallest path.
    -1 is treated as inf.
 */
dtype find_min(dtype ij, dtype ik, dtype kj)
{
    if((ik == -1) || (kj == -1))
        return ij;
    else if(ij == -1)
        return ik + kj;
    else
        return MIN(ij, ik + kj);
}