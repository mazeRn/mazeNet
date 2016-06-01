cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -Wextra -Weffc++ ")
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS}")

project ( network )
set( CMAKE_BUILD_TYPE Release )

add_library( network STATIC TcpServer.cpp TcpClient.cpp TcpConnection.cpp TcpMessage.cpp )
