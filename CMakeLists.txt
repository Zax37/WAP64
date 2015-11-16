cmake_minimum_required(VERSION 3.0)

project(WAP64)
set(EXECUTABLE_NAME ${PROJECT_NAME})

add_subdirectory(libwap32)

set(SOURCES
    src/main.cpp
    src/res.h
    src/res.cpp
	src/common.h
	src/common.cpp
	src/cMachine.h
	src/cMachine.cpp
	src/cMenu.cpp
	src/cPlayLevel.cpp
)

#set( SFGUI_STATIC_LIBRARIES true CACHE BOOL "Do you want to link SFGUI statically?" )
#set( SFML_STATIC_LIBRARIES true CACHE BOOL "Do you want to link SFML statically?" )
#set( SFML_ROOT D:/Projects/dist/SFML CACHE PATH "SFML root dir" )
#set( SFGUI_ROOT D:/Projects/dist/SFGUI CACHE PATH "SFGUI root dir" )

add_definitions(-D_USE_MATH_DEFINES)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})
find_package( OpenGL REQUIRED )
find_package( SFML 2.3 REQUIRED COMPONENTS graphics window system )
find_package( SFGUI REQUIRED )

add_definitions(-DSFML_STATIC)
add_definitions(-DSFGUI_STATIC)

include_directories(${SFML_INCLUDE_DIR} ${SFGUI_INCLUDE_DIR} ${libwap32_INCLUDE_DIRS})
add_executable(${EXECUTABLE_NAME} ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} ${SFGUI_LIBRARY} ${SFML_LIBRARIES} ${SFML_DEPENDENCIES} ${OPENGL_gl_LIBRARY} wap32)

add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy
					libwap32/libwap32.dll libwap32.dll)				
					
#add_custom_command(TARGET ${EXECUTABLE_NAME} PRE_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy_directory
#                   ${CMAKE_SOURCE_DIR}/DATA $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>)
