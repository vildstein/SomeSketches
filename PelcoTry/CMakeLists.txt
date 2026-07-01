cmake_minimum_required(VERSION 2.8.11...3.20)
set(PROJECT_NAME "PelcoClient")

project(${PROJECT_NAME} C)

message(STATUS "PROJECT_NAME =  ${PROJECT_NAME}")
message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")

set(CMAKE_C_STANDARD 11)

set(UDP_CLIENT_APP_NAME "${PROJECT_NAME}_UDP_client.exe")

set(FUNCS_DEPS_LIST "ERROR_func.c;SET_ADDRESS_func.c;READ_N_func.c;skel_defines.h;")

foreach (VAR IN LISTS exList ITEMS)
    message(STATUS "VAR_IN_FOREACH =  ${VAR}")
endforeach()

add_executable(${UDP_CLIENT_APP_NAME}
	PelcoClient.c
	upd_functions.c
        ${FUNCS_DEPS_LIST}
)
target_link_libraries(${UDP_CLIENT_APP_NAME} PRIVATE curses)
target_compile_definitions(${UDP_CLIENT_APP_NAME} PRIVATE OS_DEFINED_CMAKE_LIN)
message(STATUS "LINUX DEFINED")


#if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#    target_compile_definitions(${CLIENT_APP_NAME} PRIVATE OS_DEFINED_CMAKE_LIN)
#    message(STATUS "LINUX DEFINED")
#else ()
#    target_compile_definitions(${CLIENT_APP_NAME} PRIVATE OS_DEFINED_CMAKE_WIN)
#    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
#    message(STATUS "DEFINED NOT LINUX")
#endif()
