cmake_minimum_required (VERSION 2.8)

project ("socket")

add_definitions(-std=c++11)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	MESSAGE(STATUS "current platform: Linux ")
#	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")
	add_definitions(-pthread)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	MESSAGE(STATUS "current platform: Windows")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	MESSAGE(STATUS "current platform: FreeBSD")
ELSE ()
	MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

include_directories(.)

add_executable(echo_server "demo/echo_server.cpp" socket.h socket.cpp )
add_executable(sock_send "demo/sock_send.cpp" socket.h socket.cpp )

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(echo_server pthread)
	target_link_libraries(echo_server pthread)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")