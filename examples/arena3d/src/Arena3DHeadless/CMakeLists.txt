MENGINE_PROJECT(Arena3DHeadless)

ADD_FILTER(
src
    Arena3DHeadless.cpp
    Arena3DHeadless.h
)

ADD_MENGINE_LIBRARY(Game)

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel ProviderService EnumeratorService StringizeService DocumentService StatisticService SocketService)

if(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} DebugAllocatorSystem Win32TimeSystem Win32ThreadSystem Win32SocketSystem ws2_32)
elseif(MENGINE_TARGET_MACOS OR MENGINE_TARGET_UNIX)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} DebugAllocatorSystem POSIXTimeSystem POSIXThreadSystem POSIXSocketSystem)
else()
    message(FATAL_ERROR "Arena3DHeadless requires Windows, macOS, or Unix")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Game)
