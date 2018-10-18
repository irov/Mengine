set( MY_LIB_NAME MovieCompiler )

ADD_FILTER(
src
    MovieCompiler.cpp
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

INCLUDE_DIRECTORIES( 
			         ${THIRDPARTY_DIR}/astralax
                     ${THIRDPARTY_DIR}/lz4/lib
                     ${THIRDPARTY_DIR}/libmovie/include
                    )

LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX}    
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}lz4${MENGINE_LIB_SUFFIX}
    ToolUtils
    )
    
set_target_properties( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS /NODEFAULTLIB:"libci.lib" )    