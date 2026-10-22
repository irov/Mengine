if("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_FILE}")
    cmake_minimum_required(VERSION 3.24)
endif()

function(_MENGINE_SWIFTPODS_QUOTE OUTPUT VALUE)
    string(REPLACE "\\" "\\\\" VALUE "${VALUE}")
    string(REPLACE "\"" "\\\"" VALUE "${VALUE}")
    string(REPLACE "\n" "\\n" VALUE "${VALUE}")
    string(REPLACE "\r" "\\r" VALUE "${VALUE}")
    string(REPLACE "\t" "\\t" VALUE "${VALUE}")
    SET(${OUTPUT} "\"${VALUE}\"" PARENT_SCOPE)
endfunction()

function(_MENGINE_SWIFTPODS_PACKAGE_RECORDS OUTPUT LIST_VAR)
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
                _MENGINE_SWIFTPODS_QUOTE(FIELD "${FIELD}")
                list(APPEND RECORD "${FIELD}")
            endforeach()

            list(JOIN RECORD "," RECORD)
            list(APPEND BUFFER "[${RECORD}]")
        endforeach()
    endif()

    list(JOIN BUFFER ",\n" BUFFER)
    SET(${OUTPUT} "${BUFFER}" PARENT_SCOPE)
endfunction()

MACRO(MENGINE_GENERATE_SWIFTPODS)
    FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.xcworkspace")
    FILE(WRITE "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.xcworkspace/contents.xcworkspacedata"
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<Workspace version=\"1.0\">\n  <FileRef location=\"group:${CMAKE_PROJECT_NAME}.xcodeproj\"/>\n</Workspace>\n")

    if(APPLICATION_APPLE_PODS OR APPLICATION_APPLE_SWIFT_PACKAGES OR APPLICATION_APPLE_SCRIPT_PHASES OR MENGINE_SWIFTPODS_INTEGRATED)
        _MENGINE_SWIFTPODS_PACKAGE_RECORDS(SWIFT_PACKAGES APPLICATION_APPLE_SWIFT_PACKAGES)
        _MENGINE_SWIFTPODS_PACKAGE_RECORDS(PODS APPLICATION_APPLE_PODS)
        SET(SOURCES "")

        foreach(SOURCE IN LISTS APPLICATION_APPLE_POD_SOURCES)
            _MENGINE_SWIFTPODS_QUOTE(SOURCE "${SOURCE}")
            list(APPEND SOURCES "${SOURCE}")
        endforeach()

        list(JOIN SOURCES "," SOURCES)
        _MENGINE_SWIFTPODS_PACKAGE_RECORDS(SCRIPT_PHASES APPLICATION_APPLE_SCRIPT_PHASES)

        _MENGINE_SWIFTPODS_QUOTE(APPLICATION "${PROJECT_NAME}")
        _MENGINE_SWIFTPODS_QUOTE(XCODE_PROJECT "${CMAKE_PROJECT_NAME}")
        find_program(MENGINE_SWIFTPODS_EXECUTABLE NAMES swiftpods DOC "SwiftPods executable")

        if(NOT MENGINE_SWIFTPODS_EXECUTABLE)
            message(FATAL_ERROR "SwiftPods is required for Apple SDK integration. Add swiftpods to PATH or set MENGINE_SWIFTPODS_EXECUTABLE.")
        endif()

        SET(SWIFTPODS_PLATFORM "osx")

        if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
            SET(SWIFTPODS_PLATFORM "ios")
        endif()

        _MENGINE_SWIFTPODS_QUOTE(SWIFTPODS_EXECUTABLE "${MENGINE_SWIFTPODS_EXECUTABLE}")
        _MENGINE_SWIFTPODS_QUOTE(DEPLOYMENT_TARGET "${CMAKE_OSX_DEPLOYMENT_TARGET}")
        FILE(WRITE "${CMAKE_BINARY_DIR}/mengine_swiftpods.json"
            "{\"application\":${APPLICATION},\"project\":${XCODE_PROJECT},\"swiftpods\":${SWIFTPODS_EXECUTABLE},\"platform\":\"${SWIFTPODS_PLATFORM}\",\"deployment_target\":${DEPLOYMENT_TARGET},\"pods\":[${PODS}],\"sources\":[${SOURCES}],\"packages\":[${SWIFT_PACKAGES}],\"scripts\":[${SCRIPT_PHASES}]}\n")

        # SwiftPods integrates after generation, including cleanup when the last SDK is removed.
        # Automatic regeneration would overwrite them; rerun make-solution after CMake changes.
        SET(MENGINE_SWIFTPODS_INTEGRATED ON CACHE INTERNAL "SwiftPods manages this Xcode solution")
        SET(CMAKE_SUPPRESS_REGENERATION ON CACHE BOOL "Disable automatic CMake regeneration" FORCE)
    else()
        FILE(REMOVE "${CMAKE_BINARY_DIR}/mengine_swiftpods.json")
    endif()
