cmake_minimum_required(VERSION 2.8)

project(WAP64)
set(EXECUTABLE_NAME ${PROJECT_NAME})

set(SOURCES
    main.cpp
    res.h
    res.cpp
)

add_definitions(-D_USE_MATH_DEFINES)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics)

include_directories(${SFML_INCLUDE_DIR})
add_executable(${EXECUTABLE_NAME} ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
add_custom_command(TARGET ${EXECUTABLE_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/DATA $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>)
