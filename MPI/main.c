//
//  main.c
//  MPI
//
//  Created by sueric on 25/03/14.
//  Copyright (c) 2014 SuEric. All rights reserved.
//

#include <stdio.h>
#include <mpi.h>

int main(int argc, char ** argv)
{
    MPI_Status st;
    int miid, nps, num;
    
    /* all MPI programs start with MPI_Init */
    MPI_Init(&argc, &argv);
    
    /* Comm_size tells us how many processes there are  */
    MPI_Comm_size(MPI_COMM_WORLD, &nps);
    
    /* Comm_rank finds the rank of the process */
    MPI_Comm_rank(MPI_COMM_WORLD, &miid);
    
    /* Print out a message */
    //printf("Soy el procesador %d de %d\n", miid, nps);
    
    if (miid == 0) {
        printf("Dame un entero: \n");
        scanf("%d", &num);
        num++;
        
        MPI_Send(&num, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&num, 1, MPI_INT, nps-1, 0, MPI_COMM_WORLD, &st);
        
        printf("El resultado es: %d", num);
    }
    else {
        MPI_Recv(&num, 1, MPI_INT, miid-1, 0, MPI_COMM_WORLD, &st);
        num++;
        
        MPI_Send(&num, 1, MPI_INT, (miid+1)%nps, 0, MPI_COMM_WORLD);
    }
    
    /* MPI Programs end with MPI Finalize; this is a weak synchronization point */
    MPI_Finalize();
}