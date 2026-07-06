get_filename_component(VIRTUAL_AREA_SHARED_SHADERS_PATH "${MENGINE_DEPLOY_PATH}/../../shaders" ABSOLUTE)

if(NOT EXISTS "${VIRTUAL_AREA_SHARED_SHADERS_PATH}/Pak.json")
    message(FATAL_ERROR "VirtualArea shared shaders package not found '${VIRTUAL_AREA_SHARED_SHADERS_PATH}/Pak.json'")
endif()

file(GLOB VIRTUAL_AREA_SHARED_SHADER_MATERIALS
    "${VIRTUAL_AREA_SHARED_SHADERS_PATH}/Materials/*.json"
    "${VIRTUAL_AREA_SHARED_SHADERS_PATH}/Materials/*.txt"
)

set(VIRTUAL_AREA_BUNDLE_SHADERS_PATH "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/Data/shaders")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${VIRTUAL_AREA_BUNDLE_SHADERS_PATH}/Materials"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VIRTUAL_AREA_SHARED_SHADERS_PATH}/Pak.json"
        "${VIRTUAL_AREA_BUNDLE_SHADERS_PATH}/Pak.json"
)

foreach(VIRTUAL_AREA_SHARED_SHADER_MATERIAL ${VIRTUAL_AREA_SHARED_SHADER_MATERIALS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${VIRTUAL_AREA_SHARED_SHADER_MATERIAL}"
            "${VIRTUAL_AREA_BUNDLE_SHADERS_PATH}/Materials"
    )
endforeach()
