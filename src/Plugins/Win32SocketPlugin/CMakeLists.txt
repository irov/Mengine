MENGINE_PROJECT(Win32SocketPlugin)

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

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_WIN32_SOCKET)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX} )

if(MINGW)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Ws2_32.lib)
elseif(WIN32)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Ws2_32.lib)
endif()