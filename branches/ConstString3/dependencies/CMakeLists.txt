
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

PROJECT( MengeDependencies )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#INCLUDE( cmake_scripts/menge_macro.cmake )

ADD_SUBDIRECTORY( cmake_scripts )

#set(BUILD_SHARED_LIBS OFF)

#ADD_SUBDIRECTORY(zlib)

#set(ZLIB_LIBRARY ${CMAKE_BINARY_DIR})
#set(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/zlib
#                     ${CMAKE_BINARY_DIR}/zlib
#                     )

#set(PNG_SHARED OFF)
#set(PNG_STATIC ON)
                   
#ADD_SUBDIRECTORY( libPNG )


#set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/OpenAL)
#SET(LIBTYPE STATIC)
#ADD_SUBDIRECTORY( OpenAL )