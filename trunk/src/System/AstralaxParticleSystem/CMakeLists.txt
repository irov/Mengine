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

if( DEFINED MENGINE_ASTRALAX_PARTICLE_DLL )
    add_definitions(-DMENGE_PLUGIN_DLL)
endif()

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/astralax )

if( DEFINED MENGINE_ASTRALAX_PARTICLE_DLL )
    ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )
else()
    ADD_LIBRARY( ${MY_LIB_NAME} STATIC ${SRC_FILES} )
endif()    


TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX} )

if( MINGW )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/libmagic.a)
elseif( WIN32 )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/astralax/magic.lib)
endif( MINGW )

if( DEFINED MENGINE_ASTRALAX_PARTICLE_DLL )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Math)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Codec)

    foreach(dependency ${APPLICATION_DEPENDENCIES})
        #MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
        TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} debug ${dependency} )
        TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} optimized ${dependency} )
    endforeach(dependency ${APPLICATION_DEPENDENCIES})

    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} ${APPLICATION_LIBS} ) 
else()   
    SET( APPLICATION_DEPENDENCIES
     ${APPLICATION_DEPENDENCIES}
     ${MY_LIB_NAME}
    )

    SET( APPLICATION_DEPENDENCIES
     ${APPLICATION_DEPENDENCIES}
     PARENT_SCOPE
    )    
endif()