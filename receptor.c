#include "pccd.c"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Modo de uso: ./receptor 'id_nodo' 'numero_nodos'\n");
        exit(0);
    }//numero_nodos debe ser al menos 2, si no estariamos reservando memoria para un array de longuitud 0
    int nodo = atoi(argv[1]);
    int num_nodos = atoi(argv[2]);


    int *mi_ticket, *max_ticket, *id_nodos_pendientes, *num_pendientes, *quiero, *mi_prioridad, *dentro;
    key_t clave_mi_ticket, clave_max_ticket, clave_id_nodos_pendientes, clave_num_pendientes, clave_quiero, clave_mi_prioridad, clave_dentro;
    int mem_comp_mi_ticket, mem_comp_max_ticket, mem_comp_id_nodos_pendientes, mem_comp_num_pendientes, mem_comp_quiero, mem_comp_mi_prioridad, mem_comp_dentro;

    clave_mi_ticket = generar_clave("/home/juan/PCCD_C/receptor.c", -1 * nodo);
    clave_max_ticket = generar_clave("/home/juan/PCCD_C/lectores.c", -1 * nodo);
    clave_id_nodos_pendientes = generar_clave("/home/juan/PCCD_C/pagos_anulaciones.c", -1 * nodo);
    clave_num_pendientes = generar_clave("/home/juan/PCCD_C/prerreservas.c", -1 * nodo);
    clave_quiero = generar_clave("/home/juan/PCCD_C/pccd.c", -1 * nodo);
    clave_mi_prioridad = generar_clave("/home/juan/PCCD_C/pccd.h", -1 * nodo);
    clave_dentro = generar_clave("/home/juan/PCCD_C/inicializacion", -1 * nodo);

    mem_comp_mi_ticket = obtener_memoria_compartida(clave_mi_ticket, sizeof(int), IPC_CREAT);
    mem_comp_max_ticket = obtener_memoria_compartida(clave_max_ticket, sizeof(int), IPC_CREAT);
    mem_comp_id_nodos_pendientes = obtener_memoria_compartida(clave_id_nodos_pendientes, (num_nodos - 1)*sizeof(int), IPC_CREAT);
    mem_comp_num_pendientes = obtener_memoria_compartida(clave_num_pendientes, sizeof(int), IPC_CREAT);
    mem_comp_quiero = obtener_memoria_compartida(clave_quiero, sizeof(int), IPC_CREAT);
    mem_comp_mi_prioridad = obtener_memoria_compartida(clave_mi_prioridad, sizeof(int), IPC_CREAT);
    mem_comp_dentro = obtener_memoria_compartida(clave_dentro, sizeof(int), IPC_CREAT);

    mi_ticket = asignar_memoria_compartida(mem_comp_mi_ticket);
    max_ticket = asignar_memoria_compartida(mem_comp_max_ticket);
    id_nodos_pendientes = asignar_memoria_compartida(mem_comp_id_nodos_pendientes);
    num_pendientes = asignar_memoria_compartida(mem_comp_num_pendientes);
    quiero = asignar_memoria_compartida(mem_comp_quiero);
    mi_prioridad = asignar_memoria_compartida(mem_comp_mi_prioridad);
    dentro = asignar_memoria_compartida(mem_comp_dentro);

    *quiero = 0;
    *mi_prioridad = 0;
    *num_pendientes = 0;
    *mi_ticket = 0;
    *dentro = 0;

    key_t clave_buzon = generar_clave("/home/juan/PCCD_C/pccd.c", nodo);
    int buzon = obtener_buzon(clave_buzon, IPC_CREAT);
    printf("El buzon es %i y se genero con %i\n", buzon, clave_buzon);

    /***************************************************************************************/



    /***************************************************************************************/

    key_t clave_atomico = generar_clave("/home/juan/PCCD_C/inicializacion.c", -1*nodo);
    int mem_comp_atomico = obtener_memoria_compartida(clave_atomico, sizeof(sem_t), IPC_CREAT);
    sem_t *semaforo_atomico = asignar_memoria_compartida(mem_comp_atomico);
    inicializar_semaforo(semaforo_atomico, 1);

    int vecino = 0, ticket_vecino = 0, prioridad_vecino = 0;

    while (1) {
        printf("Esperando una peticion...\n");
        recibir_mensaje(REQUEST, nodo, &vecino, &ticket_vecino, &prioridad_vecino);
        printf("He recibido un mensaje del nodo %i con prioridad %i\n", vecino, prioridad_vecino);
        wait(semaforo_atomico);
        *max_ticket = maximo(ticket_vecino, *max_ticket);
        //wait(semaforo_atomico);
        printf("Yo quiero: %i, Mi ticket: %i, Max ticket: %i, Mi prioridad: %i\n", *quiero, *mi_ticket, *max_ticket, *mi_prioridad);
        if(*quiero != 1 || (*mi_prioridad < prioridad_vecino && *dentro != 1) ||
                (*mi_prioridad == prioridad_vecino && *mi_ticket > ticket_vecino) ||
                (*mi_prioridad == prioridad_vecino && *mi_ticket == ticket_vecino && nodo > vecino) ||
                (*mi_prioridad == LECTOR && prioridad_vecino == LECTOR)){
            enviar_mensaje(REPLY, vecino, nodo, 0, *mi_prioridad);
            printf("Doy permiso al nodo %i con prioridad %i y ticket %i\n", vecino, prioridad_vecino, ticket_vecino);
        }else{
            id_nodos_pendientes[*num_pendientes] = vecino;
            *num_pendientes = *num_pendientes +1;
        }
        post(semaforo_atomico);
    }
}