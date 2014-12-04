//
//  promedio.c
//  MPI
//
//  Created by sueric on 03/04/14.
//  Copyright (c) 2014 SuEric. All rights reserved.
//

#include <stdio.h>
#include <mpi.h>

int main(int argc, char ** argv)
{
    MPI_Status status;
    int miid, nps, num;
    
    /* all MPI programs start with MPI_Init */
    MPI_Init(&argc, &argv);
    
    /* Comm_size tells us how many processes there are  */
    MPI_Comm_size(MPI_COMM_WORLD, &nps);
    
    /* Comm_rank finds the rank of the process */
    MPI_Comm_rank(MPI_COMM_WORLD, &miid);
    
    if (miid == 0) {
        printf("Ingrese número: \n");
        scanf("%d", &num);
        
        MPI_Send(&num, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); // Envía al segundo proceso
        MPI_Recv(&num, 1, MPI_INT, nps-1, 0, MPI_COMM_WORLD, &status); // Recibe del último proceso
        
        float promedio  = num/nps;
        printf("El promedio del número %d es: %f\n", num, promedio);
    }
    else {
        MPI_Recv(&num, 1, MPI_INT, miid-1, 0, MPI_COMM_WORLD, &status);
        
        num++;
        
        MPI_Send(&num, 1, MPI_INT, (miid+1) % nps, 0, MPI_COMM_WORLD);
    }
    
    /* MPI Programs end with MPI Finalize; this is a weak synchronization point */
    MPI_Finalize();
}