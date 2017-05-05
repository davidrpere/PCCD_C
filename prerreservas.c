#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pccd.c"

#define PRIORIDAD 2

void *prerreserva(void*);

sem_t* semaforo_pagos_anulaciones;
sem_t* semaforo_prerreservas;
sem_t* semaforo_prioridades;
sem_t semaforo_contador;
int nodo;
int num_nodos;
int numero_prerreservas;

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Modo de uso: ./prerreservas 'id_nodo' 'numero_nodos'\n");
        exit(0);
    }
    nodo = atoi(argv[1]);
    num_nodos = atoi(argv[2]);

    inicializar_semaforo(&semaforo_contador, 1);
    numero_prerreservas= 0;

    key_t clave_pagos_anulaciones, clave_prerreservas, clave_prioridades;
    int mem_comp_pagos_anulaciones, mem_comp_prerreservas, mem_comp_prioridades;

    clave_pagos_anulaciones = generar_clave("pagos_anulaciones.c", nodo);
    clave_prerreservas = generar_clave("prerreservas.c", nodo);
    clave_prioridades = generar_clave("inicializacion.c", nodo);

    mem_comp_pagos_anulaciones = obtener_memoria_compartida(clave_pagos_anulaciones, sizeof(sem_t), IPC_EXCL);
    mem_comp_prerreservas = obtener_memoria_compartida(clave_prerreservas, sizeof(sem_t), IPC_EXCL);
    mem_comp_prioridades = obtener_memoria_compartida(clave_prioridades, sizeof(sem_t), IPC_EXCL);

    semaforo_pagos_anulaciones = asignar_memoria_compartida(mem_comp_pagos_anulaciones);
    semaforo_prerreservas = asignar_memoria_compartida(mem_comp_prerreservas);
    semaforo_prioridades = asignar_memoria_compartida(mem_comp_prioridades);

    while(1){
        printf("¿Cuantos procesos de prerreserva quieres lanzar? (Pulse 0 para salir) ");
        int numero;
        scanf(" %i", &numero);
        fflush(stdin);
        if(numero == 0) exit(0);
        int i=0;
        pthread_t hilo;
        for(; i<numero; i++){
            crear_hilo(&hilo, prerreserva, NULL);
        }
    }

}


void *prerreserva(void *parametro){

    wait(semaforo_prerreservas);

    wait(semaforo_prioridades);

    seccion_critica("Prerreserva ha entrado en la SC");
    sleep(1);
    seccion_critica("Prerreserva ha salido de la SC");

    signal(semaforo_prioridades);

    signal(semaforo_prerreservas);

    pthread_exit((void*)NULL);
}
