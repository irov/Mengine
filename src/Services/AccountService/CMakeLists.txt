MENGINE_PROJECT(AccountService)

ADD_FILTER(
src
    Account.h
    Account.cpp
    AccountService.h
    AccountService.cpp
)

ADD_MENGINE_LIBRARY(Services)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)