ENDMACRO()

# Single-quoted Ruby strings preserve Xcode's $ variables and prevent interpolation.
function(_MENGINE_SWIFTPODS_RUBY_QUOTE OUTPUT VALUE)
    string(REPLACE "\\" "\\\\" VALUE "${VALUE}")
    string(REPLACE "'" "\\'" VALUE "${VALUE}")
    SET(${OUTPUT} "'${VALUE}'" PARENT_SCOPE)
endfunction()

function(_MENGINE_SWIFTPODS_PREPARE_PACKAGES SOLUTION_DIR)
    if(SOLUTION_DIR STREQUAL "")
        message(FATAL_ERROR "Set MENGINE_SWIFTPODS_SOLUTION_DIR to the Xcode solution directory")
    endif()

    get_filename_component(SOLUTION_DIR "${SOLUTION_DIR}" ABSOLUTE)

    file(READ "${SOLUTION_DIR}/mengine_swiftpods.json" CONFIGURATION)
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

    string(JSON SWIFTPODS GET "${CONFIGURATION}" swiftpods)
    string(JSON PLATFORM GET "${CONFIGURATION}" platform)
    string(JSON DEPLOYMENT_TARGET GET "${CONFIGURATION}" deployment_target)
    _MENGINE_SWIFTPODS_RUBY_QUOTE(PROJECT_RUBY "${PROJECT_NAME}")
    _MENGINE_SWIFTPODS_RUBY_QUOTE(DEPLOYMENT_RUBY "${DEPLOYMENT_TARGET}")
    SET(PODFILE "# Generated by Mengine; rerun make-solution after changing SDK declarations.\nplatform :${PLATFORM}, ${DEPLOYMENT_RUBY}\nproject ${PROJECT_RUBY}\nworkspace ${PROJECT_RUBY}\n")
    SET(SWIFTPODS_TARGETS "${APPLICATION_NAME}")
    SET(PLUGIN_TARGETS "")
    SET(POD_BUILD_SETTINGS "{}")
    string(JSON SOURCE_COUNT LENGTH "${CONFIGURATION}" sources)

    if(SOURCE_COUNT GREATER 0)
        math(EXPR SOURCE_LAST "${SOURCE_COUNT}-1")

        foreach(INDEX RANGE 0 ${SOURCE_LAST})
            string(JSON SOURCE GET "${CONFIGURATION}" sources ${INDEX})
            _MENGINE_SWIFTPODS_RUBY_QUOTE(SOURCE "${SOURCE}")
            string(APPEND PODFILE "source ${SOURCE}\n")
        endforeach()
    endif()

    string(APPEND PODFILE "use_frameworks! :linkage => :static\n")
    string(JSON POD_COUNT LENGTH "${CONFIGURATION}" pods)
    string(JSON PACKAGE_COUNT LENGTH "${CONFIGURATION}" packages)

    if(PACKAGE_COUNT GREATER 0 OR POD_COUNT GREATER 0)
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

    endif()

    foreach(KIND IN ITEMS pods packages)
        string(JSON COUNT LENGTH "${CONFIGURATION}" ${KIND})

        if(COUNT EQUAL 0)
            continue()
        endif()

        math(EXPR PACKAGE_LAST "${COUNT}-1")

        foreach(PACKAGE_INDEX RANGE 0 ${PACKAGE_LAST})
            string(JSON OWNER GET "${CONFIGURATION}" ${KIND} ${PACKAGE_INDEX} 0)
            string(JSON PRODUCT_NAME GET "${CONFIGURATION}" ${KIND} ${PACKAGE_INDEX} 1)
            _MENGINE_SWIFTPODS_RUBY_QUOTE(PRODUCT_RUBY "${PRODUCT_NAME}")

            if(OWNER STREQUAL "@APPLICATION@")
                SET(OWNER "${APPLICATION_NAME}")
            endif()

            if(KIND STREQUAL "pods")
                string(JSON VERSION GET "${CONFIGURATION}" pods ${PACKAGE_INDEX} 2)
                string(JSON OPTIONS GET "${CONFIGURATION}" pods ${PACKAGE_INDEX} 3)
                _MENGINE_SWIFTPODS_RUBY_QUOTE(VERSION_RUBY "${VERSION}")
                SET(DECLARATION "pod ${PRODUCT_RUBY}, ${VERSION_RUBY}")
                string(JSON OPTION_COUNT LENGTH "${OPTIONS}")

                if(OPTION_COUNT GREATER 0)
                    math(EXPR OPTION_LAST "${OPTION_COUNT}-1")

                    foreach(INDEX RANGE 0 ${OPTION_LAST})
                        string(JSON KEY MEMBER "${OPTIONS}" ${INDEX})
                        string(JSON VALUE GET "${OPTIONS}" "${KEY}")

                        if(KEY STREQUAL "build_settings")
                            string(REGEX REPLACE "/.*$" "" POD_ROOT "${PRODUCT_NAME}")
                            string(JSON PREVIOUS ERROR_VARIABLE ERROR GET "${POD_BUILD_SETTINGS}" "${POD_ROOT}")

                            if(NOT ERROR AND NOT PREVIOUS STREQUAL VALUE)
                                message(FATAL_ERROR "Conflicting pod build settings: ${POD_ROOT}")
                            endif()

                            string(JSON POD_BUILD_SETTINGS SET "${POD_BUILD_SETTINGS}" "${POD_ROOT}" "${VALUE}")
                            continue()
                        endif()

                        _MENGINE_SWIFTPODS_RUBY_QUOTE(VALUE "${VALUE}")
                        string(APPEND DECLARATION ", :${KEY} => ${VALUE}")
                    endforeach()
                endif()
            else()
                string(JSON REPOSITORY GET "${CONFIGURATION}" packages ${PACKAGE_INDEX} 2)
                string(JSON VERSION GET "${CONFIGURATION}" packages ${PACKAGE_INDEX} 3)
                _MENGINE_SWIFTPODS_RUBY_QUOTE(REPOSITORY_RUBY "${REPOSITORY}")
                _MENGINE_SWIFTPODS_RUBY_QUOTE(VERSION_RUBY "${VERSION}")
                SET(DECLARATION "spm ${PRODUCT_RUBY}, :git => ${REPOSITORY_RUBY}, :exact => ${VERSION_RUBY}, :products => [${PRODUCT_RUBY}]")
            endif()

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
                    list(APPEND PLUGIN_TARGETS "${NAME}")
                    continue()
                endif()

                list(APPEND SWIFTPODS_TARGETS "${NAME}")
                list(APPEND "PACKAGES_${NAME}" "  ${DECLARATION}\n")
            endforeach()
        endforeach()
    endforeach()

    string(JSON SCRIPT_COUNT LENGTH "${CONFIGURATION}" scripts)

    if(SCRIPT_COUNT GREATER 0)
        math(EXPR SCRIPT_LAST "${SCRIPT_COUNT}-1")

        foreach(SCRIPT_INDEX RANGE 0 ${SCRIPT_LAST})
            string(JSON NAME GET "${CONFIGURATION}" scripts ${SCRIPT_INDEX} 1)
            string(JSON SCRIPT GET "${CONFIGURATION}" scripts ${SCRIPT_INDEX} 2)
            string(JSON INPUT_FILES GET "${CONFIGURATION}" scripts ${SCRIPT_INDEX} 3)
            SET(INPUTS "")

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
                    _MENGINE_SWIFTPODS_RUBY_QUOTE(PATH "${PATH}")
                    list(APPEND INPUTS "${PATH}")
                endforeach()
            endif()

            list(JOIN INPUTS ", " INPUTS)
            _MENGINE_SWIFTPODS_RUBY_QUOTE(NAME_RUBY "${NAME}")
            _MENGINE_SWIFTPODS_RUBY_QUOTE(SCRIPT_RUBY "${SCRIPT}")
            SET(SCRIPTS "${SCRIPTS}  script_phase :name => ${NAME_RUBY}, :script => ${SCRIPT_RUBY}, :input_files => [${INPUTS}], :always_out_of_date => '1'\n")
        endforeach()
    endif()

    list(REMOVE_DUPLICATES SWIFTPODS_TARGETS)
    list(REMOVE_DUPLICATES PLUGIN_TARGETS)

    foreach(NAME IN LISTS SWIFTPODS_TARGETS)
        _MENGINE_SWIFTPODS_RUBY_QUOTE(NAME_RUBY "${NAME}")
        list(REMOVE_DUPLICATES "PACKAGES_${NAME}")
        list(JOIN "PACKAGES_${NAME}" "" PACKAGES)
        SET(PODFILE "${PODFILE}\ntarget ${NAME_RUBY} do\n${PACKAGES}")

        if(NAME STREQUAL APPLICATION_NAME)
            string(APPEND PODFILE "${SCRIPTS}")

            foreach(PLUGIN IN LISTS PLUGIN_TARGETS)
                _MENGINE_SWIFTPODS_RUBY_QUOTE(PLUGIN "${PLUGIN}")
                string(APPEND PODFILE "  target ${PLUGIN} do\n    inherit! :search_paths\n  end\n")
            endforeach()
        endif()

        string(APPEND PODFILE "end\n")
    endforeach()

    string(JSON SETTING_COUNT LENGTH "${POD_BUILD_SETTINGS}")

    if(SETTING_COUNT GREATER 0)
        math(EXPR SETTING_LAST "${SETTING_COUNT}-1")
        string(APPEND PODFILE "\npost_install do |installer|\n  settings = {\n")

        foreach(INDEX RANGE 0 ${SETTING_LAST})
            string(JSON POD_NAME MEMBER "${POD_BUILD_SETTINGS}" ${INDEX})
            _MENGINE_SWIFTPODS_RUBY_QUOTE(POD_RUBY "${POD_NAME}")
            string(APPEND PODFILE "    ${POD_RUBY} => {\n")
            string(JSON COUNT LENGTH "${POD_BUILD_SETTINGS}" "${POD_NAME}")
            math(EXPR LAST "${COUNT}-1")

            foreach(SETTING_INDEX RANGE 0 ${LAST})
                string(JSON KEY MEMBER "${POD_BUILD_SETTINGS}" "${POD_NAME}" ${SETTING_INDEX})
                string(JSON VALUE GET "${POD_BUILD_SETTINGS}" "${POD_NAME}" "${KEY}")
                _MENGINE_SWIFTPODS_RUBY_QUOTE(KEY "${KEY}")
                _MENGINE_SWIFTPODS_RUBY_QUOTE(VALUE "${VALUE}")
                string(APPEND PODFILE "      ${KEY} => ${VALUE},\n")
            endforeach()

            string(APPEND PODFILE "    },\n")
        endforeach()

        string(APPEND PODFILE "  }\n  installer.pod_targets.each do |pod|\n    overrides = settings[pod.pod_name]\n    next unless overrides\n    pod.native_target.build_configurations.each do |configuration|\n      overrides.each { |key, value| configuration.build_settings[key] = value }\n    end\n  end\nend\n")
    endif()

    if(NOT PROJECT STREQUAL ORIGINAL_PROJECT)
        file(WRITE "${PROJECT_FILE}.tmp" "${PROJECT}")
        execute_process(COMMAND plutil -convert xml1 "${PROJECT_FILE}.tmp" COMMAND_ERROR_IS_FATAL ANY)
        file(RENAME "${PROJECT_FILE}.tmp" "${PROJECT_FILE}")
    endif()

    file(WRITE "${SOLUTION_DIR}/Podfile" "${PODFILE}")
    execute_process(COMMAND "${SWIFTPODS}" install
        WORKING_DIRECTORY "${SOLUTION_DIR}" COMMAND_ERROR_IS_FATAL ANY)

    if(PACKAGE_COUNT GREATER 0)
        execute_process(COMMAND xcodebuild -resolvePackageDependencies
            -workspace "${SOLUTION_DIR}/${PROJECT_NAME}.xcworkspace"
            -scheme "${APPLICATION_NAME}"
            -clonedSourcePackagesDirPath "${SOLUTION_DIR}/SourcePackages"
            COMMAND_ERROR_IS_FATAL ANY)
    endif()
endfunction()

if("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_FILE}")
    _MENGINE_SWIFTPODS_PREPARE_PACKAGES("${MENGINE_SWIFTPODS_SOLUTION_DIR}")
endif()
