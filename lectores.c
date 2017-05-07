#include "pccd.c"

void *lector(void*);

sem_t* semaforo_pagos_anulaciones;
sem_t* semaforo_prerreservas;
sem_t* semaforo_prioridades;
sem_t* semaforo_lectores;
sem_t semaforo_contador;
int nodo;
int num_nodos;
int numero_lectores;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Modo de uso: ./lectores 'id_nodo' 'numero_nodos'\n");
        exit(0);
    }
    nodo = atoi(argv[1]);
    num_nodos = atoi(argv[2]);

    inicializar_semaforo(&semaforo_contador, 1);
    numero_lectores= 0;

    key_t clave_pagos_anulaciones, clave_prerreservas, clave_prioridades, clave_lectores;
    int mem_comp_pagos_anulaciones, mem_comp_prerreservas, mem_comp_prioridades, mem_comp_lectores;

    clave_pagos_anulaciones = generar_clave("pagos_anulaciones.c", nodo);
    clave_prerreservas = generar_clave("prerreservas.c", nodo);
    clave_prioridades = generar_clave("inicializacion.c", nodo);
    clave_lectores = generar_clave("lectores.c", nodo);

    mem_comp_pagos_anulaciones = obtener_memoria_compartida(clave_pagos_anulaciones, sizeof(sem_t), IPC_EXCL);
    mem_comp_prerreservas = obtener_memoria_compartida(clave_prerreservas, sizeof(sem_t), IPC_EXCL);
    mem_comp_prioridades = obtener_memoria_compartida(clave_prioridades, sizeof(sem_t), IPC_EXCL);
    mem_comp_lectores = obtener_memoria_compartida(clave_lectores, sizeof(sem_t), IPC_EXCL);

    semaforo_pagos_anulaciones = asignar_memoria_compartida(mem_comp_pagos_anulaciones);
    semaforo_prerreservas = asignar_memoria_compartida(mem_comp_prerreservas);
    semaforo_prioridades = asignar_memoria_compartida(mem_comp_prioridades);
    semaforo_lectores = asignar_memoria_compartida(mem_comp_lectores);

    while(1) {
        printf("¿Cuantos procesos de grada o evento quieres lanzar? (Pulse 0 para salir) ");
        int numero;
        scanf(" %i", &numero);
        fflush(stdin);
        if (numero == 0) exit(0);
        hora_actual(stdout);
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

    wait(&semaforo_contador);
    if(numero_lectores == 0){
        wait(semaforo_pagos_anulaciones);
        wait(semaforo_prerreservas);
    }
    numero_lectores++;
    post(&semaforo_contador);

    seccion_critica_local("Grada o evento ha entrado en la SC");
    //seccion_critica_distribuida();
    sleep(10);
    seccion_critica_local("Grada o evento ha salido de la SC");

    wait(&semaforo_contador);
    numero_lectores--;
    if(numero_lectores == 0){
        post(semaforo_prerreservas);
        post(semaforo_pagos_anulaciones);
    }
    post(&semaforo_contador);

    pthread_exit(NULL);
}