//
//  diccionario.c
//  MPI
//
//  Created by sueric on 26/04/14.
//  Copyright (c) 2014 SuEric. All rights reserved.
//

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>

struct Diccionario {
    char palabra[100];
    char binary[11];
};

struct Diccionario diccionario[1024];

MPI_Datatype crearStruct() {
	MPI_Datatype oldtypes[2] = {MPI_CHAR, MPI_CHAR}; // Tipo de datos que tiene el struct
	int blockcounts[2] = {100, 11}; // Tamaño de bloques
	MPI_Aint offsets[2] = {0, sizeof(char) * 100}; // Inicio de memoria de cada bloque
    
    // Se crea la estructura en voctype
    MPI_Datatype voctype; // Tipo de dato nuevo
	MPI_Type_struct(1, blockcounts, offsets, oldtypes, &voctype);
    
    // Commit del struct
    MPI_Type_commit(&voctype);
    
    return voctype;
}

void guardarDatos(char nombre_archivo[], int numero_palabras, char vector_resultado[]) {
    FILE *archivo;
    
    if ((archivo = fopen("resultados.txt", "a"))) {
        fprintf(archivo, "Archivo: %s. ", nombre_archivo);
        fprintf(archivo, "Palabras: %d. ", numero_palabras);
        fprintf(archivo, "Vector: %s\n", vector_resultado);
    }
    else {
        perror("Error escritura resultados");
        exit(1);
    }
    fclose(archivo);
}

int buscar_palabra_repetida(char *palabra, int contador_palabras) {
    for (int i = 0; i < contador_palabras; i++)
        if (strcmp(palabra, diccionario[i].palabra) == 0) return i;
    return -1;
}

int buscar_vec_repetido(char *binario, int contador_palabras) {
    for (int i = 0; i < contador_palabras; i++)
        if (strcmp(binario, diccionario[i].binary) == 0) return i;
    return -1;
}

void makeBinary(struct Diccionario diccionary[], int contador_palabra) {
    int vec_repetido = 0;
    
    char aux_vector[11];
    do {
        for (int i = 0; i < 10; i++) {
            int n = rand() % 2;
            if(n == 1) aux_vector[i] = '1';
            else aux_vector[i] = '0';
        }
        vec_repetido = buscar_vec_repetido(aux_vector, contador_palabra);
    } while(vec_repetido != -1);
    
    strcpy(diccionario[contador_palabra].binary, aux_vector);
}

void suma_binaria(char a[], char b[]) {
    int an;
    int bn;
    int C = 0;
    
    for(int i = 19; i >= 0 ; i--) {
		// Se resta 48 para llevarse a la forma int
        an = a[i]-48;
        bn = b[i]-48;
        
        if (an + bn + C == 0) {
            C = 0;
            b[i] = '0';
        }
        else	 if (an+bn+C == 1) {
            C=0;
            b[i] = '1';
        } else if (an+bn+C == 2) {
            C=1;
            b[i] = '0';
        } else if (an+bn+C == 3) {
            C=1;
            b[i] = '1';
        }
    }
}

