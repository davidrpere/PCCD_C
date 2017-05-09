#include <semaphore.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/msg.h>

#define PAGO_ANULACION 3
#define PRERRESERVA 2
#define LECTOR 1
#define REQUEST 2
#define REPLY 1
#define maximo(x,y) ((x) > (y)) ? (x) : (y)

struct mensaje_struct{
    long mtype;
    int emisor;
    int ticket;
    int prioridad;
};

void wait(sem_t *semaforo);
void post(sem_t *semaforo);
void inicializar_semaforo(sem_t *semaforo, unsigned int valor);
void crear_hilo(pthread_t *hilo, void *(*start_routine) (void *), void *parametro);
key_t generar_clave(const char *fichero, int semilla);
int obtener_memoria_compartida(key_t clave, size_t talla, int modo);
void *asignar_memoria_compartida(int zona_memoria);
char *hora_actual();
void enviar_mensaje(long tipo, int destino, int emisor, int ticket, int prioridad);
void recibir_mensaje(long tipo, int receptor, int* emisor, int* ticket_origen, int* prioridad_vecino);
int obtener_buzon(key_t clave, int modo);
void seccion_critica_distribuda(char *mensaje, int nodo);
void seccion_critica_local(char *mensaje, int nodo);
void cuenta_atras(int segundos);