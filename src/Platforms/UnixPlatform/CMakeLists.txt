MENGINE_PROJECT(UnixPlatform)

if(NOT MENGINE_TARGET_UNIX)
    message(FATAL_ERROR "UnixPlatform requires MENGINE_TARGET_UNIX")
endif()

find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)

ADD_FILTER(
src
    UnixPlatformService.h
    UnixPlatformService.cpp
    UnixPlatformSystem.h
    UnixPlatformSystem.cpp
    UnixDynamicLibrary.h
    UnixDynamicLibrary.cpp
    UnixInput.h
    UnixInput.cpp
)

ADD_MENGINE_LIBRARY(Platforms)

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${X11_LIBRARIES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${CMAKE_DL_LIBS})

if(TARGET OpenGL::OpenGL)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} OpenGL::OpenGL)
elseif(TARGET OpenGL::GL)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} OpenGL::GL)
else()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OPENGL_LIBRARIES})
endif()

if(TARGET OpenGL::GLX)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} OpenGL::GLX)
endif()
