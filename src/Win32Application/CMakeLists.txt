MENGINE_PROJECT(Win32Application)

if(MENGINE_TOOLCHAIN_MSVC)
ADD_FILTER(
main
    Win32Main.cpp
)
endif()

if(MENGINE_TOOLCHAIN_MINGW)
ADD_FILTER(
main
    MinGW32Main.cpp
)
endif()
    
ADD_FILTER(
Win32Application
    Win32Application.cpp
    Win32Application.h
    Win32ExtraFileLogger.h
    Win32ExtraFileLogger.cpp
)

if(MENGINE_TOOLCHAIN_MINGW)
    # resource compilation for mingw
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Win32Application_rc.o
                       COMMAND ${MINGW_ROOT}/bin/windres.exe -I ${CMAKE_CURRENT_SOURCE_DIR}
                                           -i ${CMAKE_CURRENT_SOURCE_DIR}/Win32Application.rc
                                           -o ${CMAKE_CURRENT_BINARY_DIR}/Win32Application_rc.o)
    
    SET(SRC_FILES ${SRC_FILES} ${CMAKE_CURRENT_BINARY_DIR}/Win32Application_rc.o)
endif()

if(MENGINE_TOOLCHAIN_MSVC)
    SET(SRC_FILES ${SRC_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/Win32Application.rc)
endif()

LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set(CMAKE_CXX_STACK_SIZE 4194304)

ADD_MENGINE_EXECUTABLE_WIN32()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_PLUGIN_MENGINE_STATIC)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Mengine)
else()
    ADD_DEPENDENCIES(${PROJECT_NAME} Mengine)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Windows)

if(MENGINE_PLUGIN_MENGINE_STATIC)
    if(APPLICATION_LINK_LIBRARIES)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${APPLICATION_LINK_LIBRARIES})
    endif()
    
    if(APPLICATION_DEPENDENCIES)
        ADD_DEPENDENCIES(${PROJECT_NAME} ${APPLICATION_DEPENDENCIES})
    endif()
endif()

if(NOT MENGINE_TOOLCHAIN_MSVC)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} libole32.a)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} atomic)
endif()

if(MENGINE_TOOLCHAIN_MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /NODEFAULTLIB:"libci.lib")
endif()

if(MINGW)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS -Wl,-Map="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Win32Application.map")
endif()

if(MENGINE_TOOLCHAIN_MSVC)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()