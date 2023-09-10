MENGINE_PROJECT(ResourceValidatePlugin)

ADD_FILTER(
src
    ResourceValidateServiceInterface.h

    ResourceValidatePlugin.h
    ResourceValidatePlugin.def
    ResourceValidatePlugin.cpp
    
    ResourceValidateService.h
    ResourceValidateService.cpp
    
    ResourceImageDefaultValidator.h
    ResourceImageDefaultValidator.cpp
    ResourceImageDataValidator.h
    ResourceImageDataValidator.cpp
    ResourceImageSequenceValidator.h
    ResourceImageSequenceValidator.cpp
    ResourceMusicValidator.h
    ResourceMusicValidator.cpp
    ResourceSoundValidator.h
    ResourceSoundValidator.cpp
    ResourceHITValidator.h
    ResourceHITValidator.cpp
    ResourceFileValidator.h
    ResourceFileValidator.cpp
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_RESOURCEVALIDATE)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)