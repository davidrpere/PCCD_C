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

    key_t clave_pagos_anulaciones, clave_prerreservas, clave_prioridades;
    int mem_comp_pagos_anulaciones, mem_comp_prerreservas, mem_comp_prioridades;
    sem_t *semaforo_pagos_anulaciones, *semaforo_prerreservas, *semaforo_prioridades;

    clave_pagos_anulaciones = generar_clave("pagos_anulaciones.c", nodo);
    clave_prerreservas = generar_clave("prerreservas.c", nodo);
    clave_prioridades = generar_clave("inicializacion.c", nodo);

    mem_comp_pagos_anulaciones = obtener_memoria_compartida(clave_pagos_anulaciones, sizeof(sem_t), IPC_CREAT);
    mem_comp_prerreservas = obtener_memoria_compartida(clave_prerreservas, sizeof(sem_t), IPC_CREAT);
    mem_comp_prioridades = obtener_memoria_compartida(clave_prioridades, sizeof(sem_t), IPC_CREAT);

    semaforo_pagos_anulaciones = asignar_memoria_compartida(mem_comp_pagos_anulaciones);
    semaforo_prerreservas = asignar_memoria_compartida(mem_comp_prerreservas);
    semaforo_prioridades = asignar_memoria_compartida(mem_comp_prioridades);

    inicializar_semaforo(semaforo_pagos_anulaciones, 1);
    inicializar_semaforo(semaforo_prerreservas, 1);
    inicializar_semaforo(semaforo_prioridades, 1);

    printf("Semaforos inicializados correctamente\n");

    if(remove("seccion_critica.txt") != 0){
        perror("Error al eliminar la SC");
        exit(-1);
    }

    return 0;
}
