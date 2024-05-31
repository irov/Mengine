MENGINE_PROJECT(Win32CryptographySystem)

ADD_FILTER(
src
    Win32CryptographySystem.h
    Win32CryptographySystem.cpp
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Advapi32.lib)
