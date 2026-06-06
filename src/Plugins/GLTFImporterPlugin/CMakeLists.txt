MENGINE_PROJECT(GLTFImporterPlugin)

ADD_FILTER(
src
    GLTFImporterPlugin.cpp
    GLTFImporterPlugin.def
    GLTFImporterPlugin.h

    GLTFImporterInterface.h
    GLTFImporterService.cpp
    GLTFImporterService.h

    cgltf_impl.cpp
)

if(DEFINED CGLTF_INCLUDE_DIR)
    INCLUDE_DIRECTORIES(${CGLTF_INCLUDE_DIR})
else()
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/cgltf)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_GLTF_IMPORTER)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
