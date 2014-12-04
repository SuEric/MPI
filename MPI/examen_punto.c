//
//  examen_punto.c
//  MPI
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
    int arreglo[size], suma = 0, suma_procesos = 0, first, chunksize;
    MPI_Status status;

    /* All MPI programs start with MPI_Init */
    MPI_Init(&argc, &argv);
    
    /* Comm_size tells us how many processes there are  */
    MPI_Comm_size(MPI_COMM_WORLD, &nps);
    
    /* Comm_rank finds the rank of the process */
    MPI_Comm_rank(MPI_COMM_WORLD, &miid);
    
    chunksize = size / nps;
    
    srand((unsigned int)time(NULL));
    
    if (miid == 0) {

        for (int i = 0; i < size; i++) {
            arreglo[i] = aleatorio(50);
        }
        
        // Envío de partes a todos
        for (int i = 1; i < nps; i++) {
            first = i * chunksize;
            MPI_Send(&arreglo[first], chunksize, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        
        suma = 0;
        first = miid * chunksize;
        chunksize = size - 1;
        // Efectuo mi parte
        for (int i = first; i < chunksize; i++) suma += arreglo[i];
        
        // Recibo todas las sumas de los demas y calculo suma total
        for (int i = 0; i < nps - 1; i++) {
            MPI_Recv(&suma_procesos, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            suma += suma_procesos;
        }
        
        // Calculo e imprimo el promedio
        float promedio = suma / size;
        printf("El promedio es: %.2f\n", promedio);
    }
    
    else {
        // Recibo mi parte del maestro
        MPI_Recv(arreglo, chunksize, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        
        // Hago mi suma de mi parte
        for (int i = 0; i < chunksize; i++) suma_procesos += arreglo[i];
        
        // Envio al maestro
        MPI_Send(&suma_procesos, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    
    /* MPI Programs end with MPI Finalize; this is a weak synchronization point */
    MPI_Finalize();
}