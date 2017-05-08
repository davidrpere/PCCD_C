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

    if(modo == IPC_CREAT){
        memoria_compartida = shmget(clave, talla, 0777 | IPC_CREAT);
        FILE *fichero = fopen("memoria_compartida.txt", "a");
        if(fprintf(fichero, "%i\n", memoria_compartida) < 0 || fclose(fichero) != 0){
            perror("Error al escribir en el fichero 'memoria_compartida.txt'");
            exit(-1);
        }
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

void seccion_critica_local(char *mensaje, int nodo){
    FILE *fichero = fopen("seccion_critica_local.txt", "a");
    hora_actual(fichero);
    if(fprintf(fichero, "%i %s\n", nodo, mensaje) < 0 || fclose(fichero) != 0){
        perror("Error al escribir en la seccion critica local");
        exit(-1);
    }
}

void seccion_critica_distribuda(char *mensaje, int nodo){
    FILE *fichero = fopen("seccion_critica_distribuida.txt", "a");
    hora_actual(fichero);
    if(fprintf(fichero, "%i %s\n", nodo, mensaje) < 0 || fclose(fichero) != 0){
        perror("Error al escribir en la seccion critica distribuida");
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

void enviar_mensaje(long tipo, int destino, int emisor, int ticket, int prioridad){

    key_t clave = generar_clave("pccd.c", destino);
    int buzon = obtener_buzon(clave, IPC_EXCL);

    struct mensaje_struct mensaje;
    mensaje.emisor = emisor;
    mensaje.prioridad = prioridad;
    mensaje.ticket = ticket;
    mensaje.mtype = tipo;

    if(msgsnd(buzon, &mensaje, sizeof(mensaje)-sizeof(long), IPC_NOWAIT) == -1){
        perror("Error al enviar un mensaje_struct");
        exit(-1);
    }
}

void recibir_mensaje(long tipo, int receptor, int* emisor, int* ticket_origen, int* prioridad_vecino){

    key_t clave = generar_clave("pccd.c", receptor);
    int buzon = obtener_buzon(clave, IPC_EXCL);
    struct mensaje_struct mensaje;

    if(msgrcv(buzon, (void*) &mensaje, sizeof(mensaje)-sizeof(long), tipo, 0) == sizeof(mensaje)-sizeof(mensaje.mtype)-1){
        perror("Error al recibir un mensaje_struct");
        exit(-1);
    }

    *emisor = mensaje.emisor;
    *ticket_origen = mensaje.ticket;
    *prioridad_vecino = mensaje.prioridad;

}

int obtener_buzon(key_t clave, int modo){
    int buzon = -1;
    if(modo == IPC_CREAT){
        buzon = msgget(clave, 0777 | IPC_CREAT);
    }else if(modo == IPC_EXCL){
        buzon = msgget(clave, 0777);
    }

    if(buzon == -1){
        if(modo == IPC_CREAT){
            perror("Error al crear un buzon");
            exit(-1);
        }else if(modo == IPC_EXCL){
            perror("Error al utilizar un buzon");
            exit(-1);
        }else{
            perror("Error al obtener un buzon");
            exit(-1);
        }
    }
    return buzon;
}