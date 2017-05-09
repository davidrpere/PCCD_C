#!/bin/bash
gcc -Wall inicializacion.c -o inicializacion -lpthread
gcc -Wall lectores.c -o lectores -lpthread
gcc -Wall pagos_anulaciones.c -o pagos_anulaciones -lpthread
gcc -Wall prerreservas.c -o prerreservas -lpthread
gcc -Wall receptor.c -o receptor -lpthread
gcc -Wall registro_sc_local.c -o registro_sc_local
gcc -Wall registro_sc_distribuida.c -o registro_sc_distribuida