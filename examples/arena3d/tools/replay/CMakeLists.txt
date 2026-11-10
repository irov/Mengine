add_executable(Arena3DReplayTool Arena3DReplayMain.cpp Arena3DReplayAllocator.cpp)
target_link_libraries(Arena3DReplayTool PRIVATE Arena3DSimulation)
target_compile_features(Arena3DReplayTool PRIVATE cxx_std_20)
add_dependencies(Arena3DReplayTool Arena3DContent)

add_executable(Arena3DGrenadeTests Arena3DGrenadeTests.cpp Arena3DReplayAllocator.cpp)
target_link_libraries(Arena3DGrenadeTests PRIVATE Arena3DSimulation)
target_compile_features(Arena3DGrenadeTests PRIVATE cxx_std_20)
add_test(NAME Arena3DGrenadeTests COMMAND Arena3DGrenadeTests)

add_executable(Arena3DRailgunTests Arena3DRailgunTests.cpp Arena3DReplayAllocator.cpp)
target_link_libraries(Arena3DRailgunTests PRIVATE Arena3DSimulation)
target_include_directories(Arena3DRailgunTests PRIVATE ${ARENA3D_GENERATED}/include)
target_compile_features(Arena3DRailgunTests PRIVATE cxx_std_20)
add_test(NAME Arena3DRailgunTests COMMAND Arena3DRailgunTests ${CMAKE_CURRENT_BINARY_DIR}/railgun_test.a3rp)

add_executable(Arena3DMovementTests Arena3DMovementTests.cpp Arena3DReplayAllocator.cpp)
target_link_libraries(Arena3DMovementTests PRIVATE Arena3DSimulation)
target_include_directories(Arena3DMovementTests PRIVATE ${ARENA3D_GENERATED}/include)
target_compile_features(Arena3DMovementTests PRIVATE cxx_std_20)
add_test(NAME Arena3DMovementTests COMMAND Arena3DMovementTests)

add_executable(Arena3DCommandTests Arena3DCommandTests.cpp Arena3DReplayAllocator.cpp)
target_link_libraries(Arena3DCommandTests PRIVATE Arena3DSimulation)
target_compile_features(Arena3DCommandTests PRIVATE cxx_std_20)
add_test(NAME Arena3DCommandTests COMMAND Arena3DCommandTests ${CMAKE_CURRENT_BINARY_DIR}/command_test.a3rp)

add_executable(Arena3DPausedReplayTests Arena3DPausedReplayTests.cpp Arena3DReplayAllocator.cpp)
target_link_libraries(Arena3DPausedReplayTests PRIVATE Arena3DSimulation)
target_compile_features(Arena3DPausedReplayTests PRIVATE cxx_std_20)
add_test(NAME Arena3DPausedReplayTests COMMAND Arena3DPausedReplayTests ${CMAKE_CURRENT_BINARY_DIR}/paused_replay_test.a3rp)

set(ARENA3D_ENGINE_ROOT ${ARENA3D_ROOT}/../..)
add_library(Arena3DTestSupport STATIC
    Arena3DReplayAllocator.cpp Arena3DTestDiagnostics.cpp
    ${ARENA3D_ENGINE_ROOT}/src/Kernel/Factorable.cpp
    ${ARENA3D_ENGINE_ROOT}/src/Kernel/ReferenceCounter.cpp
    ${ARENA3D_ENGINE_ROOT}/src/Kernel/Documentable.cpp
    ${ARENA3D_ENGINE_ROOT}/src/Kernel/ApplicationLifecycleable.cpp
    ${ARENA3D_ENGINE_ROOT}/src/Kernel/RenderDeviceLifecycleable.cpp)
target_include_directories(Arena3DTestSupport PUBLIC ${ARENA3D_ENGINE_ROOT}/src ${ARENA3D_ENGINE_ROOT}/dependencies/stdex/include)
target_compile_definitions(Arena3DTestSupport PRIVATE MENGINE_LIBRARY_NAME="Arena3DTestSupport" MENGINE_LIBRARY_STATIC)
target_compile_features(Arena3DTestSupport PUBLIC cxx_std_20)

