add_library(Arena3DSimulation STATIC
    Arena3DFixed.cpp
    Arena3DFixed.h
    Arena3DProtocol.cpp
    Arena3DProtocol.h
    Arena3DSimulation.cpp
    Arena3DSimulation.h
    Arena3DSession.cpp
    Arena3DSession.h
    Arena3DReplay.cpp
    Arena3DReplay.h
)

target_include_directories(Arena3DSimulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Arena3DSimulation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/Data/generated/include)
target_compile_features(Arena3DSimulation PUBLIC cxx_std_17)
add_dependencies(Arena3DSimulation Arena3DContent)

if(DEFINED THIRDPARTY_LIB_DIR AND DEFINED MENGINE_LIB_PREFIX)
    target_include_directories(Arena3DSimulation PUBLIC ${THIRDPARTY_DIR}/kinefix/include)
    target_link_libraries(Arena3DSimulation PUBLIC ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}kinefix${MENGINE_LIB_SUFFIX})
else()
    target_link_libraries(Arena3DSimulation PUBLIC kinefix)
endif()

if(MSVC)
    target_compile_options(Arena3DSimulation PRIVATE /W4 /WX)
else()
    target_compile_options(Arena3DSimulation PRIVATE -Wall -Wextra -Werror)
endif()
