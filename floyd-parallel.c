/*  Name: Nick Dreitzler
    Username: ndreitz
    CUID: C15564443
    Class: ECE 4730
    Professor: Walt Ligon
    Semester: Fall2020
    Filename: floyd-parallel.c

    Purpose: Reads a graph file, computes Floyd's shortest 
        pairs, writes resulting shortest path graph, to be
        stored in the same format.

    usage: ./floyd-parallel <input_file> <output_file>
    
    Output: Binary file with n by n matrix representing 
        Floyd's shortest pairs.
 */

#include "graph.h"
#include "graph_checkerboard_io.h"

#define NDIMS 2

void find_shortest_paths(int num_nodes, dtype ***A, MPI_Comm Grid_comm);

int main(int argc, char **argv)
{
    MPI_Comm MPI_grid;  /*MPI communicator with cartesian topology*/
    dtype *storage;     /*Storage for data in graph*/
    dtype **graph;      /*2d matrix containing graph adjacency data*/
    double TimeNOIO;    /*Execution time without IO*/
    double MaxTimeNOIO; /*Max execution time without IO*/
    double TimeIO;      /*Execution time with IO*/
    double MaxTimeIO;   /*Max execution time with IO*/
    int size;           /*Number of MPI tasks*/
    int rank;           /*rank of MPI task*/
    int num_nodes;      /*Number of nodes in graph*/
    int dims[] = {0,0}; /*Dimensions of MPI_grid*/
    int periods[NDIMS]; /*Periods of MPI_grid*/

    //MPI setup
    MPI_Init(&argc, &argv);
    MPI_Barrier(MPI_COMM_WORLD);
    TimeIO = -MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Need input and output files
    if(argc != 3)
    {
        printf("Usage:\n\t./floyd-parrallel <input file> <output file>\n");
        MPI_Finalize();
        exit(-1);
    }

    //Create cartesian topology and read adjacency matrix into tasks*/
    MPI_Dims_create( size , NDIMS , dims);
    MPI_Cart_create( MPI_COMM_WORLD , NDIMS , dims , periods , 0 , &MPI_grid);
    read_checkerboard_graph(argv[1], (void *)&graph, (void *)&storage, MPI_INT, &num_nodes, MPI_grid);

    //Find shortest paths between nodes
    TimeNOIO = -MPI_Wtime();
    find_shortest_paths(num_nodes, &graph, MPI_grid);
    TimeNOIO += MPI_Wtime();

    //Write resulting shortest paths matrix to file
    write_checkerboard_graph(argv[2], (void *)&graph, (void *)&storage, MPI_INT, num_nodes, MPI_grid);
    TimeIO += MPI_Wtime();

    //Determine max execution times and store result with task 0
    MPI_Reduce( (void *)(&TimeIO), (void *)(&MaxTimeIO), 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce( (void *)(&TimeNOIO), (void *)(&MaxTimeNOIO), 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    //Print results
    if(rank == 0)
    {
        printf("%5s\t%5s\t%11s\t%13s\n", "size", "n", "TimeIO (ms)", "TimeNOIO (ms)");
        printf("%5d\t%5d\t%11f\t%13f\n", size, num_nodes, TimeIO*1000, TimeNOIO*1000);
    }

    //Clean up
    MPI_Comm_free( &MPI_grid);
    MPI_Finalize();
    return(0);
}

void find_shortest_paths(
    int num_nodes,      /*IN      - number of nodes in graph*/
    dtype ***A,         /*IN/OUT  - Pointer to graph data*/
    MPI_Comm Grid_comm) /*IN      - MPI communicator with cartesian topology*/
{
    MPI_Comm Row_comm;      /*MPI communicator for the rows in Grid_comm*/
    MPI_Comm Col_comm;      /*MPI communicator for the columns in Grid_comm*/
    dtype **graph = *A;     /*graph data*/
    dtype *tmp_col;         /*temp column data*/
    dtype *tmp_row;         /*temp row data*/
    int dims[NDIMS];        /*Holds dimensions of Grid_comm*/
    int periods[NDIMS];     /*Holds periods of Grid_comm*/
    int grid_coords[NDIMS]; /*Holds coordnates of node*/
    int num_cols;           /*Block size of current node in i direction*/
    int num_rows;           /*Block size of current node in j direction*/
    int root;               /*root node*/
    int ranks[3];           /*{rank in MPI_COMM_WORLD, rank in Row_comm, rank in Col_comm}*/
    int rdims[] = {1, 0};   /*Used to create Row_comm*/
    int cdims[] = {0, 1};   /*Used to create Col_comm*/
    int i,j,k;
    int offset;

    MPI_Comm_rank(MPI_COMM_WORLD, &(ranks[0]));
    MPI_Cart_get( Grid_comm , NDIMS , dims , periods , grid_coords);
    
    //Create Row_comm and Col_comm
    //These are the rows and columns of Grid_comm
    MPI_Cart_sub( Grid_comm , rdims , &Row_comm);
    MPI_Cart_sub( Grid_comm , cdims , &Col_comm);

    //Determine Row_comm and Col_comm ranks
    MPI_Comm_rank(Row_comm, &(ranks[1]));
    MPI_Comm_rank(Col_comm, &(ranks[2]));

    //Determine number of rows/cols that the the current task has.
    //Note that all tasks in the same Row_comm will have the same
    //number of rows, and vice versa
    num_rows = BLOCK_SIZE(grid_coords[0], dims[0], num_nodes);
    num_cols = BLOCK_SIZE(grid_coords[1], dims[1], num_nodes);

    tmp_col = (dtype *)malloc(sizeof(dtype)*num_rows);
    tmp_row = (dtype *)malloc(sizeof(dtype)*num_cols);
    for(k = 0; k < num_nodes; k++)
    {
        //Find root for each row/column
        root = BLOCK_OWNER(k, dims[0], num_nodes);

        //If root equals Row_comm rank.
        //This is the task that has the needed row for the kth iteration
        if(root == ranks[1])
        {      
            offset = k - BLOCK_LOW(root, dims[0], num_nodes);
            for(i = 0; i < num_cols; i++)
            {
                tmp_row[i] = graph[offset][i];
            }    
        }
        //Bcast tmp_row to all tasks in same row
        MPI_Bcast( tmp_row, num_cols, MPI_INT, root, Row_comm);

        //If root equals Col_comm rank.
        //This is the task that has the needed col for the kth iteration.
        if(root == ranks[2])
        {
            offset = k - BLOCK_LOW(root, dims[1], num_nodes);
            for(i = 0; i < num_rows; i++)
            {
                tmp_col[i] = graph[i][offset];   
            }   
        }
        //Bcast tmp_col to all tasks in same column
        MPI_Bcast( tmp_col, num_rows, MPI_INT, root, Col_comm);

        //Floyd's algorithm
        for(i = 0; i < num_rows; i++)
             for(j = 0; j < num_cols; j++)
                 graph[i][j] = find_min(graph[i][j], tmp_col[i], tmp_row[j]);
    }//End for k
    
    //Clean up
    FREE(tmp_col);
    FREE(tmp_row);
    MPI_Comm_free( &Row_comm);
    MPI_Comm_free( &Col_comm); 
}

