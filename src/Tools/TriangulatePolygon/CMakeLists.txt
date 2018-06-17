set( MY_LIB_NAME TriangulatePolygon )

ADD_FILTER(
src
    TriangulatePolygon.cpp
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

INCLUDE_DIRECTORIES( 
	${THIRDPARTY_DIR}/boost/libs/config/include
	${THIRDPARTY_DIR}/boost/libs/assert/include
	${THIRDPARTY_DIR}/boost/libs/preprocessor/include
	${THIRDPARTY_DIR}/boost/libs/predef/include
	${THIRDPARTY_DIR}/boost/libs/numeric/conversion/include
	${THIRDPARTY_DIR}/boost/libs/type_traits/include
	${THIRDPARTY_DIR}/boost/libs/mpl/include
	${THIRDPARTY_DIR}/boost/libs/math/include
	${THIRDPARTY_DIR}/boost/libs/iterator/include
	${THIRDPARTY_DIR}/boost/libs/range/include
	${THIRDPARTY_DIR}/boost/libs/static_assert/include
	${THIRDPARTY_DIR}/boost/libs/throw_exception/include
	${THIRDPARTY_DIR}/boost/libs/throw_detail/include
	${THIRDPARTY_DIR}/boost/libs/core/include
	${THIRDPARTY_DIR}/boost/libs/detail/include
	${THIRDPARTY_DIR}/boost/libs/utility/include
	${THIRDPARTY_DIR}/boost/libs/integer/include
	${THIRDPARTY_DIR}/boost/libs/array/include
	${THIRDPARTY_DIR}/boost/libs/smart_ptr/include
	${THIRDPARTY_DIR}/boost/libs/concept_check/include
	${THIRDPARTY_DIR}/boost/libs/move/include
	${THIRDPARTY_DIR}/boost/libs/format/include
	${THIRDPARTY_DIR}/boost/libs/optional/include
	${THIRDPARTY_DIR}/boost/libs/container/include
	${THIRDPARTY_DIR}/boost/libs/variant/include
	${THIRDPARTY_DIR}/boost/libs/tuple/include
	${THIRDPARTY_DIR}/boost/libs/rational/include
	${THIRDPARTY_DIR}/boost/libs/algorithm/include
	${THIRDPARTY_DIR}/boost/libs/function/include
	${THIRDPARTY_DIR}/boost/libs/type_index/include
	${THIRDPARTY_DIR}/boost/libs/container_hash/include
	${THIRDPARTY_DIR}/boost/libs/bind/include
	${THIRDPARTY_DIR}/boost/libs/function_types/include
	${THIRDPARTY_DIR}/boost/libs/intrusive/include
	${THIRDPARTY_DIR}/boost/libs/tokenizer/include
	${THIRDPARTY_DIR}/boost/libs/geometry/include
	${THIRDPARTY_DIR}/boost/libs/lexical_cast/include
	${THIRDPARTY_DIR}/boost/libs/qvm/include
	${THIRDPARTY_DIR}/boost/libs/exception/include
	${THIRDPARTY_DIR}/poly2tri
	)

LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ToolUtils)

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}poly2tri${MENGINE_LIB_SUFFIX}
    )   

set_target_properties( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS /NODEFAULTLIB:"libci.lib" ) 