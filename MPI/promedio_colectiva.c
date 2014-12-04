//
//  promedio_colectiva.c
//  MPI - Punto a punto
//
//  Created by sueric on 03/04/14.
//  Copyright (c) 2014 SuEric. All rights reserved.
//

#include <stdio.h>
#include "unistd.h"
#include <mpi.h>

int main(int argc, char ** argv)
{
    int miid, nps, num = 9999;
    
    /* All MPI programs start with MPI_Init */
    MPI_Init(&argc, &argv);
    
    /* Comm_size tells us how many processes there are  */
    MPI_Comm_size(MPI_COMM_WORLD, &nps);
    
    /* Comm_rank finds the rank of the process */
    MPI_Comm_rank(MPI_COMM_WORLD, &miid);
    
    if (miid == 0) {
        printf("Ingrese número: \n");
        scanf("%d", &num);
    }
    
    sleep(3);
    num++;
    int suma = 0;

    MPI_Reduce(&num, &suma, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    printf("Suma vale: %d\n", suma);
    
    if (miid == 0) {
        float promedio  = suma/nps;
        printf("El promedio del número %d es: %f\n", num, promedio);
    }
    
    /* MPI Programs end with MPI Finalize; this is a weak synchronization point */
    MPI_Finalize();
}