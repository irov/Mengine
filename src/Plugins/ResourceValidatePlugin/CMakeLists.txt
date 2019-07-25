set(MY_LIB_NAME ResourceValidatePlugin)

ADD_FILTER(
src
    ResourceValidatorInterface.h
    ResourceValidateServiceInterface.h

	ResourceValidatePlugin.h
    ResourceValidatePlugin.def
	ResourceValidatePlugin.cpp
    
    ResourceValidateService.h
    ResourceValidateService.cpp
    
    ResourceValidatorHelper.h
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

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/stdex/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)