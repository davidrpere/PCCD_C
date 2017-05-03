/*
 *
 * Proceso escritor
 *
 * Debe recibir por parámetros su prioridad para competir con los demás escritores
 *
 */
#include <semaphore.h>


void wait(sem_t *semaforo);
void signal(sem_t *semaforo);

int main(int argc, char *argv[]){
    if(argc != 4){
        printf("Modo de uso: escritores id_nodo numero_nodos");
        exit(0);
    }
    int id_nodo = atoi[argv[1]];
    int num_nodos = atoi[argv[2]];

    while(1){
        printf("1. Lanzar pagos\n");
        printf("2. Lanzar anulacion\n");
        printf("3. Lanzar prerreserva\n");
        printf("Seleccione la operacion: ");
        int operacion = getchar();
        fflush(stdin);
        printf("¿Cuantos procesos quieres? ");
        int numero = getchar();
        fflush(stdin);
        int i=0, prioridad = 0;
        pthread_t hilo;
        if(operacion == 1){
            //lanzar pagos
            for(i=0; i<numero; i++){
                if (pthread_create(&hilo, NULL, (void *)escritor, &struct_escritor[i])) {
                    perror("Error creando pago\n");
                    exit(-1);
                }
            }
        }else if(operacion == 2){
            //lanzar anulaciones
            for(i=0; i<numero; i++){
                if (pthread_create(&pthread_escritor[i], NULL, (void *)escritor, &struct_escritor[i])) {
                    perror("Error creando anulacion\n");
                    exit(-1);
                }
            }
        }else if(operacion == 3){
            //lanzar prerreservas
            for(i=0; i<numero; i++){
                if (pthread_create(&pthread_escritor[i], NULL, (void *)escritor, &struct_escritor[i])) {
                    perror("Error creando prerreserva\n");
                    exit(-1);
                }
            }
        }else{
            printf("Operacion no permitida\n");
        }
    }

}


void *escritor(void *parametro){
    int prioridad = (int) *parametro;
}



void crear_hilo(){
    if (pthread_create(&pthread_escritor[i], NULL, (void *)escritor,&struct_escritor[i])) {
        perror("Error creando hilo lector. \n");
        exit(-1);
    }
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