MENGINE_PROJECT(cURLCook)

ADD_FILTER(
src   
    cURLCook.h
    cURLCook.cpp
)

ADD_FILTER(
Tasks
    TaskcURLHttpBase.cpp
    TaskcURLHttpBase.h
    TaskcURLHttpGet.cpp
    TaskcURLHttpGet.h
    TaskcURLHttpHeaderData.cpp
    TaskcURLHttpHeaderData.h
    TaskcURLHttpPost.cpp
    TaskcURLHttpPost.h
)

INCLUDE_DIRECTORIES(${MENGINE_CMAKE_DEPENDENCIES_DIR}/GOAP/Config)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/GOAP/include)

ADD_MENGINE_LIBRARY(Plugins)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}GOAP${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)