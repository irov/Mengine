MENGINE_PROJECT(ZipPlugin)

ADD_FILTER(
src
    ZipPlugin.h
    ZipPlugin.def
    ZipPlugin.cpp
    
    FileGroupZip.h
    FileGroupZip.cpp
    ArchivatorZip.h
    ArchivatorZip.cpp
    ZipMemoryInputStream.h
    ZipMemoryInputStream.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/zlib)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/zlib)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ZIP)

if(WIN32)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zsd${MENGINE_LIB_SUFFIX})
else()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}z${MENGINE_LIB_SUFFIX})
endif()