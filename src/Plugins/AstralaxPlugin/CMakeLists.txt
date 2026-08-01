MENGINE_PROJECT(AstralaxPlugin)

ADD_FILTER(
src
    AstralaxInterface.h

    AstralaxIncluder.h

    AstralaxPlugin.cpp
    AstralaxPlugin.h
    AstralaxPlugin.def

    AstralaxEmitter.cpp
    AstralaxEmitter.h
    AstralaxEmitter2.cpp
    AstralaxEmitter2.h
    AstralaxEmitterContainer.cpp
    AstralaxEmitterContainer.h
    ResourceAstralax.cpp
    ResourceAstralax.h
    AstralaxService.h
    AstralaxService.cpp

    ResourceAstralaxValidator.cpp
    ResourceAstralaxValidator.h

    ParticleConverterPTCToPTZ.cpp
    ParticleConverterPTCToPTZ.h
)

if(MENGINE_PLUGIN_METABUF)
    ADD_FILTER(
    Metabuf
        MetabufLoaderResourceAstralax.cpp
        MetabufLoaderResourceAstralax.h
    )

    add_definitions(-DMETABUF_EXTERNAL_CONFIG)

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)
endif()

if(MENGINE_PLUGIN_JSON)
    ADD_FILTER(
    JSONLoader
        JSONLoaderResourceAstralax.cpp
        JSONLoaderResourceAstralax.h
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)
endif()

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        AstralaxScriptEmbedding.cpp
        AstralaxScriptEmbedding.h
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

if(MENGINE_TARGET_ANDROID)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/astralax_android/include)
elseif(MENGINE_TARGET_IOS)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/astralax_ios/include)
elseif(MENGINE_TARGET_IOS_SIMULATOR)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/astralax_ios/include)
elseif(MENGINE_TARGET_MACOS)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/astralax_macos/include)
elseif(MENGINE_TARGET_WINDOWS)
    if(MENGINE_TOOLCHAIN_MINGW AND MENGINE_ARCHITECTURE_X86)
        set(MENGINE_ASTRALAX_WINDOWS_DIR "${THIRDPARTY_DIR}/astralax_win32/mingw32")
    elseif(MENGINE_TOOLCHAIN_MINGW AND MENGINE_ARCHITECTURE_X64)
        set(MENGINE_ASTRALAX_WINDOWS_DIR "${THIRDPARTY_DIR}/astralax_win32/mingw64")
    else()
        message(FATAL_ERROR "Astralax Windows package does not contain binaries for the selected toolchain and architecture")
    endif()

    INCLUDE_DIRECTORIES(${MENGINE_ASTRALAX_WINDOWS_DIR}/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ASTRALAX)

if(MENGINE_TARGET_WINDOWS)
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE ASTRALAX_BUILD_SHARED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MENGINE_ASTRALAX_WINDOWS_DIR}/lib/libastralax.a)
elseif(MENGINE_TARGET_MACOS)
    ADD_MENGINE_MACOS_DYLIB_WITH_SIGN(${THIRDPARTY_DIR}/astralax_macos/lib/libastralax.dylib)
elseif(MENGINE_TARGET_IOS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax_ios/Astralax.xcframework/ios-arm64/libastralax.a)
elseif(MENGINE_TARGET_IOS_SIMULATOR)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/astralax_ios/Astralax.xcframework/ios-arm64_x86_64-simulator/libastralax.a)
elseif(MENGINE_TARGET_ANDROID)
    ADD_LIBRARY_IMPORTED(astralax_runtime ${THIRDPARTY_DIR}/astralax_android/lib/${ANDROID_ABI}/libastralax.so)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

if(MENGINE_PLUGIN_METABUF)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}metabuf${MENGINE_LIB_SUFFIX})
endif()

if(WIN32)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zs${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
else()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}z${MENGINE_LIB_SUFFIX})
endif()
