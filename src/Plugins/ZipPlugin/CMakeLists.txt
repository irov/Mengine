set(MY_LIB_NAME ZipPlugin)

ADD_FILTER(
src
    ZipPlugin.h
    ZipPlugin.cpp   
    FileGroupZip.h
    FileGroupZip.cpp
    ArchivatorZip.h
    ArchivatorZip.cpp    
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/zlib)

ADD_MENGINE_LIBRARY(Plugins)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX})