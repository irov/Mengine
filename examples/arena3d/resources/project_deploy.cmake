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

# The generated package can gain new files after the Xcode project was first
# configured. Copy the package directory after every application build so an
# incremental build never launches with an older manifest or missing layers.
set(ARENA3D_GENERATED_PACKAGE_PATH "${MENGINE_DEPLOY_PATH}/Data/generated")
set(ARENA3D_BUNDLE_GENERATED_PATH "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/Data/generated")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ARENA3D_BUNDLE_GENERATED_PATH}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ARENA3D_GENERATED_PACKAGE_PATH}" "${ARENA3D_BUNDLE_GENERATED_PATH}")

foreach(ARENA3D_SHADER ${ARENA3D_SHARED_SHADER_MATERIALS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ARENA3D_SHADER}" "${ARENA3D_BUNDLE_SHADERS_PATH}/Materials")
endforeach()
