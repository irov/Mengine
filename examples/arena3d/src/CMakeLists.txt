cmake_minimum_required(VERSION 3.16)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(ARENA3D_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

add_custom_target(Arena3DContent ALL
    COMMAND ${Python3_EXECUTABLE} ${ARENA3D_ROOT}/tools/arena3d_content.py build
    COMMAND ${Python3_EXECUTABLE} ${ARENA3D_ROOT}/tools/arena3d_content.py check
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ARENA3D_ROOT}/../shaders ${ARENA3D_ROOT}/resources/Data/shaders
    WORKING_DIRECTORY ${ARENA3D_ROOT}
    COMMENT "Generating and validating Arena3D content"
    VERBATIM
)

add_subdirectory(Arena3DSimulation)
add_subdirectory(Arena3DNetworking)
add_subdirectory(Arena3DHeadless)
add_subdirectory(Arena3DServer)
add_subdirectory(Arena3DBotClient)
add_subdirectory(Arena3DNetProxy)

ADD_PLUGIN(MENGINE_PLUGIN_EXAMPLEFRAMEWORK ON OFF "MENGINE_PLUGIN_EXAMPLEFRAMEWORK")
ADD_SUBDIRECTORY(Arena3DFramework)

SET(APPLICATION_LINK_LIBRARIES ${APPLICATION_LINK_LIBRARIES} PARENT_SCOPE)
SET(APPLICATION_DEPENDENCIES ${APPLICATION_DEPENDENCIES} PARENT_SCOPE)
