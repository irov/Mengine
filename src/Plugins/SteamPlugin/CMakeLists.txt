MENGINE_PROJECT(SteamPlugin)

ADD_FILTER(
src
    SteamPlugin.h
    SteamPlugin.cpp
    SteamPlugin.def
    
    SteamServiceInterface.h
    SteamService.cpp
    SteamService.h
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        SteamScriptEmbedding.cpp
        SteamScriptEmbedding.h
        )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/steam/public)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_STEAM)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

IF(MENGINE_TARGET_WINDOWS)
    IF(MENGINE_ARCHITECTURE_X86)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/steam/redistributable_bin/steam_api.lib)
    ENDIF()
    
    IF(MENGINE_ARCHITECTURE_X64)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_DIR}/steam/redistributable_bin/win64/steam_api64.lib)
    ENDIF()
ENDIF()

if(MENGINE_TARGET_MACOS)
	ADD_MENGINE_MACOS_DYLIB(${THIRDPARTY_DIR}/steam/redistributable_bin/osx/libsteam_api.dylib)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)