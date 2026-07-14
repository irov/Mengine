add_executable(Arena3DTests Arena3DTests.cpp)
target_link_libraries(Arena3DTests PRIVATE Arena3DSimulation)
target_compile_features(Arena3DTests PRIVATE cxx_std_17)
add_dependencies(Arena3DTests Arena3DContent)

if(MSVC)
    target_compile_options(Arena3DTests PRIVATE /W4 /WX)
else()
    target_compile_options(Arena3DTests PRIVATE -Wall -Wextra -Werror)
endif()

add_test(NAME Arena3DTests COMMAND Arena3DTests)
