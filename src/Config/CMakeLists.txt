MENGINE_PROJECT(Config)

list(APPEND CMAKE_MODULE_PATH "${THIRDPARTY_DIR}/cmake-modules/")
include(GetGitRevisionDescription)

get_git_head_revision(GIT_REFSPEC GIT_SHA1)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.h.in "${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.h" @ONLY)

ADD_FILTER(
src
    Char.h
    Typedef.h
    Atomic.h
    Lambda.h
    Define.h
    Config.h
    Export.h
    Secure.h
    StdInt.h
    StdDef.h
    StdIntTypes.h
    StdArg.h
    StdIO.h
    StdLib.h
    StdString.h
    StdBaseTsd.h
    Metaprogramming.h
    GitSHA1.h
    cmake_dummy.cpp
)

ADD_MENGINE_LIBRARY()