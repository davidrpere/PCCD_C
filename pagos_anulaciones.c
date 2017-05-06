#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "pccd.c"

#define PRIORIDAD 3

void *pago_anulacion(void*);

sem_t semaforo_contador;
sem_t* semaforo_pagos_anulaciones;
sem_t* semaforo_prerreservas;
sem_t* semaforo_prioridades;
sem_t* semaforo_lectores;
sem_t* semaforo_escritores;
int nodo;
int num_nodos;
int numero_pagos_anulaciones;
int *numero_escritores;
int pid = 0;


int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Modo de uso: ./pagos_anulaciones 'id_nodo' 'numero_nodos' 'pid_registro'\n");
        exit(0);
    }
    nodo = atoi(argv[1]);
    num_nodos = atoi(argv[2]);
    pid = atoi(argv[3]);

    inicializar_semaforo(&semaforo_contador, 1);
    numero_pagos_anulaciones = 0;

    key_t clave_pagos_anulaciones, clave_prerreservas, clave_prioridades, clave_lectores, clave_escritores_semaforo, clave_escritores_contador;
    int mem_comp_pagos_anulaciones, mem_comp_prerreservas, mem_comp_prioridades, mem_comp_lectores, mem_comp_escritores_semaforo, mem_comp_escritores_contador;

    clave_pagos_anulaciones = generar_clave("pagos_anulaciones.c", nodo);
    clave_prerreservas = generar_clave("prerreservas.c", nodo);
    clave_prioridades = generar_clave("inicializacion.c", nodo);
    clave_lectores = generar_clave("lectores.c", nodo);
    clave_escritores_contador = generar_clave("pccd.c", nodo);
    clave_escritores_semaforo = generar_clave("receptor.c", nodo);

    mem_comp_pagos_anulaciones = obtener_memoria_compartida(clave_pagos_anulaciones, sizeof(sem_t), IPC_EXCL);
    mem_comp_prerreservas = obtener_memoria_compartida(clave_prerreservas, sizeof(sem_t), IPC_EXCL);
    mem_comp_prioridades = obtener_memoria_compartida(clave_prioridades, sizeof(sem_t), IPC_EXCL);
    mem_comp_lectores = obtener_memoria_compartida(clave_lectores, sizeof(sem_t), IPC_EXCL);
    mem_comp_escritores_contador = obtener_memoria_compartida(clave_escritores_contador, sizeof(int), IPC_EXCL);
    mem_comp_escritores_semaforo = obtener_memoria_compartida(clave_escritores_semaforo, sizeof(sem_t), IPC_EXCL);

    semaforo_pagos_anulaciones = asignar_memoria_compartida(mem_comp_pagos_anulaciones);
    semaforo_prerreservas = asignar_memoria_compartida(mem_comp_prerreservas);
    semaforo_prioridades = asignar_memoria_compartida(mem_comp_prioridades);
    semaforo_lectores = asignar_memoria_compartida(mem_comp_lectores);
    numero_escritores = asignar_memoria_compartida(mem_comp_escritores_contador);
    semaforo_escritores = asignar_memoria_compartida(mem_comp_escritores_semaforo);

    while(1){
        printf("¿Cuantos procesos de pago o anulacion quieres lanzar? (Pulse 0 para salir) ");
        int numero;
        scanf(" %i", &numero);
        fflush(stdin);
        if(numero == 0) exit(0);
        hora_actual(stdout);
        int i=0;
        pthread_t hilo;
        for(; i<numero; i++){
            crear_hilo(&hilo, pago_anulacion, NULL);
        }
    }

}

void *pago_anulacion(void *parametro){

    wait(semaforo_escritores);
    if(*numero_escritores == 0){
        wait(semaforo_lectores);
    }
    *numero_escritores = *numero_escritores +1;
    post(semaforo_escritores);

    wait(&semaforo_contador);
    if(numero_pagos_anulaciones == 0){
        wait(semaforo_prioridades);
    }
    numero_pagos_anulaciones++;
    post(&semaforo_contador);

    wait(semaforo_pagos_anulaciones);

    seccion_critica("Pago o anulacion ha entrado en la SC");

    sistema_distribuido();
    sleep(1);

    wait(&semaforo_contador);
    numero_pagos_anulaciones--;
    if(numero_pagos_anulaciones == 0){
        post(semaforo_prioridades);
    }
    post(&semaforo_contador);

    wait(semaforo_escritores);
    *numero_escritores = *numero_escritores -1;
    if(*numero_escritores == 0){
        post(semaforo_lectores);
    }
    post(semaforo_escritores);

    seccion_critica("Pago o anulacion ha salido de la SC");
    post(semaforo_pagos_anulaciones);

    pthread_exit((void*)NULL);
}

void sistema_distribuido(){

}