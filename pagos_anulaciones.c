#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pccd.c"

void *pago_anulacion(void*);
void sistema_distribuido(void);

sem_t semaforo_contador_local;
sem_t* semaforo_pagos_anulaciones;
sem_t* semaforo_prerreservas;
sem_t* semaforo_prioridades;
sem_t* semaforo_lectores;
sem_t* semaforo_escritores;
sem_t* semaforo_atomico;
int nodo;
int num_nodos;
int numero_pagos_anulaciones;
int *numero_escritores;
pid_t pid_sc_local, pid_sc_distribuida;

int main(int argc, char *argv[]){
    if(argc != 5){
        printf("Modo de uso: ./pagos_anulaciones 'id_nodo' 'numero_nodos' 'pid_sc_local' 'pid_sc_distribuido'\n");
        exit(0);
    }
    nodo = atoi(argv[1]);
    num_nodos = atoi(argv[2]);
    pid_sc_local = atoi(argv[3]);
    pid_sc_distribuida = atoi(argv[4]);

    inicializar_semaforo(&semaforo_contador_local, 1);
    numero_pagos_anulaciones = 0;

    key_t clave_pagos_anulaciones, clave_prerreservas, clave_prioridades, clave_lectores, clave_escritores_semaforo, clave_escritores_contador;
    int mem_comp_pagos_anulaciones, mem_comp_prerreservas, mem_comp_prioridades, mem_comp_lectores, mem_comp_escritores_semaforo, mem_comp_escritores_contador;

    clave_pagos_anulaciones = generar_clave("/home/juan/PCCD_C/pagos_anulaciones.c", nodo);
    clave_prerreservas = generar_clave("/home/juan/PCCD_C/prerreservas.c", nodo);
    clave_prioridades = generar_clave("/home/juan/PCCD_C/inicializacion.c", nodo);
    clave_lectores = generar_clave("/home/juan/PCCD_C/lectores.c", nodo);
    clave_escritores_contador = generar_clave("/home/juan/PCCD_C/pccd.c", nodo);
    clave_escritores_semaforo = generar_clave("/home/juan/PCCD_C/receptor.c", nodo);

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

    key_t clave_atomico = generar_clave("/home/juan/PCCD_C/inicializacion.c", -1*nodo);
    int mem_comp_atomico = obtener_memoria_compartida(clave_atomico, sizeof(sem_t), IPC_EXCL);
    semaforo_atomico = asignar_memoria_compartida(mem_comp_atomico);

    while(1){
        printf("¿Cuantos procesos de pago o anulacion quieres lanzar? (Pulse 0 para salir) ");
        int numero;
        scanf(" %i", &numero);
        fflush(stdin);
        if(numero == 0) exit(0);
        printf("%s\n", hora_actual());
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

    wait(&semaforo_contador_local);
    if(numero_pagos_anulaciones == 0){
        wait(semaforo_prioridades);
    }
    numero_pagos_anulaciones++;
    post(&semaforo_contador_local);

    wait(semaforo_pagos_anulaciones);

    seccion_critica_local("Pago o anulacion ha entrado en la SC", nodo, pid_sc_local, PAGO_ANULACION, 1);
    sistema_distribuido();

    wait(&semaforo_contador_local);
    numero_pagos_anulaciones--;
    if(numero_pagos_anulaciones == 0){
        post(semaforo_prioridades);
    }
    post(&semaforo_contador_local);

    wait(semaforo_escritores);
    *numero_escritores = *numero_escritores -1;
    if(*numero_escritores == 0){
        post(semaforo_lectores);
    }
    post(semaforo_escritores);

    seccion_critica_local("Pago o anulacion ha salido de la SC", nodo, pid_sc_local, PAGO_ANULACION, 0);
    post(semaforo_pagos_anulaciones);

    pthread_exit(NULL);
}

void sistema_distribuido(){

    int *quiero, *mi_ticket, *max_ticket, *num_pendientes, *id_nodos_pendientes, *mi_prioridad, *dentro;

    key_t clave_mi_ticket, clave_max_ticket, clave_id_nodos_pendientes, clave_num_pendientes, clave_quiero, clave_mi_prioridad, clave_dentro;
    int mem_comp_mi_ticket, mem_comp_max_ticket, mem_comp_id_nodos_pendientes, mem_comp_num_pendientes, mem_comp_quiero, mem_comp_mi_prioridad, mem_comp_dentro;

    clave_mi_ticket = generar_clave("/home/juan/PCCD_C/receptor.c", -1 * nodo);
    clave_max_ticket = generar_clave("/home/juan/PCCD_C/lectores.c", -1 * nodo);
    clave_id_nodos_pendientes = generar_clave("/home/juan/PCCD_C/pagos_anulaciones.c", -1 * nodo);
    clave_num_pendientes = generar_clave("/home/juan/PCCD_C/prerreservas.c", -1 * nodo);
    clave_quiero = generar_clave("/home/juan/PCCD_C/pccd.c", -1 * nodo);
    clave_mi_prioridad = generar_clave("/home/juan/PCCD_C/pccd.h", -1 * nodo);
    clave_dentro = generar_clave("/home/juan/PCCD_C/inicializacion", -1 * nodo);

    mem_comp_mi_ticket = obtener_memoria_compartida(clave_mi_ticket, sizeof(int), IPC_EXCL);
    mem_comp_max_ticket = obtener_memoria_compartida(clave_max_ticket, sizeof(int), IPC_EXCL);
    mem_comp_id_nodos_pendientes = obtener_memoria_compartida(clave_id_nodos_pendientes, (num_nodos - 1)*sizeof(int), IPC_EXCL);
    mem_comp_num_pendientes = obtener_memoria_compartida(clave_num_pendientes, sizeof(int), IPC_EXCL);
    mem_comp_quiero = obtener_memoria_compartida(clave_quiero, sizeof(int), IPC_EXCL);
    mem_comp_mi_prioridad = obtener_memoria_compartida(clave_mi_prioridad, sizeof(int), IPC_EXCL);
    mem_comp_dentro = obtener_memoria_compartida(clave_dentro, sizeof(int), IPC_EXCL);

    mi_ticket = asignar_memoria_compartida(mem_comp_mi_ticket);
    max_ticket = asignar_memoria_compartida(mem_comp_max_ticket);
    id_nodos_pendientes = asignar_memoria_compartida(mem_comp_id_nodos_pendientes);
    num_pendientes = asignar_memoria_compartida(mem_comp_num_pendientes);
    quiero = asignar_memoria_compartida(mem_comp_quiero);
    mi_prioridad = asignar_memoria_compartida(mem_comp_mi_prioridad);
    dentro = asignar_memoria_compartida(mem_comp_dentro);

    wait(semaforo_atomico);
    *quiero = 1;
    *mi_ticket = *max_ticket +1;
    *mi_prioridad = PAGO_ANULACION;
    post(semaforo_atomico);

    int emisor, ticket_origen, prioridad_origen;

    int i;
    for(i=1; i<=num_nodos; i++){
        if(i != nodo){
            enviar_mensaje(REQUEST, i, nodo, *mi_ticket, *mi_prioridad);
        }
    }
    for(i=1; i<=num_nodos-1; i++){
        recibir_mensaje(REPLY, nodo, &emisor, &ticket_origen, &prioridad_origen);
    }
    wait(semaforo_atomico);
    *dentro = 1;
    post(semaforo_atomico);

    //SC
    seccion_critica_distribuda("Pago o anulacion ha entrado en la SC distribuida", nodo, pid_sc_distribuida, PAGO_ANULACION, 1);
    sleep(2);
    seccion_critica_distribuda("Pago o anulacion ha salido de la SC distribuida", nodo, pid_sc_distribuida, PAGO_ANULACION, 0);
    //distribuida

    wait(semaforo_atomico);
    *dentro = 0;
    post(semaforo_atomico);

    wait(semaforo_atomico);
    *quiero = 0;
    for(i=0; i<*num_pendientes; i++){
        enviar_mensaje(REPLY, id_nodos_pendientes[i], nodo, *mi_ticket, *mi_prioridad);
    }
    *num_pendientes=0;
    post(semaforo_atomico);
}