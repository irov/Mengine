get_filename_component(TILED_MAP_SHARED_SHADERS_PATH "${MENGINE_DEPLOY_PATH}/../../shaders" ABSOLUTE)
get_filename_component(TILED_MAP_DOWNLOADS_PATH "${MENGINE_DEPLOY_PATH}/.downloads/tiled/examples" ABSOLUTE)

if(NOT EXISTS "${TILED_MAP_SHARED_SHADERS_PATH}/Pak.json")
    message(FATAL_ERROR "TiledMap shared shaders package not found '${TILED_MAP_SHARED_SHADERS_PATH}/Pak.json'")
endif()

if(NOT EXISTS "${TILED_MAP_DOWNLOADS_PATH}/desert.tmx")
    message(FATAL_ERROR "TiledMap official examples not found '${TILED_MAP_DOWNLOADS_PATH}'. Run build/downloads/downloads first")
endif()

file(GLOB TILED_MAP_SHARED_SHADER_MATERIALS
    "${TILED_MAP_SHARED_SHADERS_PATH}/Materials/*.json"
    "${TILED_MAP_SHARED_SHADERS_PATH}/Materials/*.txt"
    "${TILED_MAP_SHARED_SHADERS_PATH}/Materials/*.hlsl"
)

set(TILED_MAP_BUNDLE_DATA_PATH "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/Data")
set(TILED_MAP_BUNDLE_SHADERS_PATH "${TILED_MAP_BUNDLE_DATA_PATH}/shaders")
set(TILED_MAP_BUNDLE_EXAMPLES_PATH "${TILED_MAP_BUNDLE_DATA_PATH}/tiled")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${TILED_MAP_BUNDLE_SHADERS_PATH}/Materials"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${TILED_MAP_BUNDLE_EXAMPLES_PATH}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${TILED_MAP_DOWNLOADS_PATH}"
        "${TILED_MAP_BUNDLE_EXAMPLES_PATH}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${TILED_MAP_SHARED_SHADERS_PATH}/Pak.json"
        "${TILED_MAP_BUNDLE_SHADERS_PATH}/Pak.json"
)

foreach(TILED_MAP_SHARED_SHADER_MATERIAL ${TILED_MAP_SHARED_SHADER_MATERIALS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${TILED_MAP_SHARED_SHADER_MATERIAL}"
            "${TILED_MAP_BUNDLE_SHADERS_PATH}/Materials"
    )
endforeach()
