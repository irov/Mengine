SET(MY_LIB_NAME PackageService)

ADD_FILTER(
src
	PackageService.h
	PackageService.cpp
    Package.h
    Package.cpp
)

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)

ADD_MENGINE_LIBRARY(Services)