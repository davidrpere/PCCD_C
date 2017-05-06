cmake_minimum_required (VERSION 2.6)
project (PCCD_C)
add_executable(pagos_anulaciones pagos_anulaciones.c)
add_executable(prerreservas prerreservas.c)
add_executable(registro_sc_distribuida registro_sc_distribuida.c)

set(
        CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_SOURCE_DIR}/out
)