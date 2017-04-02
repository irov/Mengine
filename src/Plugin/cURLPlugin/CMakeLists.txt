set( MY_LIB_NAME cURLPlugin )

ADD_FILTER(
src
	cURLPlugin.cpp
	cURLPlugin.h
	cURLPlugin.def
	cURLRequestPerformer.cpp
	cURLRequestPerformer.h
	cURLRequestPerformerSync.h
	cURLRequestPerformerSync.cpp
	cURLRequestPerformerPostForm.h
	cURLRequestPerformerPostForm.cpp
	cURLRequestPerformerAsync.h
	cURLRequestPerformerAsync.cpp
	cURLUtils.cpp
	cURLUtils.h
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils 
					 ${THIRDPARTY_DIR}                 
					 ${THIRDPARTY_DIR}/libcurl/include
)

add_definitions(-DMENGE_PLUGIN_DLL)

ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )

SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}/EXPORT:dllCreatePlugin")

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory)

foreach(dependency ${APPLICATION_DEPENDENCIES})
	#MESSAGE( ${LIB_PREFIX}${dependency}${MENGINE_LIB_SUFFIX} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} debug ${dependency} )
	TARGET_LINK_LIBRARIES( ${MY_LIB_NAME} optimized ${dependency} )
endforeach(dependency ${APPLICATION_DEPENDENCIES})

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${CURL_STATIC_LIBS} )
