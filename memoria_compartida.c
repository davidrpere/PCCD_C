#include <semaphore.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>

sem_t semaforo_lectores;
sem_t semaforo_prerreservas;
sem_t semaforo_pago_anulaciones;

int main(int argc, char *argv[]){

    key_t key_semaforo_lectores = 88888881;
    key_t key_semaforo_prerreservas = 88888882;
    key_t key_semaforo_pago_anulacion = 88888883;

    int shmid;
    int* dir_semaforos[3];

    for(int i = 0 ; i < 3 ; i++){
        if ((shmid = shmget(key_semaforo_pago_anulacion, sizeof(sem_t), IPC_CREAT)) == -1) {
            perror("Hubo un error al ejecutar shmget para un semáforo.");
            exit(1);
        } else {
            printf("Shmget devolvió %d .\n", shmid);
            if((dir_semaforos[i] = shmat(shmid, NULL, 0)) == -1){
                perror("Hubo un error al ejecutar shmat para un semaforo.");
                exit(1);
            }
        }
    }

    //Inicialización de los 3 semáforos de excl. mutua
    sem_init(&semaforo_lectores, 0, 1);
    sem_init(&semaforo_prerreservas, 0, 1);
    sem_init(&semaforo_pago_anulaciones, 0, 1);

    //Almacenamos en las 3 zonas de memoria compartida las direcciones de los semáforos
    dir_semaforos[0] = &semaforo_lectores;
    dir_semaforos[1] = &semaforo_prerreservas;
    dir_semaforos[2] = &semaforo_pago_anulaciones;

    return 0;
}