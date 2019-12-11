set(MY_LIB_NAME ResourceConvertPlugin)

ADD_FILTER(
src
    ResourceConvertServiceInterface.h

    ResourceConvertPlugin.h
    ResourceConvertPlugin.def
    ResourceConvertPlugin.cpp
    
    ResourceConvertService.h
    ResourceConvertService.cpp    
)

ADD_MENGINE_LIBRARY(Plugins)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)