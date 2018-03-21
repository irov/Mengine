set( MY_LIB_NAME Win32SocketPlugin )

ADD_FILTER(
src
	Win32Socket.cpp
	Win32Socket.h
	Win32SocketSystem.cpp
	Win32SocketSystem.h
    
    Win32SocketPlugin.cpp
    Win32SocketPlugin.h    
)

if( MENGINE_WIN32_SOCKET_DLL )
    ADD_FILTER(src Win32SocketPlugin.def)
    
    add_definitions(-DMENGINE_PLUGIN_DLL)    
endif()

INCLUDE_DIRECTORIES( 
    ${THIRDPARTY_DIR}/stdex/include
    )

if( MENGINE_ASTRALAX_PARTICLE2_DLL )
    ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )
else()
    ADD_LIBRARY( ${MY_LIB_NAME} STATIC ${SRC_FILES} )
endif()    

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX} )

if( MINGW )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Ws2_32.lib)
elseif( WIN32 )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Ws2_32.lib)
endif( MINGW )

if( MENGINE_ASTRALAX_PARTICLE2_DLL )    
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory)

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

