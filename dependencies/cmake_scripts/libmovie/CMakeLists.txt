CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(movie)

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libmovie )

# OUR SOURCES
set( sources
${SOURCE_DIR}/src/movie_utils.c
${SOURCE_DIR}/src/movie_math.c
${SOURCE_DIR}/src/movie_node.c
${SOURCE_DIR}/src/movie_math.h
${SOURCE_DIR}/src/movie_data.c
${SOURCE_DIR}/src/movie_utils.h
${SOURCE_DIR}/src/movie_memory.h
${SOURCE_DIR}/src/movie_instance.c
${SOURCE_DIR}/src/movie_stream.h
)

include_directories(${SOURCE_DIR}/include)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )

