cmake_minimum_required(VERSION 2.8.11...3.30)
set(PROJECT_NAME "Sendf")

project(${PROJECT_NAME} C)

message(STATUS "PROJECT_NAME =  ${PROJECT_NAME}")
message(STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")

set(CMAKE_C_STANDARD 11)


set(SKELETONS_DIR "/home/vildstein/Prog/SomeSketches/Some_Nets/Skeletons/")
message(STATUS "This is KOSTbIL TO INCLUDE ${SKELETONS_DIR}")

add_executable(${PROJECT_NAME}
    main.c parseArgs.c
)

target_compile_definitions(${PROJECT_NAME} PRIVATE OS_DEFINED_CMAKE_LIN)
target_include_directories(${PROJECT_NAME} PUBLIC "${SKELETONS_DIR}")

target_link_libraries(${PROJECT_NAME} PRIVATE
    pthread
)
