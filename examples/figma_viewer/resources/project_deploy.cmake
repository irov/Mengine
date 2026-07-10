get_filename_component(FIGMA_VIEWER_SHARED_SHADERS_PATH "${MENGINE_DEPLOY_PATH}/../../shaders" ABSOLUTE)

if(NOT EXISTS "${FIGMA_VIEWER_SHARED_SHADERS_PATH}/Pak.json")
    message(FATAL_ERROR "FigmaViewer shared shaders package not found '${FIGMA_VIEWER_SHARED_SHADERS_PATH}/Pak.json'")
endif()

file(GLOB FIGMA_VIEWER_SHARED_SHADER_MATERIALS
    "${FIGMA_VIEWER_SHARED_SHADERS_PATH}/Materials/*.json"
    "${FIGMA_VIEWER_SHARED_SHADERS_PATH}/Materials/*.txt"
)

set(FIGMA_VIEWER_BUNDLE_SHADERS_PATH "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/Data/shaders")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${FIGMA_VIEWER_BUNDLE_SHADERS_PATH}/Materials"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FIGMA_VIEWER_SHARED_SHADERS_PATH}/Pak.json"
        "${FIGMA_VIEWER_BUNDLE_SHADERS_PATH}/Pak.json"
)

foreach(FIGMA_VIEWER_SHARED_SHADER_MATERIAL ${FIGMA_VIEWER_SHARED_SHADER_MATERIALS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FIGMA_VIEWER_SHARED_SHADER_MATERIAL}"
            "${FIGMA_VIEWER_BUNDLE_SHADERS_PATH}/Materials"
    )
endforeach()
