//
//  examen_colectiva2.c
//  MPI - colectiva2
//
//  Created by sueric on 10/04/14.
//  Copyright (c) 2014 SuEric. All rights reserved.
//

#include <stdio.h>
#include "unistd.h"
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#define size 10000

int aleatorio(int limit) {
    /* return a random number between 0 and limit inclusive.
     */
    
    int divisor = RAND_MAX/(limit+1);
    int retval;
    
    do {
        retval = rand() / divisor;
    } while (retval > limit);
    
    return retval;
}

int main(int argc, char ** argv)
{
    int miid, nps;
    int arreglo[size], arreglo2[size], suma = 0, suma_procesos = 0, chunksize;

    /* All MPI programs start with MPI_Init */
    MPI_Init(&argc, &argv);
    
    /* Comm_size tells us how many processes there are  */
    MPI_Comm_size(MPI_COMM_WORLD, &nps);
    
    /* Comm_rank finds the rank of the process */
    MPI_Comm_rank(MPI_COMM_WORLD, &miid);
    
    chunksize = size / nps;
    
    if (chunksize * nps != size) {
        if (miid == 0)
            printf("size %d debe ser divibile por nps (%d)\n", size, nps);
        MPI_Finalize();
        exit(1);
    }
    
    // Maestro llena vector
    if (miid == 0) {
        srand((unsigned int)time(NULL));
        for (int i = 0; i < size; i++) arreglo[i] = aleatorio(300);
    }
    
    //sleep((unsigned int).3);
    
    // Distribuye el vector
    MPI_Scatter(arreglo, chunksize, MPI_INT, arreglo2, chunksize, MPI_INT, 0, MPI_COMM_WORLD);
    
    suma_procesos = 0;
    
    // Se calcula la suma de mi parte
    for (int i = 0; i < chunksize; i++) suma_procesos += arreglo2[i];
    
    // Reduccion de todas las sumas y obtencion del total
    MPI_Reduce(&suma_procesos, &suma, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (miid == 0) {
        float promedio = suma / size;
        printf("El promedio es: %.2f\n", promedio);
    }
    
    MPI_Finalize();
}