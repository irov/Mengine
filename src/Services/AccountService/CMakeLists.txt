SET(MY_LIB_NAME AccountService)

ADD_FILTER(
src
    Account.h
    Account.cpp
    AccountService.h
    AccountService.cpp
)

ADD_MENGINE_LIBRARY(Services)