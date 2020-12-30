cmake_minimum_required (VERSION 2.8)

project ("socket")

add_definitions(-std=c++11)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
#	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")
#	add_definitions(-pthread)
	SET(COMMON_LIBS pthread)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	MESSAGE(STATUS "current platform: FreeBSD")
ELSE ()
	MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

set(BUILD_OUTPUT ${CMAKE_BINARY_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_OUTPUT})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_OUTPUT})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_OUTPUT})
message("sock:" ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
include_directories(.)
add_library(${PROJECT_NAME} SHARED socket.h socket.cpp)

option(BUILD_DEMO " «∑Ò±‡“Îdemo " ON)
IF(BUILD_DEMO)
#	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
#		COMMAND copy ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/* ./demo )
	add_subdirectory(demo)
ENDIF()
#add_executable(echo_server "demo/echo_server.cpp"  )
#add_executable(sock_send "demo/sock_send.cpp" )

#target_link_libraries(echo_server ${PROJECT_NAME} ${COMMON_LIBS})
#target_link_libraries(sock_send ${PROJECT_NAME} ${COMMON_LIBS})
