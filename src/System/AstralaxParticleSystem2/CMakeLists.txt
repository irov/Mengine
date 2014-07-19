set( MY_LIB_NAME AstralaxParticleSystem2 )

ADD_FILTER(
src
	AstralaxEmitter2.cpp
	AstralaxEmitter2.h
	AstralaxEmitterContainer2.cpp
	AstralaxEmitterContainer2.h
	AstralaxParticleSystem2.cpp
	AstralaxParticleSystem2.h
    
    AstralaxParticlePlugin2.cpp
    AstralaxParticlePlugin2.h
    AstralaxParticlePlugin2.def
)

add_definitions(-DMENGE_PLUGIN_DLL)
add_definitions(-DMAGIC_3D)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/astralax )

ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Math)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Codec)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX} )

if( MINGW )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/win/lib/mingw/libmagic3d.a)
elseif( WIN32 )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/win/lib/microsoft/magic3d.lib)
elseif( APPLE )    
    IF(${MENGINE_TARGET} STREQUAL "IPAD")
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${magic_lib_path}/libimagic3d.a)
    ENDIF(${MENGINE_TARGET} STREQUAL "IPAD")
    
    IF(${MENGINE_TARGET} STREQUAL "IPAD_SIMULATOR")
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${magic_lib_path}/libimagic3d_simulator.a)
    ENDIF(${MENGINE_TARGET} STREQUAL "IPAD_SIMULATOR")
endif( MINGW )