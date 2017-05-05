//
// Created by David Rodriguez Pereira on 3/5/17.
//

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define PRIORIDAD 2

//Declaraciones de funciones
void wait(sem_t *semaforo);
void signal(sem_t *semaforo);
void *prerreserva(void*);

// Direcciones de los semáforos, por orden: lectores(0), prerreservas(1), pagos anulaciones (2)
int* dir_semaforos[3];


int main(int argc, char *argv[]){

    if(argc != 3){
        printf("Modo de uso: escritores id_nodo numero_nodos");
        exit(0);
    }
    int id_nodo = atoi[argv[1]];
    int num_nodos = atoi[argv[2]];

    //Key para memoria compartida para el semáforo de lectores, de prerreservas y de pagos_anulaciones
    key_t key_semaforo_lectores = 88888881;
    key_t key_semaforo_prerreservas = 88888882;
    key_t key_semaforo_pago_anulacion = 88888883;

    int shmid;

    for(int i = 0 ; i < 3 ; i++){
        if ((shmid = shmget(key_semaforo_pago_anulacion, sizeof(sem_t), IPC_CREAT)) == -1) {
            perror("Hubo un error al ejecutar shmget para el semáforo");
            exit(1);
        } else {
            printf("Shmget devolvió %d .\n", shmid);
            if((dir_semaforos[i] = shmat(shmid, NULL, 0)) == -1){
                perror("Hubo un error al ejecutar shmat para el semáforo");
                exit(1);
            }
        }
    }

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
    int valor_sem_pa;
    sem_wait(dir_semaforos[0]);
    sem_wait(dir_semaforos[1]);

    if(sem_getvalue(dir_semaforos[3], &valor_sem_pa)==0){
        if(valor_sem_pa==1){
            sem_wait(dir_semaforos[2]);
        }else{

        }
    }else{
        perror("Error en sem_getvalue.");
    }
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