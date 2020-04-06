MENGINE_PROJECT(AstralaxCompiler)

ADD_FILTER(
src
    AstralaxCompiler.cpp
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

add_definitions(-DMAGIC_3D)

INCLUDE_DIRECTORIES( 
			         ${THIRDPARTY_DIR}/astralax
                     ${THIRDPARTY_DIR}/lz4/lib
                     ${THIRDPARTY_DIR}/libmath/include
                    )

LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( ${PROJECT_NAME} WIN32 ${SRC_FILES} )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}lz4${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/astralax/magic3d.lib
    ToolUtils    
    )
    
set_target_properties( ${PROJECT_NAME} PROPERTIES LINK_FLAGS /NODEFAULTLIB:"libci.lib" )    