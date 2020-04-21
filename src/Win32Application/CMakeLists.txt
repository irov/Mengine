MENGINE_PROJECT(Win32Application)

ADD_FILTER(
MessageBoxLogger
    MessageBoxLogger.cpp
    MessageBoxLogger.h
)

if(MENGINE_TOOLCHAIN_MSVC)
ADD_FILTER(
main
    win32_main.cpp
)
endif()

if(MENGINE_TOOLCHAIN_MINGW)
ADD_FILTER(
main
    mingw32_main.cpp
)
endif()

    
ADD_FILTER(
Win32Application
    Win32Application.cpp
    Win32Application.h
)

if(MENGINE_TOOLCHAIN_MINGW)
    # resource compilation for mingw
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Win32Application_rc.o
                       COMMAND ${MINGW32_ROOT}/bin/windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
                                           -i${CMAKE_CURRENT_SOURCE_DIR}/Win32Application.rc
                                           -o ${CMAKE_CURRENT_BINARY_DIR}/Win32Application_rc.o)
    SET(SRC_FILES ${SRC_FILES} ${CMAKE_CURRENT_BINARY_DIR}/Win32Application_rc.o)
endif()

if(MENGINE_TOOLCHAIN_MSVC)
    SET(SRC_FILES ${SRC_FILES} Win32Application.rc)
endif()
    
LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

ADD_MENGINE_EXECUTABLE()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Bootstrapper)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${APPLICATION_LINK_LIBRARIES})

if(NOT MSVC)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} libole32.a)
endif()

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /NODEFAULTLIB:"libci.lib")
endif()

if(MINGW)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS -Wl,-Map="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Win32Application.map"            )
endif()

ADD_DEPENDENCIES(${PROJECT_NAME} ${APPLICATION_DEPENDENCIES})