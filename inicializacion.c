//
// Created by juan on 5/05/17.
//
#include "pccd.c"

int main(int argc, char *argv[]){

    if(argc != 2){
        printf("Modo de uso: ./inicializacion 'nodo'\n");
        exit(0);
    }

    int nodo = atoi(argv[1]);

    key_t clave_pagos_anulaciones, clave_prerreservas, clave_prioridades, clave_lectores, clave_escritores_contador, clave_escritores_semaforo;
    int mem_comp_pagos_anulaciones, mem_comp_prerreservas, mem_comp_prioridades, mem_comp_lectores, mem_comp_escritores_contador, mem_comp_escritores_semaforo;
    sem_t *semaforo_pagos_anulaciones, *semaforo_prerreservas, *semaforo_prioridades, *semaforo_lectores, *semaforo_escritores;
    int *numero_escritores;

    clave_pagos_anulaciones = generar_clave("pagos_anulaciones.c", nodo);
    clave_prerreservas = generar_clave("prerreservas.c", nodo);
    clave_prioridades = generar_clave("inicializacion.c", nodo);
    clave_lectores = generar_clave("lectores.c", nodo);
    clave_escritores_contador = generar_clave("pccd.c", nodo);
    clave_escritores_semaforo = generar_clave("receptor.c", nodo);

    mem_comp_pagos_anulaciones = obtener_memoria_compartida(clave_pagos_anulaciones, sizeof(sem_t), IPC_CREAT);
    mem_comp_prerreservas = obtener_memoria_compartida(clave_prerreservas, sizeof(sem_t), IPC_CREAT);
    mem_comp_prioridades = obtener_memoria_compartida(clave_prioridades, sizeof(sem_t), IPC_CREAT);
    mem_comp_lectores = obtener_memoria_compartida(clave_lectores, sizeof(sem_t), IPC_CREAT);
    mem_comp_escritores_contador = obtener_memoria_compartida(clave_escritores_contador, sizeof(int), IPC_CREAT);
    mem_comp_escritores_semaforo = obtener_memoria_compartida(clave_escritores_semaforo, sizeof(sem_t), IPC_CREAT);


    semaforo_pagos_anulaciones = asignar_memoria_compartida(mem_comp_pagos_anulaciones);
    semaforo_prerreservas = asignar_memoria_compartida(mem_comp_prerreservas);
    semaforo_prioridades = asignar_memoria_compartida(mem_comp_prioridades);
    semaforo_lectores = asignar_memoria_compartida(mem_comp_lectores);
    numero_escritores = asignar_memoria_compartida(mem_comp_escritores_contador);
    semaforo_escritores = asignar_memoria_compartida(mem_comp_escritores_semaforo);

    inicializar_semaforo(semaforo_pagos_anulaciones, 1);
    inicializar_semaforo(semaforo_prerreservas, 1);
    inicializar_semaforo(semaforo_prioridades, 1);
    inicializar_semaforo(semaforo_lectores, 1);
    *numero_escritores = 0;
    inicializar_semaforo(semaforo_escritores, 1);

    printf("Memoria compartida inicializada correctamente\n");

    if(remove("seccion_critica_local.txt") != 0){
        perror("Error al eliminar la SC");
        exit(-1);
    }

    return 0;
}
