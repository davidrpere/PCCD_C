#include "pccd.c"

int main(int argc, char* argv[]){

    key_t clave_buzon = generar_clave("pccd.c", 1);
    obtener_buzon(clave_buzon, IPC_CREAT);

    enviar_mensaje(1, 1, 4, 12, 2);

    return 0;

}

