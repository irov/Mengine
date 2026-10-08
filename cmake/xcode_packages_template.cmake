if("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_FILE}")
    cmake_minimum_required(VERSION 3.24)
endif()

function(_MENGINE_XCODE_QUOTE OUTPUT VALUE)
    string(REPLACE "\\" "\\\\" VALUE "${VALUE}")
    string(REPLACE "\"" "\\\"" VALUE "${VALUE}")
    string(REPLACE "\n" "\\n" VALUE "${VALUE}")
    string(REPLACE "\r" "\\r" VALUE "${VALUE}")
    string(REPLACE "\t" "\\t" VALUE "${VALUE}")
    SET(${OUTPUT} "\"${VALUE}\"" PARENT_SCOPE)
endfunction()

function(_MENGINE_XCODE_PACKAGE_RECORDS OUTPUT LIST_VAR)
    SET(BUFFER "")
    list(LENGTH ${LIST_VAR} LENGTH)

    if(LENGTH GREATER 0)
        math(EXPR LAST "${LENGTH}-1")

        foreach(INDEX RANGE 0 ${LAST} 4)
            SET(RECORD "")
            math(EXPR END "${INDEX}+3")

            foreach(FIELD_INDEX RANGE ${INDEX} ${END})
                list(GET ${LIST_VAR} ${FIELD_INDEX} FIELD)
                string(REPLACE "($)" "$" FIELD "${FIELD}")
                _MENGINE_XCODE_QUOTE(FIELD "${FIELD}")
                list(APPEND RECORD "${FIELD}")
            endforeach()

            list(JOIN RECORD "," RECORD)
            list(APPEND BUFFER "[${RECORD}]")
        endforeach()
    endif()

    list(JOIN BUFFER ",\n" BUFFER)
    SET(${OUTPUT} "${BUFFER}" PARENT_SCOPE)
endfunction()

MACRO(MENGINE_GENERATE_SWIFT_PACKAGES)
    FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.xcworkspace")
    FILE(WRITE "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.xcworkspace/contents.xcworkspacedata"
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<Workspace version=\"1.0\">\n  <FileRef location=\"group:${CMAKE_PROJECT_NAME}.xcodeproj\"/>\n</Workspace>\n")

    if(APPLICATION_APPLE_SWIFT_PACKAGES OR APPLICATION_APPLE_SCRIPT_PHASES)
        _MENGINE_XCODE_PACKAGE_RECORDS(SWIFT_PACKAGES APPLICATION_APPLE_SWIFT_PACKAGES)
        _MENGINE_XCODE_PACKAGE_RECORDS(SCRIPT_PHASES APPLICATION_APPLE_SCRIPT_PHASES)

        _MENGINE_XCODE_QUOTE(APPLICATION "${PROJECT_NAME}")
        _MENGINE_XCODE_QUOTE(XCODE_PROJECT "${CMAKE_PROJECT_NAME}")
        FILE(WRITE "${CMAKE_BINARY_DIR}/mengine_xcode_packages.json"
            "{\"application\":${APPLICATION},\"project\":${XCODE_PROJECT},\"packages\":[${SWIFT_PACKAGES}],\"scripts\":[${SCRIPT_PHASES}]}\n")

        # Xcode package references are added after generation by the make-solution scripts.
        # Automatic regeneration would overwrite them; rerun make-solution after CMake changes.
        if(NOT DEFINED MENGINE_XCODE_PACKAGES_PREVIOUS_REGENERATION)
            SET(MENGINE_XCODE_PACKAGES_PREVIOUS_REGENERATION "${CMAKE_SUPPRESS_REGENERATION}"
                CACHE INTERNAL "Regeneration setting before native Xcode integration")
        endif()

        SET(CMAKE_SUPPRESS_REGENERATION ON CACHE BOOL "Disable automatic CMake regeneration" FORCE)
    else()
        FILE(REMOVE "${CMAKE_BINARY_DIR}/mengine_xcode_packages.json")

        if(DEFINED MENGINE_XCODE_PACKAGES_PREVIOUS_REGENERATION)
            SET(CMAKE_SUPPRESS_REGENERATION "${MENGINE_XCODE_PACKAGES_PREVIOUS_REGENERATION}"
                CACHE BOOL "Disable automatic CMake regeneration" FORCE)
            UNSET(MENGINE_XCODE_PACKAGES_PREVIOUS_REGENERATION CACHE)
        endif()
    endif()
