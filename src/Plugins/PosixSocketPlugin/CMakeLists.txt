set(MY_LIB_NAME PosixSocketPlugin)

ADD_FILTER(
src
	PosixSocket.cpp
	PosixSocket.h
    PosixSocketProviderInterface.h
	PosixSocketInputStream.cpp
	PosixSocketInputStream.h
	PosixSocketOutputStream.cpp
	PosixSocketOutputStream.h
	PosixSocketSystem.cpp
	PosixSocketSystem.h

    PosixSocketPlugin.cpp
    PosixSocketPlugin.h
	PosixSocketPlugin.def
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_POSIX_SOCKET)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})