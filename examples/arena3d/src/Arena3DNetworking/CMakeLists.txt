MENGINE_PROJECT(Arena3DNetworking)

ADD_FILTER(
src
    Arena3DNetworking.h
    Arena3DNetworkServer.cpp
    Arena3DNetworkServer.h
    Arena3DNetworkClient.cpp
    Arena3DNetworkClient.h
)

ADD_MENGINE_LIBRARY(Game)

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel Arena3DSimulation SocketService)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/Data/generated/include)
ADD_DEPENDENCIES(${PROJECT_NAME} Arena3DContent)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Game)
