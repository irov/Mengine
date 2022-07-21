MENGINE_PROJECT(AstralaxPlugin)

ADD_FILTER(
src
    AstralaxInterface.h
    
    AstralaxIncluder.h
    
    AstralaxPlugin.cpp
    AstralaxPlugin.h
    AstralaxPlugin.def

    AstralaxModule.cpp
    AstralaxModule.h
    AstralaxEmitter.cpp
    AstralaxEmitter.h
    AstralaxEmitter2.cpp
    AstralaxEmitter2.h
    AstralaxEmitterContainer.cpp
    AstralaxEmitterContainer.h
    ResourceAstralax.cpp
    ResourceAstralax.h
    LoaderResourceAstralax.cpp
    LoaderResourceAstralax.h
    AstralaxService.h
    AstralaxService.cpp
    
    ResourceAstralaxValidator.cpp
    ResourceAstralaxValidator.h
    
    ParticleConverterPTCToPTZ.cpp
    ParticleConverterPTCToPTZ.h
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AstralaxScriptEmbedding.cpp
        AstralaxScriptEmbedding.h    
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

add_definitions(-DMAGIC_3D)
add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)

if(MENGINE_TARGET_IOS)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/astralax_ios)
endif()

if(MENGINE_TARGET_IOS_SIMULATOR)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/astralax_ios)
endif()

if(MENGINE_TARGET_ANDROID)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/astralax_android)
endif()

if(MENGINE_TARGET_MACOS)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/astralax_mac)
endif()

if(MENGINE_TARGET_WINDOWS)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/astralax)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ASTRALAX)

if(MENGINE_TOOLCHAIN_MINGW)
    if(MENGINE_ARCHITECTURE_X86)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax/libmagic3d.a)
    endif()
    
    if(MENGINE_ARCHITECTURE_X64)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MENGINE_CMAKE_DEPENDENCIES_DIR}/astralax_mingw64/libmagic3d.a)
    endif()
endif()

if(MENGINE_TOOLCHAIN_MSVC)
    if(MENGINE_ARCHITECTURE_X86)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax/magic3d.lib)
    endif()
    
    if(MENGINE_ARCHITECTURE_X64)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax/magic3d_x64.lib)
    endif()
endif()

if(MENGINE_TOOLCHAIN_XCODE AND MENGINE_TARGET_MACOS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax_mac/magic3d.dylib)
endif()

if(MENGINE_TOOLCHAIN_XCODE AND MENGINE_TARGET_IOS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax_ios/bitcode/libimagic3d.a)
endif()

if(MENGINE_TOOLCHAIN_XCODE AND MENGINE_TARGET_IOS_SIMULATOR)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax_ios/libimagic3d.a)
endif()

if(MENGINE_TARGET_ANDROID)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax_android/3d/a/${ANDROID_ABI}/libamagic3d.a)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

if(MENGINE_TARGET_ANDROID)
    ADD_LIBRARY_IMPORTED(amagic3d "${THIRDPARTY_DIR}/astralax_android/3d/so/${ANDROID_ABI}/libamagic3d.so")
endif()