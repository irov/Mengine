MENGINE_PROJECT(DevelopmentConverterPlugin)

ADD_FILTER(
src
    DevelopmentConverterPlugin.cpp
    DevelopmentConverterPlugin.def
    DevelopmentConverterPlugin.h
    
    HotspotImageConverterPNGToHIT.cpp
    HotspotImageConverterPNGToHIT.h
    SoundConverterFFMPEGToOGG.cpp
    SoundConverterFFMPEGToOGG.h
    VideoConverterFFMPEGToWEBM.cpp
    VideoConverterFFMPEGToWEBM.h
    VideoConverterFFMPEGToOGV.cpp
    VideoConverterFFMPEGToOGV.h
    VideoConverterFFMPEGToOGVA.cpp
    VideoConverterFFMPEGToOGVA.h
    ShaderConverterTextToPSO.cpp
    ShaderConverterTextToPSO.h
    ShaderConverterTextToVSO.cpp
    ShaderConverterTextToVSO.h
    ShaderConverterTextToPSO11.cpp
    ShaderConverterTextToPSO11.h
    ShaderConverterTextToVSO11.cpp
    ShaderConverterTextToVSO11.h
    MovieKeyConverterXMLToAEK.cpp
    MovieKeyConverterXMLToAEK.h
    ImageConverterPVRToHTF.cpp
    ImageConverterPVRToHTF.h
    ImageConverterDDSToHTF.cpp
    ImageConverterDDSToHTF.h
    ImageConverterPNGToACF.cpp
    ImageConverterPNGToACF.h    
)

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/poly2tri)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_DEVELOPMENTCONVERTER)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Metacode)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}lz4${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}poly2tri${MENGINE_LIB_SUFFIX})