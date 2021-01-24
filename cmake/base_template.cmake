MESSAGE("*********************************************")
MESSAGE("USER: $ENV{USERNAME}")
MESSAGE("*********************************************")
MESSAGE("CMAKE VERSION: ${CMAKE_VERSION}")
MESSAGE("CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
MESSAGE("CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
MESSAGE("CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
MESSAGE("CMAKE_C_COMPILER_ID: ${CMAKE_C_COMPILER_ID}")
MESSAGE("CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
MESSAGE("CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
MESSAGE("CMAKE_ASM_MASM_COMPILER: ${CMAKE_ASM_MASM_COMPILER}")
MESSAGE("CMAKE_ASM_COMPILER: ${CMAKE_ASM_COMPILER}")
MESSAGE("CMAKE_MAKE_PROGRAM: ${CMAKE_MAKE_PROGRAM}")
MESSAGE("CMAKE_GENERATOR_TOOLSET: ${CMAKE_GENERATOR_TOOLSET}")
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

MESSAGE("MENGINE_ARCHITECTURE_X86: ${MENGINE_ARCHITECTURE_X86}")
MESSAGE("MENGINE_ARCHITECTURE_X64: ${MENGINE_ARCHITECTURE_X64}")

IF(IOS)
    SET(THIRDPARTY_ARCHITECTURE_PREFIX "ios")
ELSEIF(APPLE)
    SET(THIRDPARTY_ARCHITECTURE_PREFIX "mac")
ELSEIF(ANDROID)
    SET(THIRDPARTY_ARCHITECTURE_PREFIX "android")
ELSE()
    SET(THIRDPARTY_ARCHITECTURE_PREFIX)
ENDIF()

MESSAGE("THIRDPARTY_ARCHITECTURE_PREFIX: ${THIRDPARTY_ARCHITECTURE_PREFIX}")

IF(MENGINE_ARCHITECTURE_X86)
    SET(THIRDPARTY_ARCHITECTURE_SUFFIX "x86")
ELSEIF(MENGINE_ARCHITECTURE_X64)
    SET(THIRDPARTY_ARCHITECTURE_SUFFIX "x64")
ELSE()
    MESSAGE(FATAL_ERROR "unsupported architecture") 
ENDIF()

MESSAGE("THIRDPARTY_ARCHITECTURE_SUFFIX: ${THIRDPARTY_ARCHITECTURE_SUFFIX}")

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

IF(CMAKE_GENERATOR_PLATFORM)
    SET(THIRDPARTY_LIB_OUT "${MENGINE_REPOSITORY}/lib/dependencies/${CMAKE_GENERATOR} ${CMAKE_GENERATOR_PLATFORM}/${THIRDPARTY_ARCHITECTURE_PREFIX}${THIRDPARTY_ARCHITECTURE_SUFFIX}")
    SET(THIRDPARTY_CONFIG_OUT "${MENGINE_REPOSITORY}/dependencies/configuration/${CMAKE_GENERATOR} ${CMAKE_GENERATOR_PLATFORM}/${THIRDPARTY_ARCHITECTURE_PREFIX}${THIRDPARTY_ARCHITECTURE_SUFFIX}")
ELSE()
    SET(THIRDPARTY_LIB_OUT "${MENGINE_REPOSITORY}/lib/dependencies/${CMAKE_GENERATOR}/${THIRDPARTY_ARCHITECTURE_PREFIX}${THIRDPARTY_ARCHITECTURE_SUFFIX}")
    SET(THIRDPARTY_CONFIG_OUT "${MENGINE_REPOSITORY}/dependencies/configuration/${CMAKE_GENERATOR}/${THIRDPARTY_ARCHITECTURE_PREFIX}${THIRDPARTY_ARCHITECTURE_SUFFIX}")
ENDIF()

SET(THIRDPARTY_DIR ${MENGINE_REPOSITORY}/dependencies)

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

SET(THIRDPARTY_LIB_DIR ${THIRDPARTY_LIB_OUT}/${THIRDPARTY_LIB_OUT_SUFFIX})
SET(THIRDPARTY_CONFIG_DIR ${THIRDPARTY_CONFIG_OUT}/${THIRDPARTY_LIB_OUT_SUFFIX})

MESSAGE("THIRDPARTY_DIR: ${THIRDPARTY_DIR}")
MESSAGE("THIRDPARTY_LIB_DIR: ${THIRDPARTY_LIB_DIR}")
MESSAGE("THIRDPARTY_CONFIG_DIR: ${THIRDPARTY_CONFIG_DIR}")

SET(MENGINE_CMAKE_DEPENDENCIES_DIR ${MENGINE_REPOSITORY}/cmake/Dependencies)
SET(MENGINE_SOURCE_DIR ${MENGINE_REPOSITORY}/src)