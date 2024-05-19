MENGINE_PROJECT(AndroidHttpSystem)

ADD_FILTER(
src
    AndroidHttpSystem.cpp
    AndroidHttpSystem.h
    
    AndroidHttpResponse.cpp
    AndroidHttpResponse.h
    
    AndroidHttpRequestThreadTask.cpp
    AndroidHttpRequestThreadTask.h
    AndroidHttpRequestGetAssetThreadTask.cpp
    AndroidHttpRequestGetAssetThreadTask.h
    AndroidHttpRequestGetMessageThreadTask.cpp
    AndroidHttpRequestGetMessageThreadTask.h
    AndroidHttpRequestHeaderDataThreadTask.cpp
    AndroidHttpRequestHeaderDataThreadTask.h
    AndroidHttpRequestPostMessageThreadTask.cpp
    AndroidHttpRequestPostMessageThreadTask.h
    AndroidHttpRequestDeleteMessageThreadTask.cpp
    AndroidHttpRequestDeleteMessageThreadTask.h
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Android)