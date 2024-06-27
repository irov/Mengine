MENGINE_PROJECT(Config)

SET(MENGINE_ENGINE_GIT_SHA1 "0000000000000000000000000000000000000000")
SET(MENGINE_ENGINE_GIT_URL "NONE-URL")
SET(MENGINE_ENGINE_GIT_BRANCH "UNKNOWN-BRANCH")
SET(MENGINE_ENGINE_GIT_DATE "UNKNOWN-DATE")

find_package(Git)
MESSAGE("GIT_EXECUTABLE: ${GIT_EXECUTABLE}")

if(Git_FOUND)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE MENGINE_ENGINE_GIT_SHA1
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
        )

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" config --get remote.origin.url
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE MENGINE_ENGINE_GIT_URL
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE MENGINE_ENGINE_GIT_BRANCH
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
        )

    execute_process(
        COMMAND "${GIT_EXECUTABLE}" show -s --format=%ci HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE MENGINE_ENGINE_GIT_DATE
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE
        )
endif()

MESSAGE("MENGINE_ENGINE_GIT_SHA1: ${MENGINE_ENGINE_GIT_SHA1}")
MESSAGE("MENGINE_ENGINE_GIT_URL: ${MENGINE_ENGINE_GIT_URL}")
MESSAGE("MENGINE_ENGINE_GIT_BRANCH: ${MENGINE_ENGINE_GIT_BRANCH}")
MESSAGE("MENGINE_ENGINE_GIT_DATE: ${MENGINE_ENGINE_GIT_DATE}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.h.in ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/GitSHA1.h @ONLY)
MESSAGE("configure file: ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/GitSHA1.h")

string(TIMESTAMP MENGINE_BUILD_DATE "%Y-%m-%d %H:%M:%S")

set(MENGINE_BUILD_USERNAME $ENV{USERNAME} CACHE STRING "MENGINE_BUILD_USERNAME")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BuildInfo.h.in ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/BuildInfo.h @ONLY)
MESSAGE("configure file: ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/BuildInfo.h")

MESSAGE("MENGINE_BUILD_USERNAME: ${MENGINE_BUILD_USERNAME}")
MESSAGE("MENGINE_BUILD_DATE: ${MENGINE_BUILD_DATE}")

SET(DEFINITIONS_BUFFER "")

STRING(APPEND DEFINITIONS_BUFFER "#pragma once\n")
STRING(APPEND DEFINITIONS_BUFFER "\n")
STRING(APPEND DEFINITIONS_BUFFER "#if defined(__MENGINE_CONFIG_DEFINITIONS__)\n")

list(LENGTH MENGINE_CONFIG_DEFINITIONS LENGTH_MENGINE_CONFIG_DEFINITIONS)
math(EXPR LENGTH_MENGINE_CONFIG_DEFINITIONS "${LENGTH_MENGINE_CONFIG_DEFINITIONS}-1")

foreach(INDEX RANGE 0 ${LENGTH_MENGINE_CONFIG_DEFINITIONS} 2)
    list(GET MENGINE_CONFIG_DEFINITIONS ${INDEX} DEFINITION_NAME)
    math(EXPR INDEX "${INDEX}+1")
    
    list(GET MENGINE_CONFIG_DEFINITIONS ${INDEX} DEFINITION_VALUE)
    math(EXPR INDEX "${INDEX}+1")
    
    string(COMPARE EQUAL ${DEFINITION_VALUE} "NO-VALUE" DEFINITION_NO_VALUE)
    
    if(NOT DEFINITION_NO_VALUE)
        STRING(APPEND DEFINITIONS_BUFFER "#define ${DEFINITION_NAME} ${DEFINITION_VALUE}\n")
    else()
        STRING(APPEND DEFINITIONS_BUFFER "#define ${DEFINITION_NAME}\n")
    endif()
endforeach()

STRING(APPEND DEFINITIONS_BUFFER "#endif\n")

FILE(WRITE ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/Definitions.h ${DEFINITIONS_BUFFER})
MESSAGE("configure file: ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/Definitions.h")

ADD_FILTER(
src
    Char.h
    Utf8.h
    Utf16.h
    Utf32.h
    Stringz.h
    Typedef.h
    Typeinfo.h
    Atomic.h
    Lambda.h
    Config.h
    Export.h
    Abs.h
    StdInt.h
    StdDef.h
    StdIntTypes.h
    StdArg.h
    StdIO.h
    StdLib.h
    StdString.h
    StdLocale.h
    StdCType.h
    StdMath.h
    Limits.h
    UInt32ToPointer.h
    Metaprogramming.h
    UniqueId.h
    NotificatorId.h
    StatisticId.h
    Algorithm.h
    Numeric.h
    Iterator.h
    InitializerList.h
    Timestamp.h
    Thread.h
    Switch.h
    Utility.h
    Variant.h
    Version.h
)

ADD_FILTER(
configuration
    ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/GitSHA1.h
    ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/BuildInfo.h
    ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/Definitions.h
)

ADD_MENGINE_INTERFACE()