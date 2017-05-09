#!/bin/bash
#Comprobacion parametros pasados al script.
if [ "$#" -ne 2 ]; then
  echo -e "\n\nNECESARIO EJECUCION CON SUDO Y ENCONTRARSE DENTRO DE LA CARPETA PCCD_C."
  echo -e "NECESARIO CREAR UNA NUEVA TERMINA CON EL NOMBRE nuevaTerminal
[Edit -> Preferences -> Profiles -> New].\n\n"
  echo -e "Modo de uso: sudo ./script.sh numero_de_nodos numero_de_procesos_por_tipo\n"
  echo "-> numero_de_nodos: numero de nodos que se van a ejecutar (minimo 2)."
  echo -e "-> numero_de_procesos_por_tipo: numero de procesos que se van a ejecutar
     por cada tipo de proceso (gradas_eventos, prerreservas, pagos_anulaciones)
     en cada nodo.\n\n"
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

#Guardar PID registro_sc_distribuida
file="pid_sc_distribuida.txt"
read -d $'\x04' pid_sc_distribuida < "$file"

#Funcion para inicializar todos los procesos de un nodo.
function inicializarProcesosPorNodo() {
  local pid_sc_local
  file="pid_sc_local$1.txt"
  read -d $'\x04' pid_sc_local < "$file"

  case $1 in
    1)
    gnome-terminal -e "./prerreservas $1 ${args[0]} $pid_sc_local $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./lectores $1 ${args[0]} $pid_sc_local $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./pagos_anulaciones $1 ${args[0]} $pid_sc_local $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    ;;
    2)
    gnome-terminal -e "./prerreservas $1 ${args[0]} $pid_sc_local $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./pagos_anulaciones $1 ${args[0]} $pid_sc_local $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./lectores $1 ${args[0]} $pid_sc_local $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    ;;
    3)
    gnome-terminal -e "./pagos_anulaciones $1 ${args[0]} $pid_sc_local $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./prerreservas $1 ${args[0]} $pid_sc_local $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./lectores $1 ${args[0]} $pid_sc_local $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    ;;
    *)
    gnome-terminal -e "./prerreservas $1 ${args[0]} $pid_sc_local $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./lectores $1 ${args[0]} $pid_sc_local $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./pagos_anulaciones $1 ${args[0]} $pid_sc_local $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
  esac
}

#Funcion para inicializar en un orden configurable los procesos (necesario 3 nodos)
function inicializarProcesosConfigurable() {
  local pid_sc_local1
  file="pid_sc_local1.txt"
  read -d $'\x04' pid_sc_local1 < "$file"

  local pid_sc_local2
  file="pid_sc_local2.txt"
  read -d $'\x04' pid_sc_local2 < "$file"

  local pid_sc_local3
  file="pid_sc_local3.txt"
  read -d $'\x04' pid_sc_local3 < "$file"

  case $1 in
    1) #Inicializamos para cada nodo lectores
    gnome-terminal -e "./lectores 1 ${args[0]} $pid_sc_local1 $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./lectores 2 ${args[0]} $pid_sc_local2 $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./lectores 3 ${args[0]} $pid_sc_local3 $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    ;;
    2)
    gnome-terminal -e "./prerreservas 1 ${args[0]} $pid_sc_local1 $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./pagos_anulaciones 2 ${args[0]} $pid_sc_local2 $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./lectores 3 ${args[0]} $pid_sc_local3 $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    ;;
    3)
    gnome-terminal -e "./pagos_anulaciones 1 ${args[0]} $pid_sc_local1 $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./prerreservas 2 ${args[0]} $pid_sc_local2 $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
    gnome-terminal -e "./lectores 3 ${args[0]} $pid_sc_local3 $pid_sc_distribuida ${args[1]}"  --window-with-profile=nuevaTerminal --geometry=60x20
  esac
}

#Punto de control 1.
echo -e "\n\nA continuacion se inicializara la memoria compartida. Pulse ENTER para continuar con el script."
read aux
clear

#inicializacion de la memoria compartida del los nodos.
for (( i = 1; i <= ${args[0]}; i++ )); do
  ./inicializacion $i
done

#Punto de control 2.
echo -e "\n\nA continuacion se inicializaran los receptores. Pulse ENTER para continuar con el script."
read aux
clear

#inicializacion de los receptores de cada nodo.
for (( i = 1; i <= ${args[0]}; i++ )); do
  #./receptor $i ${args[0]}
  gnome-terminal -e "./receptor $i ${args[0]}"  --window-with-profile=nuevaTerminal --geometry=60x20
done

#Ejecucion de procesos lectores, prerreservas y pagos_anulaciones.
echo -e "\n\nSe va a arrancar la prueba. Pulse ENTER para continuar con el script."
read aux
for (( i = 1; i <= ${args[0]}; i++ )); do
  inicializarProcesosPorNodo "$i"
  #inicializarProcesosConfigurable "$i"
done

clear
