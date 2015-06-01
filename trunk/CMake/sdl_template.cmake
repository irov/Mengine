# SET LIBNAME
SET( MENGINE_LIB_PREFIX "" )
SET( MENGINE_LIB_SUFFIX ".lib" )

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)
if(LINUX)
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -Ob0 -Od")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    
    set(CMAKE_C_FLAGS "")
    set(CMAKE_C_FLAGS_DEBUG "-D_DEBUG -Ob0 -Od")
    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
        
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -MT")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -MT")

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MTd")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -MT")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -MT")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_SECURE_SCL=1 -D_HAS_ITERATOR_DEBUGGING=1")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0")

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_SECURE_SCL=1 -D_HAS_ITERATOR_DEBUGGING=1")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0")
endif()

if(WIN32)
    set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W4 /wd4481 /Zm1000 /MP /EHsc /GR")
    set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi /O2 /DNDEBUG")

    set(CMAKE_C_FLAGS "/DWIN32 /D_WINDOWS /W4 /wd4481 /Zm1000")
    set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /Zi /Ob0 /Od /RTC1")
    set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "/Zi /O2 /DNDEBUG")

    #set(CMAKE_EXE_LINKER_FLAGS "/STACK:10000000 /machine:I386")
    #set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:YES")
    #set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MT")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_SECURE_SCL=1 -D_HAS_ITERATOR_DEBUGGING=1")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0")

    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_SECURE_SCL=1 -D_HAS_ITERATOR_DEBUGGING=1")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0")
endif()

add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)





