/*  Name: Nick Dreitzler
    Username: ndreitz
    CUID: C15564443
    Class: ECE 4730
    Professor: Walt Ligon
    Semester: Fall2020
    Filename: make-graph.c

    Purpose: To create a binary file of a n by n matrix 
        representing a graph of n nodes, with directional 
        edge weights 0 to r and a probability of an edge 
        of r/p. Non-edges are represented with -1.
        Data are integers and the number n is stored as 
        the first word of the file.

    Input:  -n <number of nodes>, default 100
            -r <Max edge weight>, default 100
            -p <probability of edge r/p>, default 150
            -o <output file>, default graph.bin
            -h <print usage>
    
    Output: Binary file with n by n matrix representing a 
        graph with n node
 */

#include "graph.h"

#define BUFFER_SIZE 4096

dtype *create_graph(int num_nodes, int max_weight, int prob, dtype ***A);

int main(int argc, char **argv)
{
    dtype **graph;          /*Data written to file*/ 
    dtype *storage;         /*Contains all data for graph*/
    int num_nodes = 100;    /*Number of nodes in graph*/
    int max_weight = 100;   /*Max edge weight*/
    int prob = 150;         /*max_weight/prob is probability of connection between nodes*/ 
    int opt;
    char filename_default[] = "graph.bin";
    char *out_file = NULL;  /*Output file*/

    srand(time(0));

    //Parse Input
    while((opt = getopt(argc, argv, "n:r:p:o:h")) != -1) 
    {
        switch(opt) 
        { 
            case 'n':
                num_nodes = atoi(optarg);
                break;
            case 'r':
                max_weight = atoi(optarg);
                break;
            case 'p':
                prob = atoi(optarg);
                break;
            case 'o':
                out_file = strdup(optarg);
                break;
            case 'h':
                printf("Usage:\n");
                printf("\t-n <number of nodes>, default 100\n");
                printf("\t-r <Max edge weight>, default 100\n");
                printf("\t-p <probability of edge existing is r/p>, default 150\n");
                printf("\t-o <output file>, default graph.bin\n");
                printf("\t-h <print usage>\n");
                exit(1);
                break;
        }
    }

    //If no output file given use default
    if(out_file == NULL)
        out_file = strdup(filename_default);

    //Generate graph
    storage = create_graph(num_nodes, max_weight, prob, &graph);

    //Will free graph but not storage
    write_graph(out_file, num_nodes, &graph);

    //clean up
    FREE(storage);
    FREE(out_file);

    return(0);
}

/*Creates a num_nodes by num_nodes matrix representing the 
  edges between nodes in a graph
  Inputs:   num_nodes: the number of nodes in the graph
            max_weight: the max edge weight
            prob: max_weight/prob = probability that an edge exists
            A: Address of double poitner for matrix
  Return: Pointer to storage of matrix
 */ 
dtype *create_graph(int num_nodes, int max_weight, int prob, dtype ***A)
{
    dtype **graph;
    dtype *storage;         /*Contains all data for graph*/
    dtype temp; 
    int i,j;

    storage = allocate_graph(num_nodes, &graph);

    for(i = 0; i < num_nodes; i++)
    {
        for(j = 0; j < num_nodes; j++)
        {
            /*For main diagonal of matrix (node's connection to itself)
              edge weight equals 0*/
            if(j == i)
                graph[i][j] = 0; 
            else {
                temp = rand() % (prob + 1);
                if(temp <= max_weight)
                    graph[i][j] = temp;
                //No Connection
                else
                    graph[i][j] = -1;
            }
        }//End for j
    }//End for i

    *A = graph;
    return storage;
}
