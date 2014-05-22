set( MY_LIB_NAME AstralaxCompiler )

ADD_FILTER(
src
    AstralaxCompiler.cpp
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils
			         ${THIRDPARTY_DIR}/astralax
                    )

LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/astralax/win/lib/microsoft/magic.lib
    Shlwapi.lib
    )
    
set_target_properties( ${MY_LIB_NAME} PROPERTIES LINK_FLAGS /NODEFAULTLIB:"libci.lib" )    