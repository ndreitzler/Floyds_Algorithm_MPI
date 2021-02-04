/*  Name: Nick Dreitzler
    Username: ndreitz
    CUID: C15564443
    Class: ECE 4730
    Professor: Walt Ligon
    Semester: Fall2020
    Filename: floyd-serial.c

    Purpose: Reads a graph file, computes Floyd's shortest 
        pairs, writes resulting shortest path graph, to be
        stored in the same format.

    usage: ./floyd-serial <input_file> <output_file>
    
    Output: Binary file with n by n matrix representing 
        Floyd's shortest pairs.
 */

#include "graph.h"

dtype find_min(dtype ij, dtype ik, dtype kj);

int main(int argc, char **argv)
{
    dtype *storage;         /*Storage for data in graph*/
    dtype **graph;          /*2d matrix containing graph adjacency data*/
    int num_nodes;
    clock_t exe_time_IO;
    clock_t exe_time_NOIO;
    int i,j,k;

    if(argc != 3)
    {
        printf("Usage:\n\t./floyd-serial <input file> <output file>\n");
        exit(1);
    }

    exe_time_IO = -clock();
    storage = read_graph(argv[1], &num_nodes, &graph);
    exe_time_NOIO = -clock();

    //Floyd's Algorithm
    for(k = 0; k < num_nodes; k++)
    {
        for(i = 0; i < num_nodes; i++)
        {
            for(j = 0; j < num_nodes; j++)
            {
                graph[i][j] = find_min(graph[i][j], graph[i][k], graph[k][j]);
            }
        }
    }
    exe_time_NOIO += clock();
    write_graph(argv[2], num_nodes, &graph);
    exe_time_IO += clock();

    printf("%7s\t%5s\t%11s\t%13s\n", "Serial", "n", "TimeIO (ms)", "TimeNOIO (ms)");
    printf("%7d\t%5d\t%11f\t%13f\n", 1, num_nodes, ((double)exe_time_IO*1000)/CLOCKS_PER_SEC, ((double)exe_time_NOIO*1000)/CLOCKS_PER_SEC);

    FREE(storage);
}

