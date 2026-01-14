MENGINE_PROJECT(GDKPlatform)

ADD_FILTER(
src
    GDKPlatformService.h
    GDKPlatformService.cpp
    GDKPlatformSystem.h
    GDKPlatformSystem.cpp
    GDKDynamicLibrary.h
    GDKDynamicLibrary.cpp
)

# Include GDK SDK configuration to get GDK_INCLUDE_DIR
include(${MENGINE_REPOSITORY}/cmake/FindGDK.cmake)

ADD_MENGINE_LIBRARY(Platforms)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

IF(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Shlwapi.lib)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Shell32.lib)
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} GDK)
