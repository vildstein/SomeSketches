cmake_minimum_required(VERSION 2.8.11...3.20)
set(PROJECT_NAME "AnglesTest")
project(${PROJECT_NAME})

# Enables testing for this directory and below
# https://cmake.org/cmake/help/v3.5/command/enable_testing.html
enable_testing() #CMAKE


# replace XX with the version you have
# https://www.boost.org/doc/libs/latest/libs/test/doc/html/boost_test/section_faq.html
set(Boost_ADDITIONAL_VERSIONS "1.87" "1.87.0")
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

# finds boost, triggers an error otherwise
find_package(Boost 1.87 REQUIRED COMPONENTS unit_test_framework)

# indicates the location of the boost installation tree.
# hard-coded for our simple example.
# https://www.boost.org/doc/libs/latest/libs/test/doc/html/boost_test/section_faq.html
#set(BOOST_INCLUDE_DIRS "/usr/include/boost")

message(STATUS ${BOOST_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} angleTest.cpp)

# indicates the include paths
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_INCLUDE_DIRS})


# indicates the shared library variant
target_compile_definitions(${PROJECT_NAME} PRIVATE "BOOST_TEST_DYN_LINK=1")
# indicates the link paths
target_link_libraries(${PROJECT_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

# declares a test with our executable
#add_test(NAME test1 COMMAND test_executable)