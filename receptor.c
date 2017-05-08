#include "pccd.c"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Modo de uso: ./receptor 'id_nodo' 'numero_nodos'\n");
        exit(0);
    }//numero_nodos debe ser al menos 2, si no estariamos reservando memoria para un array de longuitud 0
    int nodo = atoi(argv[1]);
    int num_nodos = atoi(argv[2]);


    int *mi_ticket, *max_ticket, *id_nodos_pendientes, *num_pendientes, *quiero, *mi_prioridad;
    key_t clave_mi_ticket, clave_max_ticket, clave_id_nodos_pendientes, clave_num_pendientes, clave_quiero, clave_mi_prioridad;
    int mem_comp_mi_ticket, mem_comp_max_ticket, mem_comp_id_nodos_pendientes, mem_comp_num_pendientes, mem_comp_quiero, mem_comp_mi_prioridad;

    clave_mi_ticket = generar_clave("receptor.c", -1 * nodo);
    clave_max_ticket = generar_clave("lectores.c", -1 * nodo);
    clave_id_nodos_pendientes = generar_clave("pagos_anulaciones.c", -1 * nodo);
    clave_num_pendientes = generar_clave("prerreservas.c", -1 * nodo);
    clave_quiero = generar_clave("pccd.c", -1 * nodo);
    clave_mi_prioridad = generar_clave("pccd.h", -1 * nodo);

    mem_comp_mi_ticket = obtener_memoria_compartida(clave_mi_ticket, sizeof(int), IPC_CREAT);
    mem_comp_max_ticket = obtener_memoria_compartida(clave_max_ticket, sizeof(int), IPC_CREAT);
    mem_comp_id_nodos_pendientes = obtener_memoria_compartida(clave_id_nodos_pendientes, (num_nodos - 1)*sizeof(int), IPC_CREAT);
    mem_comp_num_pendientes = obtener_memoria_compartida(clave_num_pendientes, sizeof(int), IPC_CREAT);
    mem_comp_quiero = obtener_memoria_compartida(clave_quiero, sizeof(int), IPC_CREAT);
    mem_comp_mi_prioridad = obtener_memoria_compartida(clave_mi_prioridad, sizeof(int), IPC_CREAT);

    mi_ticket = asignar_memoria_compartida(mem_comp_mi_ticket);
    max_ticket = asignar_memoria_compartida(mem_comp_max_ticket);
    id_nodos_pendientes = asignar_memoria_compartida(mem_comp_id_nodos_pendientes);
    num_pendientes = asignar_memoria_compartida(mem_comp_num_pendientes);
    quiero = asignar_memoria_compartida(mem_comp_quiero);
    mi_prioridad = asignar_memoria_compartida(mem_comp_mi_prioridad);

    *quiero = 0;
    *mi_prioridad = 100;
    *num_pendientes = 0;
    *mi_ticket = 0;

    key_t clave_buzon = generar_clave("pccd.c", nodo);
    obtener_buzon(clave_buzon, IPC_CREAT);

    /***************************************************************************************/

    /*************************************************************************************/

    key_t clave_atomico = generar_clave("inicializacion.c", -1*nodo);
    int mem_comp_atomico = obtener_memoria_compartida(clave_atomico, sizeof(sem_t), IPC_CREAT);
    sem_t *semaforo_atomico = asignar_memoria_compartida(mem_comp_atomico);
    inicializar_semaforo(semaforo_atomico, 1);

    int vecino = 0, ticket_vecino = 0, prioridad_vecino = 0;

    while (1) {
        recibir_mensaje(REQUEST, nodo, &vecino, &ticket_vecino, &prioridad_vecino);
        printf("He recidido un mensaje del nodo %i con prioridad %i\n", vecino, prioridad_vecino);
        wait(semaforo_atomico);
        *max_ticket = maximo(ticket_vecino, *max_ticket);
        //wait(semaforo_atomico);
        if(*quiero != 1 || *mi_prioridad < prioridad_vecino ||
                (*mi_prioridad == prioridad_vecino && *mi_ticket > ticket_vecino) ||
                (*mi_prioridad == prioridad_vecino && *mi_ticket == ticket_vecino && nodo > vecino) ||
                (*mi_prioridad == LECTOR && prioridad_vecino == LECTOR)){
            enviar_mensaje(REPLY, vecino, nodo, 0, *mi_prioridad);
        }else{
            id_nodos_pendientes[*num_pendientes] = vecino;
            *num_pendientes = *num_pendientes +1;
        }
        post(semaforo_atomico);
    }
}