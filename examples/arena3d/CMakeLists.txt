cmake_minimum_required(VERSION 3.16)

project(Arena3DHeadless LANGUAGES C CXX)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(ARENA3D_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(ARENA3D_GENERATED ${ARENA3D_ROOT}/resources/Data/generated)
set(ARENA3D_KINEFIX_ROOT ${ARENA3D_ROOT}/../../dependencies/kinefix)

if(NOT EXISTS ${ARENA3D_KINEFIX_ROOT}/CMakeLists.txt)
    message(FATAL_ERROR "Arena3D requires the Mengine kinefix dependency. Run the Mengine downloads target first.")
endif()

set(KINEFIX_BUILD_TESTS ON CACHE BOOL "KINEFIX_BUILD_TESTS" FORCE)
set(KINEFIX_INSTALL OFF CACHE BOOL "KINEFIX_INSTALL" FORCE)
add_subdirectory(${ARENA3D_KINEFIX_ROOT} ${CMAKE_BINARY_DIR}/kinefix)

add_custom_target(Arena3DContent ALL
    COMMAND ${Python3_EXECUTABLE} ${ARENA3D_ROOT}/tools/arena3d_content.py build
    COMMAND ${Python3_EXECUTABLE} ${ARENA3D_ROOT}/tools/arena3d_content.py check
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ARENA3D_ROOT}/../shaders ${ARENA3D_ROOT}/resources/Data/shaders
    WORKING_DIRECTORY ${ARENA3D_ROOT}
    COMMENT "Generating and validating Arena3D low-poly content"
    VERBATIM
)

add_subdirectory(src/Arena3DSimulation)
add_subdirectory(tools/replay)

enable_testing()
add_subdirectory(tests)
