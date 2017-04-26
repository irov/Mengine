set( MY_LIB_NAME AstralaxParticlePlugin )

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

if( MENGINE_PLUGIN_ASTRALAX_DLL )
    add_definitions(-DMENGE_PLUGIN_DLL)
endif()

add_definitions(-DMAGIC_3D)

INCLUDE_DIRECTORIES( 
    ${THIRDPARTY_DIR}/astralax 
    ${THIRDPARTY_DIR}/pybind/include
    )

if( MENGINE_PLUGIN_ASTRALAX_DLL )
    ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )
else()
    ADD_LIBRARY( ${MY_LIB_NAME} STATIC ${SRC_FILES} )
endif()    

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX} )
#TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}astralax${MENGINE_LIB_SUFFIX} )

if( MINGW )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/libmagic3d.a)
elseif( WIN32 )
    if( MENGINE_PLATFORM_WIN32 )
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/magic3d.lib)
    endif()
    
    if( MENGINE_PLATFORM_WIN64 )
        TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/magic3d_x64.lib)
    endif()
endif( MINGW )

if( MENGINE_PLUGIN_ASTRALAX_DLL )    
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Math)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Codec)  
else()   
    SET( APPLICATION_DEPENDENCIES
     ${APPLICATION_DEPENDENCIES}
     ${MY_LIB_NAME}
    )

    SET( APPLICATION_DEPENDENCIES
     ${APPLICATION_DEPENDENCIES}
     PARENT_SCOPE
    )    
    
    SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    Logger
    Core
    Codec
    Archive
    Factory
    Kernel
    )

    SET( APPLICATION_LIBS
     ${APPLICATION_LIBS}
     PARENT_SCOPE
    )
endif()

