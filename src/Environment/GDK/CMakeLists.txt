MENGINE_PROJECT(GDK)

ADD_FILTER(
src
    GDKIncluder.h
    
    GDKDummy.h
    GDKDummy.cpp
)

# Include GDK SDK configuration
# Note: This should be included from the main CMakeLists.txt (cmake/GDK/CMakeLists.txt)
# before any targets are created, so GDK_INCLUDE_DIR is available globally

ADD_MENGINE_LIBRARY(Environment)

IF(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Windows)

    if(GDK_LIB_DIR)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GDK_LIB_DIR}/xgameruntime.lib)
    else()
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} xgameruntime.lib)
    endif()
ENDIF()
