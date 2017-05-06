#include <semaphore.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include

void wait(sem_t *semaforo);
void post(sem_t *semaforo);
void inicializar_semaforo(sem_t *semaforo, unsigned int valor);
void crear_hilo(pthread_t *hilo, void *(*start_routine) (void *), void *parametro);
key_t generar_clave(const char *fichero, int semilla);
int obtener_memoria_compartida(key_t clave, size_t talla, int modo);
void *asignar_memoria_compartida(int zona_memoria);
void hora_actual(FILE *fichero);