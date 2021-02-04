/*  Name: Nick Dreitzler
    Username: ndreitz
    CUID: C15564443
    Class: ECE 4730
    Professor: Walt Ligon
    Semester: Fall2020
    Filename: print-graph.c

    Purpose: To read a binary file of a n by n matrix 
        representing a graph of n nodes.
        Data are integers and the number n is stored as 
        the first word of the file.

    Usage: ./print-graph <input file>
    
    Output: Print out of given graph
 */

#include "graph.h"

int main(int argc, char **argv)
{
    dtype *storage;
    dtype **graph;
    int num_nodes;

    if(argc != 2)
    {
        printf("Usage:\n\t./print-file <input file>\n");
        exit(1);
    }

    //Read adjacency matrix from argv[1]
    storage = read_graph(argv[1], &num_nodes, &graph);

    print_graph(num_nodes, graph);

    //Clean up
    FREE(storage);
    FREE(graph);
    return(0);
}