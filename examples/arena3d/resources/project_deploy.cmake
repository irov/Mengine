get_filename_component(ARENA3D_SHARED_SHADERS_PATH "${MENGINE_DEPLOY_PATH}/../../shaders" ABSOLUTE)

if(NOT EXISTS "${ARENA3D_SHARED_SHADERS_PATH}/Pak.json")
    message(FATAL_ERROR "Arena3D shared shaders package not found: ${ARENA3D_SHARED_SHADERS_PATH}")
endif()

file(GLOB ARENA3D_SHARED_SHADER_MATERIALS
    "${ARENA3D_SHARED_SHADERS_PATH}/Materials/*.json"
    "${ARENA3D_SHARED_SHADERS_PATH}/Materials/*.txt"
    "${ARENA3D_SHARED_SHADERS_PATH}/Materials/*.hlsl")

set(ARENA3D_BUNDLE_SHADERS_PATH "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/Data/shaders")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ARENA3D_BUNDLE_SHADERS_PATH}/Materials"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ARENA3D_SHARED_SHADERS_PATH}/Pak.json" "${ARENA3D_BUNDLE_SHADERS_PATH}/Pak.json")

foreach(ARENA3D_SHADER ${ARENA3D_SHARED_SHADER_MATERIALS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ARENA3D_SHADER}" "${ARENA3D_BUNDLE_SHADERS_PATH}/Materials")
endforeach()
