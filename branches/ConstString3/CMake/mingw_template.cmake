# SET LIBNAME
set(CMAKE_DEBUG_POSTFIX "d")

SET( MENGINE_LIB_PREFIX "" )
SET( MENGINE_LIB_SUFFIX ".lib" )

SET( MENGINE_LIB_POSTFIX ${MENGINE_LIB_SUFFIX} )

SET( MENGINE_LIB_SUFFIXD ${CMAKE_DEBUG_POSTFIX}${MENGINE_LIB_SUFFIX} )

set(CMAKE_CXX_FLAGS "-static-libstdc++ -static-libgcc")
add_definitions(-D"override = /* */")





