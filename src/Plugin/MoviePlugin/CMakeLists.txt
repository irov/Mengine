set( MY_LIB_NAME MoviePlugin )

ADD_FILTER(
src
	MoviePlugin.h
	MoviePlugin.cpp   
    MoviePlugin.def
    
    Movie2.cpp
    Movie2.h
    ResourceMovie2.cpp
    ResourceMovie2.h
)

add_definitions(-DMENGE_PLUGIN_DLL)

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils
        ${THIRDPARTY_DIR}/libmovie/include
        ${THIRDPARTY_DIR}/pybind/include
        ${THIRDPARTY_DIR}/metabuf
)

ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Math )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Metacode )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel )
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}movie_static${MENGINE_LIB_SUFFIX})