add_executable(Arena3DNetworkingTests
    Arena3DNetworkingTests.cpp
    ${ARENA3D_ROOT}/src/Arena3DNetworking/Arena3DNetworkServer.cpp
    ${ARENA3D_ROOT}/src/Arena3DNetworking/Arena3DNetworkClient.cpp)
target_link_libraries(Arena3DNetworkingTests PRIVATE Arena3DSimulation Arena3DTestSupport)
target_include_directories(Arena3DNetworkingTests PRIVATE ${ARENA3D_ROOT}/src/Arena3DNetworking ${ARENA3D_GENERATED}/include)
target_compile_definitions(Arena3DNetworkingTests PRIVATE MENGINE_LIBRARY_NAME="Arena3DNetworkingTests" MENGINE_LIBRARY_STATIC)
add_test(NAME Arena3DNetworkingTests COMMAND Arena3DNetworkingTests)

add_executable(Arena3DSocketServiceTests
    Arena3DSocketServiceTests.cpp Arena3DSocketReceiveTests.cpp
    ${ARENA3D_ENGINE_ROOT}/src/Services/SocketService/SocketService.cpp
    ${ARENA3D_ENGINE_ROOT}/src/Kernel/Observable.cpp
    ${ARENA3D_ENGINE_ROOT}/src/Kernel/BaseLifecycle.cpp
    ${ARENA3D_ENGINE_ROOT}/src/Kernel/ThreadMutexScope.cpp
    ${ARENA3D_ENGINE_ROOT}/src/Kernel/ReadHelper.cpp
    ${ARENA3D_ENGINE_ROOT}/src/Kernel/WriteHelper.cpp)
target_link_libraries(Arena3DSocketServiceTests PRIVATE Arena3DTestSupport)
target_compile_definitions(Arena3DSocketServiceTests PRIVATE MENGINE_LIBRARY_NAME="Arena3DSocketServiceTests" MENGINE_LIBRARY_STATIC MENGINE_STATISTIC=0)
add_test(NAME Arena3DSocketServiceTests COMMAND Arena3DSocketServiceTests)

set(ARENA3D_TEST_TARGETS
    Arena3DReplayTool Arena3DGrenadeTests Arena3DRailgunTests Arena3DMovementTests
    Arena3DCommandTests Arena3DPausedReplayTests Arena3DNetworkingTests
    Arena3DSocketServiceTests Arena3DTestSupport)
if(UNIX)
    add_executable(Arena3DDatagramTests Arena3DDatagramTests.cpp ${ARENA3D_ENGINE_ROOT}/src/Systems/POSIXSocketSystem/POSIXDatagram.cpp)
    target_link_libraries(Arena3DDatagramTests PRIVATE Arena3DTestSupport)
    target_include_directories(Arena3DDatagramTests PRIVATE ${ARENA3D_ENGINE_ROOT}/src/Systems/POSIXSocketSystem)
    target_compile_definitions(Arena3DDatagramTests PRIVATE MENGINE_LIBRARY_NAME="Arena3DDatagramTests" MENGINE_LIBRARY_STATIC)
    add_test(NAME Arena3DDatagramTests COMMAND Arena3DDatagramTests)
    list(APPEND ARENA3D_TEST_TARGETS Arena3DDatagramTests)
endif()

foreach(ARENA3D_CORE_TARGET Arena3DReplayTool Arena3DGrenadeTests Arena3DRailgunTests Arena3DMovementTests Arena3DCommandTests Arena3DPausedReplayTests)
    target_link_libraries(${ARENA3D_CORE_TARGET} PRIVATE Arena3DTestSupport)
endforeach()

foreach(ARENA3D_TEST_TARGET IN LISTS ARENA3D_TEST_TARGETS)
    if(MSVC)
        target_compile_options(${ARENA3D_TEST_TARGET} PRIVATE /W4 /WX)
    else()
        target_compile_options(${ARENA3D_TEST_TARGET} PRIVATE -Wall -Wextra -Werror)
    endif()
endforeach()
