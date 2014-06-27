set( MY_LIB_NAME AstralaxParticleSystem )

ADD_FILTER(
src
	AstralaxEmitter.cpp
	AstralaxEmitter.h
	AstralaxEmitterContainer.cpp
	AstralaxEmitterContainer.h
	AstralaxParticleSystem.cpp
	AstralaxParticleSystem.h
    
    AstralaxParticlePlugin.cpp
    AstralaxParticlePlugin.h
    AstralaxParticlePlugin.def
)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/astralax )

ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Codec)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX} )

if( MINGW )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/win/lib/mingw/libmagic.a)
elseif( WIN32 )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/win/lib/microsoft/magic.lib)
elseif( APPLE )    
    IF(${MENGINE_TARGET} STREQUAL "IPAD")
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${magic_lib_path}/libimagic.a)
    ENDIF(${MENGINE_TARGET} STREQUAL "IPAD")
    
    IF(${MENGINE_TARGET} STREQUAL "IPAD_SIMULATOR")
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${magic_lib_path}/libimagic_simulator.a)
    ENDIF(${MENGINE_TARGET} STREQUAL "IPAD_SIMULATOR")
endif( MINGW )