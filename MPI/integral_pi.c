//
//  integral_pi.c
//  MPI
//
//  Created by sueric on 03/04/14.
//  Copyright (c) 2014 SuEric. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include <mpi.h>

double PI = 3.141592653589793238462643; // VALOR MUY BUENO DE PI

double f(double a) { return (4.0 / (1.0 + a*a ));}

int main(int argc, char* argv[]){
	int n, myid, numprocs;
	const double PI = 3.141592653589793238462643;
	double mypi, pi, h, sum, x, startwtime, endwtime;
    
    /* all MPI programs start with MPI_Init */
	MPI_Init(&argc, &argv);
    
    /* Comm_size tells us how many processes there are  */
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	
    /* Comm_rank finds the rank of the process */
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    
	if(myid == 0){
		printf("Ingrese el numero de intervalos\n");
		scanf("%d", &n);
		startwtime = MPI_Wtime();
	}
    
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
	h = 1.0 / (double)n;
	sum = 0.0;
    
	for(int i = myid+1; i<=n; i+=numprocs){
		x = h * ((double)i - 0.5);
		sum += f(x);
	}
    
	mypi = h*sum;
	
    MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
	if(myid == 0){
		endwtime = MPI_Wtime();
		printf("pi es aproximadamente %1.20f con un error de %1.20f \n", pi, fabs(pi-PI));
		printf("El tiempo para el calculo fue %f \n", endwtime - startwtime);
	}
	
    
	MPI_Finalize();
}