ENDMACRO()

# Keep deterministic identifiers so reapplying integration does not duplicate objects.
function(_MENGINE_XCODE_ADD_OBJECT OUTPUT KEY VALUE)
    string(SHA256 ${OUTPUT} "${KEY}")
    string(SUBSTRING "${${OUTPUT}}" 0 24 ${OUTPUT})
    string(TOUPPER "${${OUTPUT}}" ${OUTPUT})
    string(JSON OLD_TYPE ERROR_VARIABLE ERROR GET "${PROJECT}" objects "${${OUTPUT}}" isa)
    string(JSON NEW_TYPE GET "${VALUE}" isa)

    if(NOT ERROR AND NOT OLD_TYPE STREQUAL NEW_TYPE)
        message(FATAL_ERROR "Xcode object identifier collision: ${${OUTPUT}}")
    endif()

    string(JSON PROJECT SET "${PROJECT}" objects "${${OUTPUT}}" "${VALUE}")
    SET(${OUTPUT} "${${OUTPUT}}" PARENT_SCOPE)
    SET(PROJECT "${PROJECT}" PARENT_SCOPE)
endfunction()

function(_MENGINE_XCODE_APPEND OBJECT FIELD IDENTIFIER)
    string(JSON REFERENCES ERROR_VARIABLE ERROR GET "${PROJECT}" objects "${OBJECT}" "${FIELD}")

    if(ERROR)
        SET(REFERENCES "[]")
    endif()

    string(FIND "${REFERENCES}" "\"${IDENTIFIER}\"" INDEX)

    if(INDEX EQUAL -1)
        string(JSON COUNT LENGTH "${REFERENCES}")
        string(JSON REFERENCES SET "${REFERENCES}" ${COUNT} "\"${IDENTIFIER}\"")
        string(JSON PROJECT SET "${PROJECT}" objects "${OBJECT}" "${FIELD}" "${REFERENCES}")
        SET(PROJECT "${PROJECT}" PARENT_SCOPE)
    endif()
endfunction()