int main(int argc, char ** argv)
{
    srand((unsigned int)time(NULL));
    MPI_Status status;
    int miid, nps;
    int contador_archivo = 0;
    int contador_palabra = 0;
    int chunksize, first;
    
    FILE *archivo;
    
    /********* Configuracion MPI *********/
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nps);
    MPI_Comm_rank(MPI_COMM_WORLD, &miid);
    /********* Configuracion MPI *********/
    
    DIR *dirp;
    struct dirent *entry;
    if (miid == 0) {
        if( (dirp = opendir("/Users/sueric/Desktop/MPI/archivos")) != NULL) {
            while ((entry = readdir(dirp)) != NULL) {
                
                if (!strcmp (entry->d_name, ".") ||
                    !strcmp (entry->d_name, "..") ||
                    !strcmp (entry->d_name, ".DS_Store")) continue;
                
                if (entry->d_type == DT_REG) {
                    char ruta_absoluta[100] = "/Users/sueric/Desktop/MPI/archivos/";
                    if ( (archivo = fopen(strcat(ruta_absoluta, entry->d_name), "rt")) != NULL) {
                        fseek(archivo, 0, SEEK_END);
                        long fsize = ftell(archivo);
                        fseek(archivo, 0, SEEK_SET);
                        
                        char *string = malloc(fsize + 1);
                        fread(string, fsize, 1, archivo);
                        
                        // Separa palabras por espacios
                        // y las añade al diccionario
                        char *split = strtok(string, " ");
                        while (split != NULL) {
                            int palabra_repetida = 0;
                            
                            for (int i = 0; i < (int)(sizeof(split)/sizeof(char)); i++) split[i] = tolower(split[i]);
                            
                            palabra_repetida = buscar_palabra_repetida(split, contador_palabra);
                            
                            if (palabra_repetida == -1) strcpy(diccionario[contador_palabra].palabra, split);
                            else {
                                split = strtok(NULL, " ");
                                continue;
                            }
                            
                            makeBinary(diccionario, contador_palabra);
                            
                            split = strtok(NULL, " ");
                            contador_palabra++;
                        }
                        
                        printf("\n\n\n");
                        contador_archivo++;
                        fclose(archivo);
                    }
                    else {
                        perror("Error abriendo archivo: ");
                        exit(1);
                    }
                }
            }
            closedir(dirp);
        }
        else {
            perror("Error abriendo directorio: ");
            exit(1);
        }
        
        chunksize = contador_archivo / (nps-1);
        
        printf("Numero de archivos en total: %d\n", contador_archivo);
        printf("Numero de palabras en total: %d\n", contador_palabra);
        
        // Envío de partes a todos
        for (int i = 1; i < nps; i++) {
            first = (i-1) * chunksize;
            MPI_Send(&first, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    
    MPI_Bcast(&contador_palabra, 1, MPI_INT, 0, MPI_COMM_WORLD); //Enviamos el total de palabras a los esclavos
    MPI_Bcast(&contador_archivo, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&chunksize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(diccionario, 1024, crearStruct(), 0, MPI_COMM_WORLD);
    
    if (miid != 0) {
        MPI_Recv(&first, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        
        if( (dirp = opendir("/Users/sueric/Desktop/MPI/archivos")) != NULL) {
            int contador = 0;
            while ((entry = readdir(dirp)) != NULL) {
                if (!strcmp (entry->d_name, ".") ||
                    !strcmp (entry->d_name, "..") ||
                    !strcmp (entry->d_name, ".DS_Store")) continue;
                
                if (contador < first) { contador++; continue; }
                else if (contador > first+chunksize-1) break;
                
                if (entry->d_type == DT_REG) {
                    char ruta_absoluta[100] = "/Users/sueric/Desktop/MPI/archivos/";
                    if ( (archivo = fopen(strcat(ruta_absoluta, entry->d_name), "rt")) != NULL) {
                        fseek(archivo, 0, SEEK_END);
                        long fsize = ftell(archivo);
                        fseek(archivo, 0, SEEK_SET);
                        
                        int contador_palabra_interno = 0;
                        
                        char *string = malloc(fsize + 1);
                        fread(string, fsize, 1, archivo);
                        
                        // Separa palabras encontrandas
                        // en los archivos por espacios
                        char arregloPalabras[contador_palabra][100];
                        char *split = strtok(string, " ");
                        while (split != NULL) {

                            for (int i = 0; i < (int)(sizeof(split)/sizeof(char)); i++) split[i] = tolower(split[i]);
                            
                            strcpy(arregloPalabras[contador_palabra_interno], split);
                            
                            // Siguiente palabra
                            split = strtok(NULL, " ");
                            contador_palabra_interno++;
                        }
                        
                        char vector1[21] = "0000000000"; // 10 elementos
                        char vector2[21] = "0000000000"; // 10 elementos
                        char vectorResultado[21];
                        
                        int indice = buscar_palabra_repetida(arregloPalabras[0], contador_palabra);
                        int indice2 = buscar_palabra_repetida(arregloPalabras[1], contador_palabra);
                        
                        strcat(vector1, diccionario[indice].binary);
                        strcat(vector2, diccionario[indice2].binary);
                        
                        strcpy(vectorResultado, vector2);
                        suma_binaria(vector1, vectorResultado);
                        
                        for (int i = 2; i < contador_palabra_interno-1; i++) {
                            int indice = buscar_palabra_repetida(arregloPalabras[i], contador_palabra);
                            suma_binaria(diccionario[indice].binary, vectorResultado);
                        }
                        
                        printf("Resultado archivo[%d] by %d: %s\n", contador+1, miid, vectorResultado);
                        
                        contador++;
                        
                        guardarDatos(entry->d_name, contador_palabra_interno, vectorResultado);
                        
                        fclose(archivo);
                    }
                    else {
                        perror("Error abriendo archivo: ");
                        exit(1);
                    }
                }
            }
            closedir(dirp);
        }
        else {
            perror("Error abriendo directorio: ");
            exit(1);
        }
    }
    
    /* MPI Programs end with MPI Finalize; this is a weak synchronization point */
    MPI_Finalize();
}