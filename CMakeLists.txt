cmake_minimum_required(VERSION 2.8.3)
PROJECT(wittyshare)

# pour support de std::tr1
# http://www.johndcook.com/cpp_regex.html
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=gnu++11")
#set(CMAKE_CSS_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=gnu++11")
SET(APPNAME wittyshare)
SET(APPBIN "${APPNAME}.wt ${APPNAME}.wthttpd")
SET(DEPLOYROOT /var/www/wt CACHE PATH "Deploy the app under this directory")

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${HARU_PREFIX} ${GDCORE_PREFIX} ${GDWTCORE_PREFIX} ${WSCORE_PREFIX})

ADD_DEFINITIONS(-DHAS_DAEMON)

SET (APPNAME_SRCS
    src/Main/main.cpp
    src/Main/WsFormConfig.cpp
    src/Main/WsLogo.cpp
    src/Main/WsSiteMap.cpp
    src/Main/WsSearchObject.cpp
    src/Main/WsTopBanner.cpp
    src/Main/WsBottomBanner.cpp
    src/Main/WsContent.cpp
    src/Main/WsContentButtonsBar.cpp
    src/Main/WsFileResource.cpp
    src/Main/WsModulesLoader.cpp 
    src/Main/WsFunctionnalities.cpp
    src/Main/WsSiteMapView.cpp
    src/Main/WsSearchView.cpp
    src/Main/WsTemplate.cpp
    src/Main/WsApplication.cpp
    src/Main/WsOption.cpp
    src/Main/WsErrorPage.cpp
    )

INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/Find_gdwtcore.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/Find_wscore.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/src/WsModule/cmake/WsModuleInclude.cmake)

IF(NOT GDWTCORE_FOUND)
    MESSAGE(FATAL_ERROR "** these program needs gdwtcore libraries... Abord.")
ENDIF(NOT GDWTCORE_FOUND)

IF(NOT WSCORE_FOUND)
    MESSAGE(FATAL_ERROR "** these program needs wscore libraries... Abord.")
ENDIF(NOT WSCORE_FOUND)


INCLUDE_DIRECTORIES(
    ${GDWTCORE_INCLUDE_DIRS}
    ${WSCORE_INCLUDE_DIRS}
    src
    )

SET ( APP_LIBRARIES
    dl
    ${GDWTCORE_LIBRARIES}
    ${WSCORE_LIBRARIES}
    )

SET ( APP_LIBRARIES
    ${APP_LIBRARIES}
    WsModule
    )

SUBDIRS (
    src/WsModule
    src/WsModules
    src/FhtmlEditor
    )

INCLUDE(${CMAKE_ROOT}/Modules/gdcmake/install_webapp.cmake)

