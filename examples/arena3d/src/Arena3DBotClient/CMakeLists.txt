MENGINE_PROJECT(Arena3DBotClient)

ADD_FILTER(src Arena3DBotClientMain.cpp)

if(MENGINE_TARGET_WINDOWS)
    ADD_MENGINE_EXECUTABLE_WIN32_CONSOLE()
else()
    ADD_MENGINE_EXECUTABLE_UNIX()
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Arena3DHeadless Arena3DNetworking Arena3DSimulation SocketService)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Game)
