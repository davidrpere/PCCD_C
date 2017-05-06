#!/bin/bash
gcc -Wall inicializacion.c -o inicializacion -lpthread
gcc -Wall lectores.c -o lectores -lpthread
gcc -Wall pagos_anulaciones.c -o pagos_anulaciones -lpthread
gcc -Wall prerreservas.c -o prerreservas -lpthread
