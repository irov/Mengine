MENGINE_PROJECT(ResourceConvertPlugin)

ADD_FILTER(
src
    ResourceConvertServiceInterface.h

    ResourceConvertPlugin.h
    ResourceConvertPlugin.def
    ResourceConvertPlugin.cpp
    
    ResourceConvertService.h
    ResourceConvertService.cpp    
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_RESOURCECONVERT)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)