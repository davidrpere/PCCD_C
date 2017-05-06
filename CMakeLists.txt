cmake_minimum_required (VERSION 2.6)
project (PCCD_C)
add_executable(memoria_compartida memoria_compartida.c)
add_executable(pago_anulacion pago_anulacion.c)
add_executable(prerreserva prerreserva.c)
add_executable(receptor receptor.c)

set(
        CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_SOURCE_DIR}/out
)