MENGINE_PROJECT_EXTRA_LANGUAGE(SwiftEnvironment Swift)

ADD_FILTER(
src
    SwiftLog.h
    SwiftLog.mm
    SwiftLogger.swift
    module.modulemap
)

ADD_MENGINE_LIBRARY(Environment)

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} AppleEnvironment iOSEnvironment)

set_target_properties(${PROJECT_NAME} PROPERTIES
    XCODE_ATTRIBUTE_SWIFT_VERSION 5.0
)
