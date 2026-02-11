MENGINE_PROJECT(TextService)

ADD_FILTER(
src
    TextService.cpp
    TextService.h
    
    TextEntry.cpp
    TextEntry.h
    
    TextArgumentValue.cpp
    TextArgumentValue.h
    
    TextArgumentId.cpp
    TextArgumentId.h
    
    TextArgumentContext.cpp
    TextArgumentContext.h
    
    TextLocalePackage.cpp
    TextLocalePackage.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/xmlsax/include)

ADD_MENGINE_LIBRARY(Services)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}xmlsax${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})