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

void seccion_critica_local(char *mensaje){
    FILE *fichero = fopen("seccion_critica_local.txt", "a");
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

void enviar_mensaje(int tipo, int destino, int emisor, int ticket, int prioridad){

    key_t clave = generar_clave("pccd.c", destino);
    int buzon = obtener_buzon(clave, IPC_EXCL);

    Mensaje mensaje;
    mensaje.emisor = emisor;
    mensaje.prioridad = prioridad;
    mensaje.ticket = ticket;
    mensaje.mtype = tipo;

    if(msgsnd(buzon, (void *) &mensaje, sizeof(mensaje)-sizeof(mensaje.mtype), IPC_NOWAIT) == -1){
        perror("Error al enviar un mensaje");
        exit(-1);
    }
}

void recibir_mensaje(int tipo, int receptor, int* emisor, int* ticket_origen, int* prioridad_vecino){

    key_t clave = generar_clave("pccd.c", receptor);
    int buzon = obtener_buzon(clave, IPC_EXCL);
    Mensaje mensaje;

    if(msgrcv(buzon, (void*) &mensaje, sizeof(mensaje)-sizeof(mensaje.mtype), tipo, 0) == sizeof(mensaje)-sizeof(mensaje.mtype)-1){
        perror("Error al recibir un mensaje");
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

void seccion_critica_distribuida(int nodo, int num_nodos, int tipo){

    int *quiero, *mi_ticket, *max_ticket, *num_pendientes, *id_nodos_pendientes, *mi_prioridad;

    key_t clave_mi_ticket, clave_max_ticket, clave_id_nodos_pendientes, clave_num_pendientes, clave_quiero, clave_mi_prioridad;
    int mem_comp_mi_ticket, mem_comp_max_ticket, mem_comp_id_nodos_pendientes, mem_comp_num_pendientes, mem_comp_quiero, mem_comp_mi_prioridad;

    clave_mi_ticket = generar_clave("receptor.c", -1 * nodo);
    clave_max_ticket = generar_clave("lectores.c", -1 * nodo);
    clave_id_nodos_pendientes = generar_clave("pagos_anulaciones.c", -1 * nodo);
    clave_num_pendientes = generar_clave("prerreservas.c", -1 * nodo);
    clave_quiero = generar_clave("pccd.c", -1 * nodo);
    clave_mi_prioridad = generar_clave("pccd.h", -1 * nodo);

    mem_comp_mi_ticket = obtener_memoria_compartida(clave_mi_ticket, sizeof(int), IPC_EXCL);
    mem_comp_max_ticket = obtener_memoria_compartida(clave_max_ticket, sizeof(int), IPC_EXCL);
    mem_comp_id_nodos_pendientes = obtener_memoria_compartida(clave_id_nodos_pendientes, (num_nodos - 1)*sizeof(int), IPC_EXCL);
    mem_comp_num_pendientes = obtener_memoria_compartida(clave_num_pendientes, sizeof(int), IPC_EXCL);
    mem_comp_quiero = obtener_memoria_compartida(clave_quiero, sizeof(int), IPC_EXCL);
    mem_comp_mi_prioridad = obtener_memoria_compartida(clave_mi_prioridad, sizeof(int), IPC_EXCL);

    mi_ticket = asignar_memoria_compartida(mem_comp_mi_ticket);
    max_ticket = asignar_memoria_compartida(mem_comp_max_ticket);
    id_nodos_pendientes = asignar_memoria_compartida(mem_comp_id_nodos_pendientes);
    num_pendientes = asignar_memoria_compartida(mem_comp_num_pendientes);
    quiero = asignar_memoria_compartida(mem_comp_quiero);
    mi_prioridad = asignar_memoria_compartida(mem_comp_mi_prioridad);


    *quiero = 1;
    *mi_ticket = *max_ticket +1;
    *mi_prioridad = tipo;

    int emisor, ticket_origen, prioridad_origen;

    int i;
    for(i=0; i<num_nodos; i++){
        if(i != nodo){
            enviar_mensaje(REQUEST, i, nodo, *mi_ticket, *mi_prioridad);
        }
    }
    for(i=0; i<num_nodos-1; i++){
        recibir_mensaje(REPLY, nodo, &emisor, &ticket_origen, &prioridad_origen);
    }
    //SC
    sleep(1);
    //distribuida
    *quiero = 0;
    for(i=0; i<*num_pendientes; i++){
        enviar_mensaje(REPLY, id_nodos_pendientes[i], nodo, *mi_ticket, *mi_prioridad);
    }
    *num_pendientes=0;

}