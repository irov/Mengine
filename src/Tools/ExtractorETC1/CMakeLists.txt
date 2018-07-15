set( MY_LIB_NAME ExtractorETC1 )

ADD_FILTER(
src
    ExtractorETC1.cpp
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils
                    )

LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX}
    ToolUtils    
    )
    
set_target_properties( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS /NODEFAULTLIB:"libci.lib" )    