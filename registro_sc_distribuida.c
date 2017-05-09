#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void handler(int signum);

int i = 0;

int main(int argc, char *argv[]) {

    struct sigaction act;

    memset(&act, 0, sizeof(act));
    act.sa_handler = handler;
    act.sa_flags = 0;

    sigaction(SIGALRM, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    sigaction(5, &act, NULL);
    sigaction(6, &act, NULL);
    sigaction(7, &act, NULL);

    printf("PID proceso SC distribuida: %d\n\n", getpid());

    while(1){
        pause();
    }

}


/**
 * Función usada para detectar las señales de los procesos que entran en la SC
 *
 * @param signum
 */

void handler(int signum) {
    switch (signum) {
        case SIGUSR1:
            //pago_anulacion en SC
            printf(ANSI_COLOR_RED     "%d PROCESO PAGO o ANULACIÓN EN SECCIÓN CRÍTICA DISTRIBUIDA"     ANSI_COLOR_RESET "\n",
                   i);
            i++;
            break;
        case SIGUSR2:
            //prerreserva en SC
            printf(ANSI_COLOR_GREEN     "%d PROCESO PRERRESERVA EN SECCIÓN CRÍTICA DISTRIBUIDA"     ANSI_COLOR_RESET "\n",
                   i);
            i++;
            break;
        case SIGALRM:
            //lector en SC
            printf(ANSI_COLOR_YELLOW     "%d PROCESO GRADAS o EVENTOS EN SECCIÓN CRÍTICA DISTRIBUIDA"     ANSI_COLOR_RESET "\n",
                   i);
            i++;
            break;
        case 5:
            printf(ANSI_COLOR_RED     "%d PROCESO PAGO o ANULACIÓN SALE DE SECCIÓN CRÍTICA"     ANSI_COLOR_RESET "\n",
                   i);
            i++;
            break;
        case 6:
            printf(ANSI_COLOR_GREEN     "%d PROCESO PRERRESERVA SALE DE SECCIÓN CRÍTICA"     ANSI_COLOR_RESET "\n", i);
            i++;
            break;
        case 7:
            printf(ANSI_COLOR_YELLOW     "%d PROCESO GRADAS o EVENTOS SALE DE SECCIÓN CRÍTICA"     ANSI_COLOR_RESET "\n",
                   i);
            i++;
            break;
        default:
            //por ver
            break;
    }
}