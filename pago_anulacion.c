/*
 *
 * Proceso escritor de mayor prioridad
 *
 * Debe recibir por parámetros el id del nodo y el numero de nodos del sistema
 *
 */
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define PRIORIDAD 3

void wait(sem_t *semaforo);
void signal(sem_t *semaforo);
void *pago_anulacion(void*);

int main(int argc, char *argv[]){
    if(argc != 4){
        printf("Modo de uso: escritores id_nodo numero_nodos");
        exit(0);
    }
    int id_nodo = atoi[argv[1]];
    int num_nodos = atoi[argv[2]];

    while(1){
        printf("¿Cuantos procesos quieres lanzar? ");
        int numero;
        scanf(" &i", &numero);
        fflush(stdin);
        int i=0;
        pthread_t hilo;
        for(; i<numero; i++){
            if (pthread_create(&hilo, NULL, (void *) pago_anulacion, (void *) NULL) {
                perror("Error creando un pago o anulacion");
                exit(-1);
            }
        }
    }

}


void *pago_anulacion(void *parametro){

    wait(&semaforo_contador);
    if(numero_pagos_anulaciones == 0){
        wait(&semaforo_lectores);
        wait(&semaforo_prerreservas);
    }
    numero_pagos_anulaciones++;
    signal(&semaforo_contador);

    wait(&semaforo_pagos_anulaciones);
    //SC
    sleep(10);
    //local

    wait(&semaforo_contador);
    numero_pagos_anulaciones--;
    if(numero_pagos_anulaciones == 0){
        signal(&semaforo_prerreservas);
        signal(&semaforo_lectores);
    }
    signal(&semaforo_contador);

    signal(&semaforo_pagos_anulaciones);
}

void wait(sem_t *semaforo){
    if(sem_wait(semaforo) != 0){
        perror("Error al hacer wait");
        exit(-1);
    }
}

void signal(sem_t *semaforo){
    if(sem_post(semaforo) != 0){
        perror("Error al hacer signal");
        exit(-1);
    }
}