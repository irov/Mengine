SET( MENGINE_LIB_PREFIX "lib" )
SET( MENGINE_LIB_SUFFIX ".a" )

#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0 -Wno-warn-absolute-paths -std=c++11 -Wc++11-extensions")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0 -Wno-warn-absolute-paths -std=c++11 -Wc++11-extensions")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wno-warn-absolute-paths -std=c++11")


add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)


