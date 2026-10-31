MENGINE_PROJECT(Emscripten)

ADD_FILTER(
src
    EmscriptenIncluder.h
    EmscriptenLocalStorage.h

    EmscriptenEnvironmentService.h
    EmscriptenEnvironmentService.cpp
)

ADD_MENGINE_LIBRARY(Environment)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

TARGET_LINK_OPTIONS(${PROJECT_NAME} INTERFACE
    "SHELL:--js-library '${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenLocalStorage.js'"
)

SET_PROPERTY(TARGET ${PROJECT_NAME} APPEND PROPERTY INTERFACE_LINK_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/EmscriptenLocalStorage.js
)
