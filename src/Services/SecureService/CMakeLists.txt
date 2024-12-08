MENGINE_PROJECT(SecureService)

ADD_FILTER(
src
    SecureService.h
    SecureService.cpp
    
    SecureUnsignedValue.h
    SecureUnsignedValue.cpp
    SecureStringValue.h
    SecureStringValue.cpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/SecureValue.h.in ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/SecureValue.h @ONLY)
MESSAGE("configure file: ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/SecureValue.h")

ADD_FILTER(
configuration
    ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/SecureValue.h
)

INCLUDE_DIRECTORIES(${MENGINE_SOLUTIONS_CONFIG_DIR}/include)

ADD_MENGINE_LIBRARY(Services)