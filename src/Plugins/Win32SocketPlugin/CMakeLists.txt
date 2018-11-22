set( MY_LIB_NAME Win32SocketPlugin )

ADD_FILTER(
src
	Win32Socket.cpp
	Win32Socket.h
    Win32SocketProviderInterface.h
	Win32SocketInputStream.cpp
	Win32SocketInputStream.h
	Win32SocketOutputStream.cpp
	Win32SocketOutputStream.h
	Win32SocketSystem.cpp
	Win32SocketSystem.h

    Win32SocketPlugin.cpp
    Win32SocketPlugin.h
	Win32SocketPlugin.def
)

#if( MENGINE_PLUGIN_WIN32_SOCKET_DLL )
    add_definitions(-DMENGINE_PLUGIN_DLL)
#endif()

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/stdex/include )

#if( MENGINE_PLUGIN_WIN32_SOCKET_DLL )
#    ADD_MENGINE_SHARED()
#else()
    ADD_MENGINE_LIBRARY()
#endif()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX} )

if( MINGW )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Ws2_32.lib)
elseif( WIN32 )
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Ws2_32.lib)
endif()

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
 Kernel
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
