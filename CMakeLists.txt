cmake_minimum_required (VERSION 2.6)
project (PCCD_C)

find_package (Threads)

add_executable(pagos_anulaciones pagos_anulaciones.c)
add_executable(prerreservas prerreservas.c)
add_executable(receptor receptor.c)
add_executable(registro_sc_distribuida registro_sc_local.c)
add_executable(inicializacion inicializacion.c)
add_executable(lectores lectores.c)
target_link_libraries (inicializacion ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (prerreservas ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (pagos_anulaciones ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (receptor ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (registro_sc_distribuida ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (lectores ${CMAKE_THREAD_LIBS_INIT})


add_library(pccd pccd.c)

include_directories("${PROJECT_BINARY_DIR}")


set(
        CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_SOURCE_DIR}/out
)