function(_MENGINE_XCODE_PREPARE_PACKAGES SOLUTION_DIR)
    if(SOLUTION_DIR STREQUAL "")
        message(FATAL_ERROR "Set MENGINE_XCODE_SOLUTION_DIR to the Xcode solution directory")
    endif()

    get_filename_component(SOLUTION_DIR "${SOLUTION_DIR}" ABSOLUTE)

    file(READ "${SOLUTION_DIR}/mengine_xcode_packages.json" CONFIGURATION)
    string(JSON PROJECT_NAME GET "${CONFIGURATION}" project)
    SET(PROJECT_FILE "${SOLUTION_DIR}/${PROJECT_NAME}.xcodeproj/project.pbxproj")
    execute_process(COMMAND plutil -convert json -o - "${PROJECT_FILE}"
        OUTPUT_VARIABLE PROJECT COMMAND_ERROR_IS_FATAL ANY)
    string(JSON PROJECT GET "${PROJECT}")
    SET(ORIGINAL_PROJECT "${PROJECT}")
    string(JSON ROOT GET "${PROJECT}" rootObject)
    string(JSON APPLICATION_NAME GET "${CONFIGURATION}" application)
    string(JSON TARGET_COUNT LENGTH "${PROJECT}" objects "${ROOT}" targets)
    math(EXPR TARGET_LAST "${TARGET_COUNT}-1")

    foreach(INDEX RANGE 0 ${TARGET_LAST})
        string(JSON TARGET GET "${PROJECT}" objects "${ROOT}" targets ${INDEX})
        string(JSON NAME GET "${PROJECT}" objects "${TARGET}" name)
        SET("TARGET_${NAME}" "${TARGET}")
    endforeach()

    SET(APPLICATION "${TARGET_${APPLICATION_NAME}}")

    if(APPLICATION STREQUAL "")
        message(FATAL_ERROR "Xcode application target not found: ${APPLICATION_NAME}")
    endif()

    string(JSON PACKAGE_COUNT LENGTH "${CONFIGURATION}" packages)

    if(PACKAGE_COUNT GREATER 0)
        # Project-wide SYMROOT selects legacy build locations, which reject Swift packages.
        # Target CONFIGURATION_BUILD_DIR settings still preserve Mengine's output paths.
        string(JSON CONFIGURATION_LIST GET "${PROJECT}" objects "${ROOT}" buildConfigurationList)
        string(JSON COUNT LENGTH "${PROJECT}" objects "${CONFIGURATION_LIST}" buildConfigurations)
        math(EXPR LAST "${COUNT}-1")

        foreach(INDEX RANGE 0 ${LAST})
            string(JSON BUILD_CONFIGURATION GET "${PROJECT}" objects "${CONFIGURATION_LIST}" buildConfigurations ${INDEX})

            string(JSON SYMROOT ERROR_VARIABLE ERROR GET "${PROJECT}" objects "${BUILD_CONFIGURATION}" buildSettings SYMROOT)

            if(NOT ERROR)
                string(JSON PROJECT REMOVE "${PROJECT}" objects "${BUILD_CONFIGURATION}" buildSettings SYMROOT)
            endif()
        endforeach()

        math(EXPR PACKAGE_LAST "${PACKAGE_COUNT}-1")

        foreach(PACKAGE_INDEX RANGE 0 ${PACKAGE_LAST})
            string(JSON OWNER GET "${CONFIGURATION}" packages ${PACKAGE_INDEX} 0)
            string(JSON PRODUCT_NAME GET "${CONFIGURATION}" packages ${PACKAGE_INDEX} 1)
            string(JSON REPOSITORY GET "${CONFIGURATION}" packages ${PACKAGE_INDEX} 2)
            string(JSON VERSION GET "${CONFIGURATION}" packages ${PACKAGE_INDEX} 3)

            if(DEFINED "VERSION_${REPOSITORY}" AND NOT "${VERSION_${REPOSITORY}}" STREQUAL VERSION)
                message(FATAL_ERROR "Conflicting package versions: ${REPOSITORY}")
            endif()

            SET("VERSION_${REPOSITORY}" "${VERSION}")
            _MENGINE_XCODE_QUOTE(REPOSITORY_JSON "${REPOSITORY}")
            _MENGINE_XCODE_QUOTE(VERSION_JSON "${VERSION}")
            _MENGINE_XCODE_ADD_OBJECT(PACKAGE "package:${REPOSITORY}"
                "{\"isa\":\"XCRemoteSwiftPackageReference\",\"repositoryURL\":${REPOSITORY_JSON},\"requirement\":{\"kind\":\"exactVersion\",\"version\":${VERSION_JSON}}}")
            _MENGINE_XCODE_APPEND("${ROOT}" packageReferences "${PACKAGE}")
            _MENGINE_XCODE_QUOTE(PRODUCT_JSON "${PRODUCT_NAME}")

            SET(TARGET_NAMES "${OWNER}")

            if("${TARGET_${OWNER}}" STREQUAL "")
                message(FATAL_ERROR "Xcode package target not found: ${OWNER}")
            endif()

            string(JSON OWNER_TYPE GET "${PROJECT}" objects "${TARGET_${OWNER}}" productType)

            if(NOT OWNER_TYPE STREQUAL "com.apple.product-type.app-extension")
                list(APPEND TARGET_NAMES "${APPLICATION_NAME}")
                list(REMOVE_DUPLICATES TARGET_NAMES)
            endif()

            foreach(NAME IN LISTS TARGET_NAMES)
                SET(TARGET "${TARGET_${NAME}}")

                if("${TARGET}" STREQUAL "")
                    message(FATAL_ERROR "Xcode package target not found: ${NAME}")
                endif()

                _MENGINE_XCODE_ADD_OBJECT(PRODUCT "product:${NAME}:${PRODUCT_NAME}"
                    "{\"isa\":\"XCSwiftPackageProductDependency\",\"package\":\"${PACKAGE}\",\"productName\":${PRODUCT_JSON}}")
                string(JSON TYPE GET "${PROJECT}" objects "${TARGET}" productType)

                # Package headers and resources live with package products, including app extensions.
                string(JSON CONFIGURATION_LIST GET "${PROJECT}" objects "${TARGET}" buildConfigurationList)
                string(JSON COUNT LENGTH "${PROJECT}" objects "${CONFIGURATION_LIST}" buildConfigurations)
                math(EXPR LAST "${COUNT}-1")

                foreach(INDEX RANGE 0 ${LAST})
                    string(JSON BUILD_CONFIGURATION GET "${PROJECT}" objects "${CONFIGURATION_LIST}" buildConfigurations ${INDEX})
                    string(JSON PROJECT SET "${PROJECT}" objects "${BUILD_CONFIGURATION}" buildSettings BUILT_PRODUCTS_DIR
                        "\"$(BUILD_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)\"")
                endforeach()

                if(TYPE STREQUAL "com.apple.product-type.library.static")
                    # Order compilation after the SDK without embedding it in each plugin archive.
                    _MENGINE_XCODE_ADD_OBJECT(DEPENDENCY "dependency:${NAME}:${PRODUCT_NAME}"
                        "{\"isa\":\"PBXTargetDependency\",\"productRef\":\"${PRODUCT}\"}")
                    _MENGINE_XCODE_APPEND("${TARGET}" dependencies "${DEPENDENCY}")
                    continue()
                endif()

                _MENGINE_XCODE_APPEND("${TARGET}" packageProductDependencies "${PRODUCT}")
                _MENGINE_XCODE_ADD_OBJECT(FRAMEWORK "framework:${NAME}:${PRODUCT_NAME}"
                    "{\"isa\":\"PBXBuildFile\",\"productRef\":\"${PRODUCT}\"}")
                string(JSON COUNT LENGTH "${PROJECT}" objects "${TARGET}" buildPhases)
                SET(FRAMEWORKS_PHASE "")

                if(COUNT GREATER 0)
                    math(EXPR LAST "${COUNT}-1")

                    foreach(INDEX RANGE 0 ${LAST})
                        string(JSON PHASE GET "${PROJECT}" objects "${TARGET}" buildPhases ${INDEX})
                        string(JSON TYPE GET "${PROJECT}" objects "${PHASE}" isa)

                        if(TYPE STREQUAL "PBXFrameworksBuildPhase")
                            SET(FRAMEWORKS_PHASE "${PHASE}")
                            break()
                        endif()
                    endforeach()
                endif()

                if(FRAMEWORKS_PHASE STREQUAL "")
                    _MENGINE_XCODE_ADD_OBJECT(FRAMEWORKS_PHASE "frameworks:${NAME}"
                        "{\"isa\":\"PBXFrameworksBuildPhase\",\"files\":[],\"buildActionMask\":\"2147483647\",\"runOnlyForDeploymentPostprocessing\":\"0\"}")
                    _MENGINE_XCODE_APPEND("${TARGET}" buildPhases "${FRAMEWORKS_PHASE}")
                endif()

                _MENGINE_XCODE_APPEND("${FRAMEWORKS_PHASE}" files "${FRAMEWORK}")
            endforeach()
        endforeach()
    endif()

    string(JSON SCRIPT_COUNT LENGTH "${CONFIGURATION}" scripts)

    if(SCRIPT_COUNT GREATER 0)
        math(EXPR SCRIPT_LAST "${SCRIPT_COUNT}-1")

        foreach(SCRIPT_INDEX RANGE 0 ${SCRIPT_LAST})
            string(JSON NAME GET "${CONFIGURATION}" scripts ${SCRIPT_INDEX} 1)
            string(JSON SCRIPT GET "${CONFIGURATION}" scripts ${SCRIPT_INDEX} 2)
            string(JSON INPUT_FILES GET "${CONFIGURATION}" scripts ${SCRIPT_INDEX} 3)
            SET(INPUTS "[]")

            if(NOT INPUT_FILES STREQUAL "NO-INPUT-FILES")
                # Script-phase callers provide a bracketed list of single-quoted paths.
                if(NOT INPUT_FILES MATCHES "^\\[.*\\]$")
                    message(FATAL_ERROR "Invalid Xcode script input paths: ${NAME}")
                endif()

                string(REGEX MATCHALL "'[^']*'" PATHS "${INPUT_FILES}")
                string(REGEX REPLACE "'[^']*'|[][ ,\t\r\n]" "" REMAINDER "${INPUT_FILES}")

                if(NOT REMAINDER STREQUAL "")
                    message(FATAL_ERROR "Invalid Xcode script input paths: ${NAME}")
                endif()

                foreach(PATH IN LISTS PATHS)
                    string(LENGTH "${PATH}" LENGTH)
                    math(EXPR LENGTH "${LENGTH}-2")
                    string(SUBSTRING "${PATH}" 1 ${LENGTH} PATH)
                    _MENGINE_XCODE_QUOTE(PATH "${PATH}")
                    string(JSON INDEX LENGTH "${INPUTS}")
                    string(JSON INPUTS SET "${INPUTS}" ${INDEX} "${PATH}")
                endforeach()
            endif()

            _MENGINE_XCODE_QUOTE(NAME_JSON "${NAME}")
            _MENGINE_XCODE_QUOTE(SCRIPT_JSON "${SCRIPT}")
            _MENGINE_XCODE_ADD_OBJECT(PHASE "script:${APPLICATION_NAME}:${NAME}"
                "{\"isa\":\"PBXShellScriptBuildPhase\",\"name\":${NAME_JSON},\"shellScript\":${SCRIPT_JSON},\"inputPaths\":${INPUTS},\"outputPaths\":[],\"files\":[],\"shellPath\":\"/bin/sh\",\"buildActionMask\":\"2147483647\",\"runOnlyForDeploymentPostprocessing\":\"0\",\"alwaysOutOfDate\":\"1\"}")
            _MENGINE_XCODE_APPEND("${APPLICATION}" buildPhases "${PHASE}")
        endforeach()
    endif()

    if(NOT PROJECT STREQUAL ORIGINAL_PROJECT)
        file(WRITE "${PROJECT_FILE}.tmp" "${PROJECT}")
        execute_process(COMMAND plutil -convert xml1 "${PROJECT_FILE}.tmp" COMMAND_ERROR_IS_FATAL ANY)
        file(RENAME "${PROJECT_FILE}.tmp" "${PROJECT_FILE}")
    endif()

    if(PACKAGE_COUNT GREATER 0)
        execute_process(COMMAND xcodebuild -resolvePackageDependencies
            -project "${SOLUTION_DIR}/${PROJECT_NAME}.xcodeproj"
            -scheme "${APPLICATION_NAME}"
            -clonedSourcePackagesDirPath "${SOLUTION_DIR}/SourcePackages"
            COMMAND_ERROR_IS_FATAL ANY)
    endif()
endfunction()

if("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_FILE}")
    _MENGINE_XCODE_PREPARE_PACKAGES("${MENGINE_XCODE_SOLUTION_DIR}")
endif()
