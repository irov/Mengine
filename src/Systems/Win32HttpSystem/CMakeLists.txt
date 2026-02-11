MENGINE_PROJECT(Win32HttpSystem)

ADD_FILTER(
src
    Win32HttpSystem.cpp
    Win32HttpSystem.h
    
    Win32HttpNetwork.h
    Win32HttpNetwork.cpp
    
    Win32HttpRequest.cpp
    Win32HttpRequest.h
    Win32HttpRequestPing.cpp
    Win32HttpRequestPing.h
    Win32HttpRequestGetAsset.cpp
    Win32HttpRequestGetAsset.h
    Win32HttpRequestGetMessage.cpp
    Win32HttpRequestGetMessage.h
    Win32HttpRequestHeaderData.cpp
    Win32HttpRequestHeaderData.h
    Win32HttpRequestPostMessage.cpp
    Win32HttpRequestPostMessage.h
    Win32HttpRequestDeleteMessage.cpp
    Win32HttpRequestDeleteMessage.h
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Windows)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Wininet.lib)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Crypt32.lib)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ws2_32.lib)