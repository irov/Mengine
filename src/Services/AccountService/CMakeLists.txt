MENGINE_PROJECT(AccountService)

ADD_FILTER(
src
    Account.h
    Account.cpp
    AccountService.h
    AccountService.cpp
)

ADD_MENGINE_LIBRARY(Services)