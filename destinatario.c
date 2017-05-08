#include "pccd.c"

int main(int argc, char* argv[]){
    int emisor, prioridad, ticket;
    recibir_mensaje(1, 22, &emisor, &ticket, &prioridad);
    printf("El mensaje recibido es de %i, que es de tipo %i, y contiene %i\n", emisor, prioridad, ticket);
    return 0;
}