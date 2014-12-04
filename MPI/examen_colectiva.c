//
//  examen_colectiva.c
//  MPI - colectiva
//
//  Created by sueric on 10/04/14.
//  Copyright (c) 2014 SuEric. All rights reserved.
//

#include <stdio.h>
#include "unistd.h"
#include <mpi.h>
#include <time.h>
#include <stdlib.h>

#define size 10

int miid, nps;
int arreglo[size], suma = 0, suma_procesos = 0, first, last, chunksize;

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
    /* All MPI programs start with MPI_Init */
    MPI_Init(&argc, &argv);
    
    /* Comm_size tells us how many processes there are  */
    MPI_Comm_size(MPI_COMM_WORLD, &nps);
    
    /* Comm_rank finds the rank of the process */
    MPI_Comm_rank(MPI_COMM_WORLD, &miid);
    
    srand((unsigned int)time(NULL));
    
    // Maestro llena vector
    if (miid == 0) {
        for (int i = 0; i < size; i++) arreglo[i] = aleatorio(50);
    }
    
    // Difunde el vector
    MPI_Bcast(arreglo, size, MPI_INT, 0, MPI_COMM_WORLD);
    
    chunksize = size / nps;
    
    if (miid == nps - 1) {
        first = (nps-1) * chunksize;
        last = size - 1;
    }
    else {
        first = miid * chunksize;
        last = (miid + 1)*chunksize - 1;
    }
    
    // Se calcula la suma de mi parte
    for (int i = first; i <= last; i++) suma_procesos += arreglo[i];

    // Reduccion de las sumas y calculo suma total
    MPI_Reduce(&suma_procesos, &suma, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (miid == 0) {
        printf("Suma vale: %d", suma);
        float promedio = suma/size;
        printf("El promedio es: %.2f\n", promedio);
    }
    
    MPI_Finalize();
}