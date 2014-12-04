//
//  factorial.c
//  MPI
//
//  Created by sueric on 03/04/14.
//  Copyright (c) 2014 SuEric. All rights reserved.
//

#include <stdio.h>
#include <mpi.h>

int main(int argc, char ** argv)
{
    int miid, nps, n, producto;
    
    /* all MPI programs start with MPI_Init */
    MPI_Init(&argc, &argv);
    
    /* Comm_size tells us how many processes there are  */
    MPI_Comm_size(MPI_COMM_WORLD, &nps);
    
    /* Comm_rank finds the rank of the process */
    MPI_Comm_rank(MPI_COMM_WORLD, &miid);
 

    n = miid +1;
    
    MPI_Reduce(&n, &producto, 1, MPI_INT, MPI_PROD, 0, MPI_COMM_WORLD);
    
    if (miid == 0) printf("El resultado es: %d\n", producto);
    
    /* MPI Programs end with MPI Finalize; this is a weak synchronization point */
    MPI_Finalize();
}