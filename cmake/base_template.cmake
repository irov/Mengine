MESSAGE("*********************************************")
MESSAGE("USER: $ENV{USERNAME}")
MESSAGE("*********************************************")
MESSAGE("CMAKE_COMMAND: ${CMAKE_COMMAND}")
MESSAGE("CMAKE_VERSION: ${CMAKE_VERSION}")
MESSAGE("CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
MESSAGE("CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
MESSAGE("CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
MESSAGE("CMAKE_C_COMPILER_ID: ${CMAKE_C_COMPILER_ID}")
MESSAGE("CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
MESSAGE("CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
MESSAGE("CMAKE_ASM_MASM_COMPILER: ${CMAKE_ASM_MASM_COMPILER}")
MESSAGE("CMAKE_ASM_COMPILER: ${CMAKE_ASM_COMPILER}")
MESSAGE("CMAKE_MAKE_PROGRAM: ${CMAKE_MAKE_PROGRAM}")
MESSAGE("CMAKE_GENERATOR_PLATFORM: ${CMAKE_GENERATOR_PLATFORM}")
MESSAGE("CMAKE_GENERATOR_TOOLSET: ${CMAKE_GENERATOR_TOOLSET}")
MESSAGE("CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")
MESSAGE("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
MESSAGE("CMAKE_SYSTEM_VERSION: ${CMAKE_SYSTEM_VERSION}")
MESSAGE("CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
MESSAGE("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
MESSAGE("CMAKE_SIZEOF_VOID_P: ${CMAKE_SIZEOF_VOID_P}")
MESSAGE("*********************************************")
MESSAGE("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
MESSAGE("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
MESSAGE("*********************************************")

IF(MSVC)
    MESSAGE("CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")    
    MESSAGE("CMAKE_VS_PLATFORM_TOOLSET_VERSION: ${CMAKE_VS_PLATFORM_TOOLSET_VERSION}")
    MESSAGE("*********************************************")
ENDIF()

IF(APPLE)
    MESSAGE("CMAKE_OSX_ARCHITECTURES: ${CMAKE_OSX_ARCHITECTURES}")
    MESSAGE("*********************************************")
ENDIF()

SET(CMAKE_TEMP_DIR ${CMAKE_BINARY_DIR}/solutions)

IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
    SET(MENGINE_ARCHITECTURE_X86 ON CACHE BOOL "MENGINE_ARCHITECTURE_X86" FORCE)
    SET(MENGINE_ARCHITECTURE_X64 OFF CACHE BOOL "MENGINE_ARCHITECTURE_X64" FORCE)
ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(MENGINE_ARCHITECTURE_X86 OFF CACHE BOOL "MENGINE_ARCHITECTURE_X86" FORCE)
    SET(MENGINE_ARCHITECTURE_X64 ON CACHE BOOL "MENGINE_ARCHITECTURE_X64" FORCE)
ELSE()
    message(FATAL_ERROR "unsupport CMAKE_SIZEOF_VOID_P")
ENDIF()

if(MENGINE_ARCHITECTURE_X86)
    ADD_DEFINITIONS(-DMENGINE_ENVIRONMENT_ARCHITECTURE_X86)
elseif(MENGINE_ARCHITECTURE_X64)
    ADD_DEFINITIONS(-DMENGINE_ENVIRONMENT_ARCHITECTURE_X64)
else()
    message(FATAL_ERROR "Invalid select ARCHITECTURE")
endif()

MESSAGE("MENGINE_ARCHITECTURE_X86: ${MENGINE_ARCHITECTURE_X86}")
MESSAGE("MENGINE_ARCHITECTURE_X64: ${MENGINE_ARCHITECTURE_X64}")

SET(MENGINE_WINDOWS_UNIVERSAL OFF CACHE BOOL "MENGINE_WINDOWS_UNIVERSAL" FORCE)

IF(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
    SET(MENGINE_WINDOWS_UNIVERSAL ON CACHE BOOL "MENGINE_WINDOWS_UNIVERSAL" FORCE)
ENDIF()

if(MENGINE_WINDOWS_UNIVERSAL)
    ADD_DEFINITIONS(-DMENGINE_WINDOWS_UNIVERSAL)
endif()

MESSAGE("MENGINE_WINDOWS_UNIVERSAL: ${MENGINE_WINDOWS_UNIVERSAL}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

SET(MENGINE_REPOSITORY ${CMAKE_CURRENT_SOURCE_DIR}/../..)

IF(NOT MENGINE_INSTALL_PATH)
    SET(MENGINE_INSTALL_PATH ${MENGINE_REPOSITORY}/bin)
ENDIF()

MESSAGE("MENGINE_INSTALL_PATH: ${MENGINE_INSTALL_PATH}")

IF(NOT MENGINE_PDB_PATH)
    SET(MENGINE_PDB_PATH ${MENGINE_REPOSITORY}/pdb)
ENDIF()

MESSAGE("MENGINE_PDB_PATH: ${MENGINE_PDB_PATH}")

SET(THIRDPARTY_DIR ${MENGINE_REPOSITORY}/dependencies)
MESSAGE("THIRDPARTY_DIR: ${THIRDPARTY_DIR}")

SET(MENGINE_CMAKE_DEPENDENCIES_DIR ${MENGINE_REPOSITORY}/cmake/Dependencies)
SET(MENGINE_SOURCE_DIR ${MENGINE_REPOSITORY}/src)

list(APPEND CMAKE_MODULE_PATH "${THIRDPARTY_DIR}/cmake-modules/")

MESSAGE("UNIX: ${UNIX}")
MESSAGE("ANDROID: ${ANDROID}")
MESSAGE("MINGW: ${MINGW}")
MESSAGE("WIN32: ${WIN32}")
MESSAGE("APPLE: ${APPLE}")
MESSAGE("IOS: ${IOS}")
MESSAGE("BORLAND: ${BORLAND}")
MESSAGE("CYGWIN: ${CYGWIN}")
MESSAGE("CYGWIN: ${CYGWIN}")
MESSAGE("MSVC: ${MSVC}")
MESSAGE("WINCE: ${WINCE}")
MESSAGE("WINDOWS_PHONE: ${WINDOWS_PHONE}")
MESSAGE("WINDOWS_STORE: ${WINDOWS_STORE}")
MESSAGE("XCODE: ${XCODE}")

if(UNIX AND NOT ANDROID AND NOT APPLE AND NOT IOS)
    # input and thirdparty paths
    SET(THIRDPARTY_LIB_OUT_SUFFIX ${CMAKE_BUILD_TYPE})
elseif(ANDROID)
    # input and thirdparty paths
    SET(THIRDPARTY_LIB_OUT_SUFFIX ${CMAKE_BUILD_TYPE}/${ANDROID_ABI})
elseif(MINGW)
    # input and thirdparty paths
    SET(THIRDPARTY_LIB_OUT_SUFFIX ${CMAKE_BUILD_TYPE})
elseif(APPLE)
    # input and thirdparty paths
    SET(THIRDPARTY_LIB_OUT_SUFFIX ${CMAKE_BUILD_TYPE})
else()
    # input and thirdparty paths
    SET(THIRDPARTY_LIB_OUT_SUFFIX ${CMAKE_BUILD_TYPE})
endif()

SET(MENGINE_BUILD_TYPE_DEBUG OFF)
SET(MENGINE_BUILD_TYPE_RELWITHDEBINFO OFF)
SET(MENGINE_BUILD_TYPE_RELEASE OFF)

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(MENGINE_BUILD_TYPE_DEBUG ON)
ELSEIF(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    SET(MENGINE_BUILD_TYPE_RELWITHDEBINFO ON)
ELSEIF(CMAKE_BUILD_TYPE MATCHES "Release")
    SET(MENGINE_BUILD_TYPE_RELEASE ON)
ELSE()
    MESSAGE(FATAL_ERROR "Non support build type: ${CMAKE_BUILD_TYPE}")
ENDIF()

MESSAGE("MENGINE_BUILD_TYPE_DEBUG: ${MENGINE_BUILD_TYPE_DEBUG}")
MESSAGE("MENGINE_BUILD_TYPE_RELWITHDEBINFO: ${MENGINE_BUILD_TYPE_RELWITHDEBINFO}")
MESSAGE("MENGINE_BUILD_TYPE_RELEASE: ${MENGINE_BUILD_TYPE_RELEASE}")

IF(MENGINE_BUILD_TYPE_DEBUG)
    SET(MENGINE_DEPENDENCIES_DEBUG_SUFFIX "d")
ELSE()
    SET(MENGINE_DEPENDENCIES_DEBUG_SUFFIX "")
ENDIF()

MESSAGE("MENGINE_DEPENDENCIES_DEBUG_SUFFIX: ${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}")

if(APPLE)
    MESSAGE("CMAKE_OSX_SYSROOT: ${CMAKE_OSX_SYSROOT}")
    MESSAGE("CMAKE_OSX_DEPLOYMENT_TARGET: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
endif()