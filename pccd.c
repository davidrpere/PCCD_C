#include "pccd.h"

void wait(sem_t *semaforo){
    if(sem_wait(semaforo) != 0){
        perror("Error al hacer un wait");
        exit(-1);
    }
}

void post(sem_t *semaforo){
    if(sem_post(semaforo) != 0){
        perror("Error al hacer un post");
        exit(-1);
    }
}

void inicializar_semaforo(sem_t *semaforo, unsigned int valor){
    if(sem_init(semaforo, 1, valor) != 0){ //Hay que poner un 1!!!!!VI
        perror("Error al inicializar un semaforo");
        exit(-1);
    }
}

void crear_hilo(pthread_t *hilo, void *(*start_routine) (void *), void *parametro){
    if (pthread_create(hilo, NULL, *start_routine, parametro) != 0){
        perror("Error creando un hilo");
        exit(-1);
    }
}

key_t generar_clave(const char *fichero, int semilla){
    key_t clave = ftok(fichero, semilla);
    if(clave == -1) {
        perror("Error al generar una clave");
        exit(-1);
    }
    return clave;
}

int obtener_memoria_compartida(key_t clave, size_t talla, int modo){
    int memoria_compartida = -1;
    //memoria_compartida = shmget(clave, talla, IPC_CREAT | IPC_EXCL | 0666);

    if(modo == IPC_CREAT){
        memoria_compartida = shmget(clave, talla, 0777 | IPC_CREAT);
    }else if(modo == IPC_EXCL){
        memoria_compartida = shmget(clave, talla, 0777);
    }

    if(memoria_compartida == -1){
        if(modo == IPC_CREAT){
            perror("Error al crear una zona de memoria compartida");
        }else if(modo == IPC_EXCL){
            perror("Error al usar una zona de memoria compartida");
        }else{
            perror("Error al obtener una zona de memoria compartida");
        }
        exit(-1);
    }
    return memoria_compartida;
}

void *asignar_memoria_compartida(int zona_memoria){
    void* asignacion = shmat(zona_memoria, NULL, 0);
    if(asignacion == (void*) -1){
        perror("Error al asignar una zona de memoria compartida");
        exit(-1);
    }
    return asignacion;
}

void seccion_critica(char *mensaje){
    FILE *fichero = fopen("seccion_critica.txt", "a");
    hora_actual(fichero);
    if(fprintf(fichero, "%s\n", mensaje) < 0 || fclose(fichero) != 0){
        perror("Error al escribir en la seccion critica");
        exit(-1);
    }
}

void hora_actual(FILE *fichero){
    time_t     ahora;
    struct tm *hora;
    char       buf[80];

    ahora = time(0);
    hora = localtime(&ahora);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", hora);
    fprintf(fichero, "%s\n", buf);
}