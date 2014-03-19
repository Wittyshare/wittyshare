cmake_minimum_required(VERSION 2.8.3)
SET(APPNAME "WsModTwitterStream")

SET ( APPNAME_SRCS 
     src/WsModTwitterStream.cpp
    )

INCLUDE(${CMAKE_SOURCE_DIR}/src/WsModule/cmake/WsModuleInclude.cmake)

