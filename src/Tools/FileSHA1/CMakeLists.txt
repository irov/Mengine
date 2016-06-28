set( MY_LIB_NAME FileSHA1 )

ADD_FILTER(
src
    FileSHA1.cpp
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils		
                    ${THIRDPARTY_DIR}/stdex/include
                    )

LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX}
    )   
