cmake_minimum_required(VERSION 2.8.3)
SET(APPNAME "fhtmlEditor")
PROJECT($APPNAME)
SET(APPBIN "${APPNAME}.wt ${APPNAME}.wthttpd")
SET(DEPLOYROOT /var/www/wt/wittyshare)

SET (APPNAME_SRCS
 src/main.cpp
)

IF(NOT GDWTCORE_FOUND)
   INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/Find_gdwtcore.cmake)
ENDIF(NOT GDWTCORE_FOUND)

IF(NOT WSCORE_FOUND)
   INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/Find_wscore.cmake)
ENDIF(NOT WSCORE_FOUND)


INCLUDE_DIRECTORIES(
  ${GDWT_INCLUDE_DIRS}
  ${WS_INCLUDE_DIRS}
)

SET ( APP_LIBRARIES
  ${GDWTCORE_LIBRARIES}
  ${WSCORE_LIBRARIES}
  )

INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/install_webapp.cmake)
