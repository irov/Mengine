MENGINE_PROJECT(Config)

include(GetGitRevisionDescription)

SET(MENGINE_ENGINE_GIT_SHA1 "0000000000000000000000000000000000000000")
get_git_head_revision(GIT_REFSPEC MENGINE_ENGINE_GIT_SHA1 "ALLOW_LOOKING_ABOVE_CMAKE_SOURCE_DIR")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.h.in ${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.h @ONLY)

MESSAGE("MENGINE_ENGINE_GIT_SHA1: ${MENGINE_ENGINE_GIT_SHA1}")

set(MENGINE_BUILD_VERSION "Versionless" CACHE STRING "MENGINE_BUILD_VERSION")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BuildVersion.h.in ${CMAKE_CURRENT_SOURCE_DIR}/BuildVersion.h @ONLY)

MESSAGE("MENGINE_BUILD_VERSION: ${MENGINE_BUILD_VERSION}")

string(TIMESTAMP MENGINE_BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

set(MENGINE_BUILD_USERNAME $ENV{USERNAME} CACHE STRING "MENGINE_BUILD_USERNAME")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/BuildInfo.h.in ${CMAKE_CURRENT_SOURCE_DIR}/BuildInfo.h @ONLY)

MESSAGE("MENGINE_BUILD_USERNAME: ${MENGINE_BUILD_USERNAME}")
MESSAGE("MENGINE_BUILD_TIMESTAMP: ${MENGINE_BUILD_TIMESTAMP}")

ADD_FILTER(
src
    Char.h
    Utf8.h
    Typedef.h
    Typeinfo.h
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
    Limits.h
    UInt32ToPointer.h
    Metaprogramming.h
    GitSHA1.h
    BuildInfo.h
    BuildVersion.h
    UniqueId.h
    Algorithm.h
    Utils.h
)

ADD_MENGINE_INTERFACE()