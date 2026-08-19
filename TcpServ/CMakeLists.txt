cmake_minimum_required(VERSION 2.8.11...3.30)
set(PROJECT_NAME "TcpServ")

project(${PROJECT_NAME} C)

message(STATUS "PROJECT_NAME =  ${PROJECT_NAME}")
message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")

set(CMAKE_C_STANDARD 11)

set(FUNCS_DEPS_LIST "ERROR_func.c;SET_ADDRESS_func.c;READ_N_func.c;skel_defines.h;")

message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")

set(SKELETONS_DIR "/home/vildstein/Prog/SomeSketches/Some_Nets/Skeletons/")
message(STATUS "This is KOSTbIL TO INCLUDE ${SKELETONS_DIR}")
#include("/home/vildstein/Prog/SomeSketches/Some_Nets/${skel}")
#include_directories("/home/vildstein/Prog/SomeSketches/Some_Nets/Skeletons/")

add_executable(${PROJECT_NAME}
    main.c
#    ${FUNCS_DEPS_LIST}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE OS_DEFINED_CMAKE_LIN)
target_include_directories(${PROJECT_NAME} PUBLIC "${SKELETONS_DIR}")



#if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#    target_compile_definitions(${CLIENT_APP_NAME} PRIVATE OS_DEFINED_CMAKE_LIN)
#    message(STATUS "LINUX DEFINED")
#else ()
#    target_compile_definitions(${CLIENT_APP_NAME} PRIVATE OS_DEFINED_CMAKE_WIN)
#    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
#    message(STATUS "DEFINED NOT LINUX")
#endif()
