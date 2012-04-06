
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( simpleini )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../simpleini )

# OUR SOURCES
set( sources
${SOURCE_DIR}/SimpleIni.h
${SOURCE_DIR}/ConvertUTF.c
${SOURCE_DIR}/ConvertUTF.h
)

add_definitions(-D_UNICODE)
add_definitions(-DSI_CONVERT_GENERIC)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
