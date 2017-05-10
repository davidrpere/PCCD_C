#include "pccd.c"

void *lector(void*);
void sistema_distribuido(void);

sem_t* semaforo_pagos_anulaciones;
sem_t* semaforo_prerreservas;
sem_t* semaforo_prioridades;
sem_t* semaforo_lectores;
sem_t semaforo_contador_local;
sem_t semaforo_contador_distribuido;
sem_t semaforo_permiso;
sem_t* semaforo_atomico;
int nodo;
int num_nodos;
int numero_lectores_local;
int numero_lectores_distribuido;
pid_t pid_sc_local, pid_sc_distribuida;

int main(int argc, char *argv[]){
    if(argc != 5){
        printf("Modo de uso: ./lectores 'id_nodo' 'numero_nodos' 'pid_sc_local' 'pid_sc_distribuido'\n");
        exit(0);
    }
    nodo = atoi(argv[1]);
    num_nodos = atoi(argv[2]);
    pid_sc_local = atoi(argv[3]);
    pid_sc_distribuida = atoi(argv[4]);

    inicializar_semaforo(&semaforo_contador_local, 1);
    inicializar_semaforo(&semaforo_permiso, 1);
    inicializar_semaforo(&semaforo_contador_distribuido, 1);
    numero_lectores_local= 0;
    numero_lectores_distribuido = 0;

    key_t clave_pagos_anulaciones, clave_prerreservas, clave_prioridades, clave_lectores;
    int mem_comp_pagos_anulaciones, mem_comp_prerreservas, mem_comp_prioridades, mem_comp_lectores;

    clave_pagos_anulaciones = generar_clave("/home/juan/PCCD_C/pagos_anulaciones.c", nodo);
    clave_prerreservas = generar_clave("/home/juan/PCCD_C/prerreservas.c", nodo);
    clave_prioridades = generar_clave("/home/juan/PCCD_C/inicializacion.c", nodo);
    clave_lectores = generar_clave("/home/juan/PCCD_C/lectores.c", nodo);

    mem_comp_pagos_anulaciones = obtener_memoria_compartida(clave_pagos_anulaciones, sizeof(sem_t), IPC_EXCL);
    mem_comp_prerreservas = obtener_memoria_compartida(clave_prerreservas, sizeof(sem_t), IPC_EXCL);
    mem_comp_prioridades = obtener_memoria_compartida(clave_prioridades, sizeof(sem_t), IPC_EXCL);
    mem_comp_lectores = obtener_memoria_compartida(clave_lectores, sizeof(sem_t), IPC_EXCL);

    semaforo_pagos_anulaciones = asignar_memoria_compartida(mem_comp_pagos_anulaciones);
    semaforo_prerreservas = asignar_memoria_compartida(mem_comp_prerreservas);
    semaforo_prioridades = asignar_memoria_compartida(mem_comp_prioridades);
    semaforo_lectores = asignar_memoria_compartida(mem_comp_lectores);

    key_t clave_atomico = generar_clave("/home/juan/PCCD_C/inicializacion.c", -1*nodo);
    int mem_comp_atomico = obtener_memoria_compartida(clave_atomico, sizeof(sem_t), IPC_EXCL);
    semaforo_atomico = asignar_memoria_compartida(mem_comp_atomico);

    while(1) {
        printf("¿Cuantos procesos de grada o evento quieres lanzar? (Pulse 0 para salir) ");
        int numero;
        scanf(" %i", &numero);
        fflush(stdin);
        if (numero == 0) exit(0);
        int i = 0;
        pthread_t hilo;
        for (; i < numero; i++) {
            crear_hilo(&hilo, lector, NULL);
        }
    }

}

void *lector(void* parametro){

    wait(semaforo_lectores);
    post(semaforo_lectores);

    wait(&semaforo_contador_local);
    if(numero_lectores_local == 0){
        wait(semaforo_pagos_anulaciones);
        wait(semaforo_prerreservas);
    }
    numero_lectores_local++;
    post(&semaforo_contador_local);

    seccion_critica_local("Grada o evento ha entrado en la SC local", nodo, pid_sc_local, LECTOR, 1);
    sistema_distribuido();
    seccion_critica_local("Grada o evento ha salido de la SC local", nodo, pid_sc_local, LECTOR, 0);

    wait(&semaforo_contador_local);
    numero_lectores_local--;
    if(numero_lectores_local == 0){
        post(semaforo_prerreservas);
        post(semaforo_pagos_anulaciones);
    }
    post(&semaforo_contador_local);

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

    wait(&semaforo_permiso);

    int emisor, ticket_origen, prioridad_origen;

    int i;

    wait(&semaforo_contador_distribuido);
    if(numero_lectores_distribuido == 0){
        wait(semaforo_atomico);
        *quiero = 1;
        *mi_ticket = *max_ticket +1;
        *mi_prioridad = LECTOR;
        post(semaforo_atomico);
        for (i = 1; i <= num_nodos; i++) {
            if (i != nodo) {
                enviar_mensaje(REQUEST, i, nodo, *mi_ticket, *mi_prioridad);
            }
        }
        for (i = 1; i <= num_nodos - 1; i++) {
            recibir_mensaje(REPLY, nodo, &emisor, &ticket_origen, &prioridad_origen);
        }
        wait(semaforo_atomico);
        *dentro = 1;
        post(semaforo_atomico);
    }
    numero_lectores_distribuido++;
    post(&semaforo_contador_distribuido);

    post(&semaforo_permiso);
    //SC
    seccion_critica_distribuda("Lector ha entrado en la SC distribuida", nodo, pid_sc_distribuida, LECTOR, 1);
    sleep(2);
    seccion_critica_distribuda("Lector ha salido de la SC distribuida", nodo, pid_sc_distribuida, LECTOR, 0);
    //distribuida

    wait(&semaforo_contador_distribuido);
    numero_lectores_distribuido--;
    if(numero_lectores_distribuido == 0){
        wait(semaforo_atomico);
        *dentro = 0;
        post(semaforo_atomico);
        wait(semaforo_atomico);
        *quiero = 0;
        for (i = 0; i < *num_pendientes; i++) {
            enviar_mensaje(REPLY, id_nodos_pendientes[i], nodo, *mi_ticket, *mi_prioridad);
        }
        *num_pendientes = 0;
        post(semaforo_atomico);
    }
    post(&semaforo_contador_distribuido);
}