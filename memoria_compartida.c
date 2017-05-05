#include <semaphore.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>

sem_t semaforo_lectores;
sem_t semaforo_prerreservas;
sem_t semaforo_pago_anulaciones;

int main(int argc, char *argv[]){

    int shmid;
    int* dir_semaforos[3];
    key_t key_semaforos[3] = {88888881, 88888882, 88888883};

    for(int i = 0 ; i < 3 ; i++){
        if ((shmid = shmget(key_semaforos[i], sizeof(sem_t), IPC_CREAT)) == -1) {
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
    sem_init(dir_semaforos[0], 0, 1);
    sem_init(dir_semaforos[1], 0, 1);
    sem_init(dir_semaforos[2], 0, 1);

    return 0;
}