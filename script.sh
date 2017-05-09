#!/bin/bash
#Comprobacion parametros pasados al script.
if [ "$#" -lt 2 ] || [ "$#" -gt 3 ]; then
  echo -e "\n\nNECESARIO EJECUCION DEL SCRIPT CON SUDO Y ENCONTRARSE DENTRO DE LA CARPETA PCCD_C."
  echo -e "NECESARIO CREAR UNA NUEVA TERMINA CON EL NOMBRE nuevaTerminal
[Edit -> Preferences -> Profiles -> New].\n\n"
  echo -e "Modo de uso: sudo ./script.sh nodo numero_de_nodos [compilar]\n"
  echo "-> nodo: numero del nodo que se va a ejecutar."
  echo "-> numero_de_nodos: numero de nodos que se van a ejecutar (minimo 2)."
  echo -e "-> [compilar]: parametro opcional que compila los procesos:
   inicializacion.c, lectores.c, pagos_anulaciones.c, prerreservas.c receptor.c.\n\n"
  exit 1
fi

#Guardado de los parametros pasados al script.
args=("$@")

#Declaracion de variables globales:
FIN=0

if [ "$#" -eq 3 ]; then
  if [ ${args[2]} = "compilar" ]; then
    ipcrm -a

    gcc -Wall inicializacion.c -o inicializacion -lpthread
    gcc -Wall lectores.c -o lectores -lpthread
    gcc -Wall pagos_anulaciones.c -o pagos_anulaciones -lpthread
    gcc -Wall prerreservas.c -o prerreservas -lpthread
    gcc -Wall receptor.c -o receptor -lpthread
  else
    echo "parametro [compilar] no correcto, escriba la palabra compilar."
    exit 1
  fi
fi

#inicializacion de la memoria compartida del nodo.
./inicializacion ${args[0]}

#Ejecucion del receptor del nodo (en nueva terminal).
gnome-terminal -e "./receptor ${args[0]} ${args[1]}"  --window-with-profile=nuevaTerminal

#Funciones:
function gradasEventos() {
  #Ejecucion de proceso lectores (en nueva terminal).
  gnome-terminal -e "./lectores ${args[0]} ${args[1]}"  --window-with-profile=nuevaTerminal
}

function prerreservas() {
  #Ejecucion de proceso prerreservas (en nueva terminal).
  gnome-terminal -e "./prerreservas ${args[0]} ${args[1]}"  --window-with-profile=nuevaTerminal
}

function pagosAnulaciones() {
  #Ejecucion de proceso pagosAnulaciones (en nueva terminal).
  gnome-terminal -e "./pagos_anulaciones ${args[0]} ${args[1]}"  --window-with-profile=nuevaTerminal
}

while [[ $FIN -eq 0 ]]; do
  echo -e "\n\nNodo: ${args[0]}, numero de vecinos: ${args[1]}.\n"
  #Opciones de ejecucion:
  echo -e "¿Que proceso/s desea lanzar a continuacion? (0 para salir)\n"
  echo -e "1. Ejecutar procesos de garadas o eventos."
  echo -e "2. Ejecutar procesos prerreservas."
  echo -e "3. Ejecutar procesos pagos o anulaciones.\n\n"
  read case;

  case $case in
    0)
      echo -e "\nScript cerrado"
      FIN=1
      ;;
    1)
      echo -e "Has seleccionado la opcion de lanzar procesos garada/s o evento/s."
      gradasEventos
      ;;
    2)
      echo -e "Has seleccionado la opcion de lanzar procesos prerreserva/s."
      prerreservas
      ;;
    3)
      echo -e "Has seleccionado la opcion de lanzar procesos pago/s o anulacion/es."
      pagosAnulaciones
      ;;
    *)
      echo -e "Opcion invalida."
  esac
done