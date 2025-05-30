MENGINE_PROJECT(SDL2SocketPlugin)

ADD_FILTER(
src
    SDL2Socket.h
    SDL2Socket.cpp
    SDL2SocketProviderInterface.h
    SDL2SocketInputStream.cpp
    SDL2SocketInputStream.h
    SDL2SocketOutputStream.cpp
    SDL2SocketOutputStream.h
    SDL2SocketSystem.h
    SDL2SocketSystem.cpp
    
    SDL2SocketPlugin.cpp
    SDL2SocketPlugin.h
    SDL2SocketPlugin.def
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include/include-config-$<LOWER_CASE:$<CONFIG>>)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL2)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2_net)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_SDL2_SOCKET)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX} )

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}SDL2_net-static${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})

if(MINGW)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Ws2_32.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Iphlpapi.lib)
elseif(WIN32)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Ws2_32.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Iphlpapi.lib)
endif()