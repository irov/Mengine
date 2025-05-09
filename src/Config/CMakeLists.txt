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

if(NOT LENGTH_MENGINE_CONFIG_DEFINITIONS EQUAL 0)
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
endif()

STRING(APPEND DEFINITIONS_BUFFER "#endif\n")

FILE(WRITE ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/Definitions.h ${DEFINITIONS_BUFFER})
MESSAGE("configure file: ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/Definitions.h")

SET(CONFIGURATIONS_BUFFER "")

STRING(APPEND CONFIGURATIONS_BUFFER "#pragma once\n")
STRING(APPEND CONFIGURATIONS_BUFFER "\n")
STRING(APPEND CONFIGURATIONS_BUFFER "#include \"Config/Config.h\"\n")
STRING(APPEND CONFIGURATIONS_BUFFER "\n")

list(LENGTH MENGINE_CONFIG_CONFIGURATIONS LENGTH_MENGINE_CONFIG_CONFIGURATIONS)

if(NOT LENGTH_MENGINE_CONFIG_CONFIGURATIONS EQUAL 0)
    math(EXPR LENGTH_MENGINE_CONFIG_CONFIGURATIONS "${LENGTH_MENGINE_CONFIG_CONFIGURATIONS}-1")

    foreach(INDEX RANGE 0 ${LENGTH_MENGINE_CONFIG_CONFIGURATIONS} 2)
        list(GET MENGINE_CONFIG_CONFIGURATIONS ${INDEX} CONFIGURATION_NAME)
        math(EXPR INDEX "${INDEX}+1")
        
        list(GET MENGINE_CONFIG_CONFIGURATIONS ${INDEX} CONFIGURATION_VALUE)
        math(EXPR INDEX "${INDEX}+1")
        
        string(COMPARE EQUAL ${CONFIGURATION_VALUE} "NO-VALUE" CONFIGURATION_NO_VALUE)
        
        if(NOT CONFIGURATION_NO_VALUE)
            STRING(APPEND CONFIGURATIONS_BUFFER "#define ${CONFIGURATION_NAME} ${CONFIGURATION_VALUE}\n")
        else()
            STRING(APPEND CONFIGURATIONS_BUFFER "#define ${CONFIGURATION_NAME}\n")
        endif()
    endforeach()
endif()

FILE(WRITE ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/Configurations.h ${CONFIGURATIONS_BUFFER})
MESSAGE("configure file: ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/Configurations.h")

ADD_FILTER(
src
    Char.h
    Utf8.h
    Utf16.h
    Utf32.h
    Path.h
    Stringz.h
    Typedef.h
    Typeinfo.h
    New.h
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
    StdLimits.h
    StdLib.h
    StdString.h
    StdLocale.h
    StdCType.h
    StdMath.h
    StdSetJMP.h
    StdException.h
    StdAlgorithm.h
    StdNumeric.h
    StdIterator.h
    StdUtility.h
    StdThread.h
    Limits.h
    UInt32ToPointer.h
    Metaprogramming.h
    UniqueId.h
    NotificatorId.h
    StatisticId.h
    InitializerList.h
    Timestamp.h
    ThreadId.h
    Switch.h
    Variant.h
    Version.h
    TypeTraits.h
    DynamicCast.h
)

ADD_FILTER(
configuration
    ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/GitSHA1.h
    ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/BuildInfo.h
    ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/Definitions.h
    ${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/Configurations.h
)

ADD_MENGINE_INTERFACE()