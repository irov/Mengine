MENGINE_PROJECT(DevelopmentConverterPlugin)

ADD_FILTER(
src
    DevelopmentConverterPlugin.cpp
    DevelopmentConverterPlugin.def
    DevelopmentConverterPlugin.h

    DevelopmentConverterProcess.h
    DevelopmentConverterProcess.cpp

    MovieKeyConverterXMLToAEK.cpp
    MovieKeyConverterXMLToAEK.h
    MovieKeyFrameFormat.h
    
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
    ImageConverterPVRToHTF.cpp
    ImageConverterPVRToHTF.h
    ImageConverterDDSToHTF.cpp
    ImageConverterDDSToHTF.h
    ImageConverterPNGToACF.cpp
    ImageConverterPNGToACF.h
)

if(MENGINE_TARGET_WINDOWS)
    ADD_FILTER(
    win32
        ShaderConverterTextToPSO.cpp
        ShaderConverterTextToPSO.h
        ShaderConverterTextToVSO.cpp
        ShaderConverterTextToVSO.h
        ShaderConverterTextToVSO3.cpp
        ShaderConverterTextToVSO3.h
        ShaderConverterTextToPSO11.cpp
        ShaderConverterTextToPSO11.h
        ShaderConverterTextToVSO11.cpp
        ShaderConverterTextToVSO11.h
    )
endif()

add_definitions(-DMETABUF_EXTERNAL_CONFIG)
add_definitions(-DP2T_STATIC_EXPORTS)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/poly2tri)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/Clipper2/CPP/Clipper2Lib/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_DEVELOPMENTCONVERTER)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Metacode)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
if(MENGINE_TARGET_WINDOWS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Windows)
elseif(MENGINE_TARGET_MACOS OR MENGINE_TARGET_UNIX)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} POSIX)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}lz4${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}poly2tri${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}Clipper2${MENGINE_LIB_SUFFIX})
