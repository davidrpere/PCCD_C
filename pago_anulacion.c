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
#include <sys/shm.h>
#include <stdlib.h>

#define PRIORIDAD 3

/**
 * Variables globales compartidas propias de los procesos pago_anulacion en un nodo.
 */
int *dir_numero_pagos_anulaciones;
key_t key_numero_pagos_anulaciones = 55555;
sem_t semaforo_contador;
/**
 * Fin variables globales propias de los procesos pago_anulacion en un nodo.
 */

/**
 * Variables globales compartidas para las direcciones de los semaforos.
 */
int *dir_semaforo_lectores;
key_t key_semaforo_lectores = 88888881;

int *dir_semaforo_prerreservas;
key_t key_semaforo_prerreservas = 88888882;

int *dir_semaforo_pagos_anulaciones;
key_t key_semaforo_pagos_anulaciones = 88888883;
/**
 * Fin variables globales compartidas para las direcciones de los semaforos.
 */

void wait(sem_t *semaforo);
void signal(sem_t *semaforo);
void *pago_anulacion(void*);

int main(int argc, char *argv[]){

    if(argc != 3){
        printf("Modo de uso: pago_anulacion id_nodo numero_nodos");

        exit(0);
    }

    crear_memoria_compartida();
    sem_init(&semaforo_contador,0,1);

    int id_nodo = atoi[argv[1]];
    int num_nodos = atoi[argv[2]];

    while(1){
        printf("¿Cuantos procesos quieres lanzar? ");
        printf("\nNumero de procesos: ");
        int numero;
        scanf("&i", &numero);
        fflush(stdin);

        pthread_t hilo;
        for(int i = 0; i<numero; i++){

            if (pthread_create(&hilo, NULL, (void *) pago_anulacion, (void *) NULL)) {
                perror("Error creando un pago o anulacion");
                exit(-1);
            }
        }
    }

}

void crear_memoria_compartida(){
    int shmid_numero_pagos_anulaciones,
            shmid_semaforo_lectores,
            shmid_semaforo_prerreservas,
            shmid_semaforo_pagos_anulaciones;

    /**
     * Creacion de la memoria compartida para obtener la direccion de la variable
     * que almacena el numero de procesos pagos y anulaciones.
     */
    shmget(key_numero_pagos_anulaciones, sizeof(int), IPC_CREAT);
    if ((shmid_numero_pagos_anulaciones = shmget(key_numero_pagos_anulaciones, NULL, 0)) == -1) {
        perror("Hubo un error al ejecutar shmget.");
        exit(1);
    }
    if ((dir_numero_pagos_anulaciones = shmat(shmid_numero_pagos_anulaciones, NULL, 0)) == -1) {
        perror("Shmat ha encontrado un error.");
        exit(1);
    }

    /**
     * Creacion de la memoria compartida para los semaforos:
     * -> semaforo_lectores
     * -> semaforo_prerreservas
     * -> semaforo_pagos_anulaciones
     */
    shmget(key_semaforo_lectores,sizeof(int), IPC_CREAT);
    if ((shmid_semaforo_lectores = shmget(key_semaforo_lectores, NULL, 0)) == -1) {
        perror("Hubo un error al ejecutar shmget.");
        exit(1);
    }
    if ((dir_semaforo_lectores = shmat(shmid_semaforo_lectores, NULL, 0)) == -1) {
        perror("Shmat ha encontrado un error.");
        exit(1);
    }

    shmget(key_semaforo_prerreservas,sizeof(int), IPC_CREAT);
    if ((shmid_semaforo_prerreservas = shmget(key_semaforo_prerreservas, NULL, 0)) == -1) {
        perror("Hubo un error al ejecutar shmget.");
        exit(1);
    }
    if ((dir_semaforo_prerreservas = shmat(key_semaforo_prerreservas, NULL, 0)) == -1) {
        perror("Shmat ha encontrado un error.");
        exit(1);
    }

    shmget(key_semaforo_pagos_anulaciones,sizeof(int), IPC_CREAT);
    if ((shmid_semaforo_pagos_anulaciones = shmget(key_semaforo_pagos_anulaciones, NULL, 0)) == -1) {
        perror("Hubo un error al ejecutar shmget.");
        exit(1);
    }
    if ((dir_semaforo_pagos_anulaciones = shmat(key_semaforo_pagos_anulaciones, NULL, 0)) == -1) {
        perror("Shmat ha encontrado un error.");
        exit(1);
    }
}

/*
 * semaforo_lectores: semaforo para detener a los escritores si hay lectores en SC.
 * semaforo_prerreservas: semaforo para detener a los escritores si una prerreserva en una SC
 */
void *pago_anulacion(void *parametro){

    wait(&semaforo_contador);
    if(*dir_numero_pagos_anulaciones == 0){
        wait(dir_semaforo_lectores);
        wait(dir_semaforo_prerreservas);
    }
    *dir_numero_pagos_anulaciones = *dir_numero_pagos_anulaciones + 1;
    signal(&semaforo_contador);

    wait(dir_semaforo_pagos_anulaciones);

    //SC
    //Inicio SC distribuida.
    sleep(10);
    //Fin SC distribuida.
    //local

    wait(&semaforo_contador);
    *dir_numero_pagos_anulaciones = *dir_numero_pagos_anulaciones - 1;
    if(*dir_numero_pagos_anulaciones == 0){
        signal(dir_semaforo_prerreservas);
        signal(dir_semaforo_lectores);
    }
    signal(&semaforo_contador);

    signal(dir_semaforo_pagos_anulaciones);
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