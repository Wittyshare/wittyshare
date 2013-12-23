cmake_minimum_required(VERSION 2.8.3)
SET(APPNAME WsModule)
SET(CMAKE_INSTALL_PREFIX  /usr/local/wittyshare)

# Cette librairie contient uniquement des fonctions sans interfaces.

SET ( APPNAME_SRCS 
     src/WsModule.cpp
    )

IF(NOT GDWTCORE_FOUND)
    INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/Find_gdwtcore.cmake)
ENDIF(NOT GDWTCORE_FOUND)

IF(NOT WSCORE_FOUND)
    INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/Find_wscore.cmake)
ENDIF(NOT WSCORE_FOUND)

INCLUDE_DIRECTORIES(
    ../
    src
    ${GDWTCORE_INCLUDE_DIRS}
    ${WSCORE_INCLUDE_DIRS}
)

INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/install_libs.cmake)

