set(MY_LIB_NAME Win32UnicodeSystem)

ADD_FILTER(
src
	Win32UnicodeSystem.cpp
	Win32UnicodeSystem.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Systems
)
