//
// Created by juan on 9/05/17.
//
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    int cantidad = atoi(argv[1]);
    int pid = atoi(argv[2]);
    int i;
    for(i=0;i<cantidad; i++){
        kill(pid, 5);
        sleep(1);
    }
    return 0;
}