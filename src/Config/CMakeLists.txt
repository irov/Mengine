MENGINE_PROJECT(Config)

list(APPEND CMAKE_MODULE_PATH "${THIRDPARTY_DIR}/cmake-modules/")
include(GetGitRevisionDescription)

SET(MENGINE_GIT_SHA1 "0000000000000000000000000000000000000000")
get_git_head_revision(GIT_REFSPEC MENGINE_GIT_SHA1)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.h.in ${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.h @ONLY)

MESSAGE("MENGINE_GIT_SHA1: ${MENGINE_GIT_SHA1}")

set(MENGINE_BUILD_VERSION "Versionless" CACHE STRING "MENGINE_BUILD_VERSION")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BuildVersion.h.in ${CMAKE_CURRENT_SOURCE_DIR}/BuildVersion.h @ONLY)

MESSAGE("MENGINE_BUILD_VERSION: ${MENGINE_BUILD_VERSION}")

ADD_FILTER(
src
    Char.h
    Typedef.h
    Atomic.h
    Lambda.h
    Defines.h
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
    UInt32ToPointer.h
    Metaprogramming.h
    GitSHA1.h
    BuildVersion.h
    cmake_dummy.cpp
)

ADD_MENGINE_LIBRARY()