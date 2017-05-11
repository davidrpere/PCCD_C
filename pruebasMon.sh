#!/bin/bash
#Comprobacion parametros pasados al script.
if [ "$#" -ne 2 ]; then
  echo -e "\n\nNECESARIO EJECUCION CON SUDO Y ENCONTRARSE DENTRO DE LA CARPETA PCCD_C."
  echo -e "NECESARIO CREAR UNA NUEVA TERMINA CON EL NOMBRE nuevaTerminal
[Edit -> Preferences -> Profiles -> New].\n\n"
  echo -e "Modo de uso: sudo ./pruebas.sh numero_de_nodos \n"
  echo "-> numero_de_nodos: numero de nodos que se van a ejecutar (minimo 2)."
  exit 1
fi

#Guardado de los parametros pasados al script.
args=("$@")

#Declaracion de variables globales:
FIN=0

#Borrado de colas de mensajes, semaforos y memoria compartida.
ipcrm -a

#Compilacion de archivos necesarios
gcc -Wall inicializacion.c -o inicializacion -lpthread
gcc -Wall lectores.c -o lectores -lpthread
gcc -Wall pagos_anulaciones.c -o pagos_anulaciones -lpthread
gcc -Wall prerreservas.c -o prerreservas -lpthread
gcc -Wall receptor.c -o receptor -lpthread
gcc -Wall registro_sc_local.c -o registro_sc_local
gcc -Wall registro_sc_distribuida.c -o registro_sc_distribuida

#Nuevas terminales con los procesos de monitorizacion:
gnome-terminal -e "./registro_sc_distribuida"  --window-with-profile=nuevaTerminal --geometry=60x20
for (( i = 1; i <= ${args[0]}; i++ )); do
  gnome-terminal -e "./registro_sc_local $i"  --window-with-profile=nuevaTerminal --geometry=60x20
done
