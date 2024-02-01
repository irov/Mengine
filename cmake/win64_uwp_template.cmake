set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")

SET(MENGINE_LIB_PREFIX "")
SET(MENGINE_LIB_SUFFIX ".lib")

set(CMAKE_DEBUG_POSTFIX "")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

IF(MENGINE_USE_PRECOMPILED_HEADER)
    set(MENGINE_C_DEBUG_INFORMATION_FORMAT "/Z7")
    set(MENGINE_CXX_DEBUG_INFORMATION_FORMAT "/Z7")
ELSE()
    set(MENGINE_C_DEBUG_INFORMATION_FORMAT "/ZI")
    set(MENGINE_CXX_DEBUG_INFORMATION_FORMAT "/ZI")
ENDIF()

SET(MENGINE_STATIC_CRT OFF CACHE BOOL "MENGINE_STATIC_CRT" FORCE)
SET(MENGINE_DYNAMIC_CRT ON CACHE BOOL "MENGINE_DYNAMIC_CRT" FORCE)

set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W4 /wd4121 /wd4250 /wd5105 /MP /EHsc /GR /UMBCS /D_UNICODE /DUNICODE")
set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG ${MENGINE_CXX_DEBUG_INFORMATION_FORMAT} /Od /RTC1")
set(CMAKE_CXX_FLAGS_RELEASE "/DNDEBUG /wd4702 /O2 /GL")

set(CMAKE_C_FLAGS "/DWIN32 /D_WINDOWS /W4 /wd4703 /wd4700 /wd4996")
set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG ${MENGINE_C_DEBUG_INFORMATION_FORMAT} /Od /RTC1")
set(CMAKE_C_FLAGS_RELEASE "/DNDEBUG /O2 /GL")

if(MENGINE_USE_PRECOMPILED_HEADER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm1000")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zm1000")
endif()

set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /DEBUG /SAFESEH:NO /LTCG:OFF /nodefaultlib:vccorlibd /nodefaultlib:msvcrtd vccorlibd.lib msvcrtd.lib")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /LTCG /nodefaultlib:vccorlib /nodefaultlib:msvcrt vccorlib.lib msvcrt.lib")

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG /SAFESEH:NO /LTCG:OFF /nodefaultlib:vccorlibd /nodefaultlib:msvcrtd vccorlibd.lib msvcrtd.lib")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG /nodefaultlib:vccorlib /nodefaultlib:msvcrt vccorlib.lib msvcrt.lib")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG /SAFESEH:NO /LTCG:OFF /nodefaultlib:vccorlibd /nodefaultlib:msvcrtd vccorlibd.lib msvcrtd.lib")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /nodefaultlib:vccorlib /nodefaultlib:msvcrt vccorlib.lib msvcrt.lib")

set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS}")
set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} /LTCG:OFF")
set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /LTCG")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MDd")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MD")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_ITERATOR_DEBUG_LEVEL=2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_ITERATOR_DEBUG_LEVEL=0")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_ITERATOR_DEBUG_LEVEL=2")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -D_ITERATOR_DEBUG_LEVEL=0")

add_definitions(-D_HAS_STD_BYTE=0)
add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)