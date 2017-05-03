//
// Created by David Rodriguez Pereira on 3/5/17.
//

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define PRIORIDAD 2

void wait(sem_t *semaforo);
void signal(sem_t *semaforo);
void *prerreserva(void*);

int main(int argc, char *argv[]){
    if(argc != 4){
        printf("Modo de uso: escritores id_nodo numero_nodos");
        exit(0);
    }
    int id_nodo = atoi[argv[1]];
    int num_nodos = atoi[argv[2]];

    while(1){
        printf("¿Cuantos procesos de prerreservas quieres lanzar? ");
        int numero;
        scanf(" &i", &numero);
        fflush(stdin);
        int i=0;
        pthread_t hilo;
        for(; i<numero; i++){
            if (pthread_create(&hilo, NULL, (void *) prerreserva, (void *) NULL) {
                perror("Error creando una prerreserva");
                exit(-1);
            }
        }
    }

}


void *prerreserva(void *parametro){

    wait(semaforo_lectores);
    wait(semaforo_prerreservas);
    if(get(semaforo_pago_anuaciones))
    wait(semaforo_pago_anulaciones);
    //SC
    sleep(10);
    //local
    signal(semaforo_pago_anulaciones);
    signal(semaforo_prerreservas);
    signal(semaforo_lectores);

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