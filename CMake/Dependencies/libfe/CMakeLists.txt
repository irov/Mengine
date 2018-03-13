CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(fe)

SET( SOURCE_DIR ${THIRDPARTY_DIR}/libfe )

# OUR SOURCES
set( sources
	${SOURCE_DIR}/include/fe/fe_bundle.h
	${SOURCE_DIR}/include/fe/fe_effect.h
	${SOURCE_DIR}/include/fe/fe_export.h
	${SOURCE_DIR}/include/fe/fe_gradient.h
	${SOURCE_DIR}/include/fe/fe_image.h
	${SOURCE_DIR}/include/fe/fe_node.h
    ${SOURCE_DIR}/src/fe_image.cpp
	${SOURCE_DIR}/src/fe_effect.cpp
	${SOURCE_DIR}/src/fe_node.cpp
	${SOURCE_DIR}/src/fe_node_custom.cpp
	${SOURCE_DIR}/src/fe_bundle.cpp
	${SOURCE_DIR}/src/fe_gradient.cpp
	${SOURCE_DIR}/src/ImageData.cpp
	${SOURCE_DIR}/src/ImageDataOperations.cpp
)

include_directories(${SOURCE_DIR}/include)

add_library( ${PROJECT_NAME} STATIC ${sources} )

