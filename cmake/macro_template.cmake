CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

MACRO(MENGINE_ADD_DEFINITION Definition)
    ADD_DEFINITIONS(-D${Definition})
    SET(MENGINE_CONFIG_DEFINITIONS ${MENGINE_CONFIG_DEFINITIONS} ${Definition})
ENDMACRO()

MACRO(SET_MENGINE_ENVIRONMENT MENGINE_TARGET MENGINE_RENDER MENGINE_PLATFORM MENGINE_TOOLCHAIN)
    SET(MENGINE_TARGET_WINDOWS OFF CACHE BOOL "MENGINE_TARGET_WINDOWS" FORCE)
    SET(MENGINE_TARGET_UWP OFF CACHE BOOL "MENGINE_TARGET_UWP" FORCE)
    SET(MENGINE_TARGET_ANDROID OFF CACHE BOOL "MENGINE_TARGET_ANDROID" FORCE)
    SET(MENGINE_TARGET_MACOS OFF CACHE BOOL "MENGINE_TARGET_MACOS" FORCE)
    SET(MENGINE_TARGET_IOS OFF CACHE BOOL "MENGINE_TARGET_IOS" FORCE)
    SET(MENGINE_TARGET_IOS_SIMULATOR OFF CACHE BOOL "MENGINE_TARGET_IOS_SIMULATOR" FORCE)
    SET(MENGINE_TARGET_UNIX OFF CACHE BOOL "MENGINE_TARGET_UNIX" FORCE)

    SET(MENGINE_RENDER_OPENGL OFF CACHE BOOL "MENGINE_RENDER_OPENGL" FORCE)
    SET(MENGINE_RENDER_DIRECTX9 OFF CACHE BOOL "MENGINE_RENDER_DIRECTX9" FORCE)
    SET(MENGINE_RENDER_DIRECTX11 OFF CACHE BOOL "MENGINE_RENDER_DIRECTX11" FORCE)

    SET(MENGINE_PLATFORM_SDL OFF CACHE BOOL "MENGINE_PLATFORM_SDL" FORCE)
    SET(MENGINE_PLATFORM_WIN32 OFF CACHE BOOL "MENGINE_PLATFORM_WIN32" FORCE)

    SET(MENGINE_TOOLCHAIN_MSVC OFF CACHE BOOL "MENGINE_TOOLCHAIN_MSVC" FORCE)
    SET(MENGINE_TOOLCHAIN_MINGW OFF CACHE BOOL "MENGINE_TOOLCHAIN_MINGW" FORCE)
    SET(MENGINE_TOOLCHAIN_GRADLE OFF CACHE BOOL "MENGINE_TOOLCHAIN_GRADLE" FORCE)
    SET(MENGINE_TOOLCHAIN_XCODE OFF CACHE BOOL "MENGINE_TOOLCHAIN_XCODE" FORCE)
    SET(MENGINE_TOOLCHAIN_GCC OFF CACHE BOOL "MENGINE_TOOLCHAIN_GCC" FORCE)
    
    SET(MENGINE_TARGET_${MENGINE_TARGET} ON CACHE BOOL "MENGINE_TARGET" FORCE)
    SET(MENGINE_RENDER_${MENGINE_RENDER} ON CACHE BOOL "MENGINE_RENDER" FORCE)
    SET(MENGINE_PLATFORM_${MENGINE_PLATFORM} ON CACHE BOOL "MENGINE_PLATFORM" FORCE)
    SET(MENGINE_TOOLCHAIN_${MENGINE_TOOLCHAIN} ON CACHE BOOL "MENGINE_TOOLCHAIN" FORCE)
    
    SET(MENGINE_ENVIRONMENT_TARGET ${MENGINE_TARGET} CACHE STRING "MENGINE_ENVIRONMENT_TARGET" FORCE)
    SET(MENGINE_ENVIRONMENT_RENDER ${MENGINE_RENDER} CACHE STRING "MENGINE_ENVIRONMENT_RENDER" FORCE)
    SET(MENGINE_ENVIRONMENT_PLATFORM ${MENGINE_PLATFORM} CACHE STRING "MENGINE_ENVIRONMENT_PLATFORM" FORCE)
    SET(MENGINE_ENVIRONMENT_TOOLCHAIN ${MENGINE_TOOLCHAIN} CACHE STRING "MENGINE_ENVIRONMENT_TOOLCHAIN" FORCE)

    MENGINE_ADD_DEFINITION(MENGINE_ENVIRONMENT_TARGET_${MENGINE_TARGET})
    MENGINE_ADD_DEFINITION(MENGINE_ENVIRONMENT_RENDER_${MENGINE_RENDER})
    MENGINE_ADD_DEFINITION(MENGINE_ENVIRONMENT_PLATFORM_${MENGINE_PLATFORM})
    MENGINE_ADD_DEFINITION(MENGINE_ENVIRONMENT_TOOLCHAIN_${MENGINE_TOOLCHAIN})
    
    SET(MENGINE_TARGET_APPLE OFF CACHE BOOL "MENGINE_TARGET_APPLE" FORCE)
    
    IF(MENGINE_TARGET_MACOS OR MENGINE_TARGET_IOS OR MENGINE_TARGET_IOS_SIMULATOR)
        SET(MENGINE_TARGET_APPLE ON CACHE BOOL "MENGINE_TARGET_APPLE" FORCE)
        
        MENGINE_ADD_DEFINITION(MENGINE_ENVIRONMENT_TARGET_APPLE)
    ENDIF()
    
    MESSAGE("*********************************************")
    
    IF(WIN32)
        IF(${CMAKE_SYSTEM_NAME} STREQUAL "WindowsStore")
            SET(MENGINE_THIRDPARTY_ARCHITECTURE_PREFIX "uwp")
        ELSE()
            IF(MINGW)
                SET(MENGINE_THIRDPARTY_ARCHITECTURE_PREFIX "mingw")
            ELSE()
                SET(MENGINE_THIRDPARTY_ARCHITECTURE_PREFIX "win32")
            ENDIF()
        ENDIF()
    ELSEIF(ANDROID)
        SET(MENGINE_THIRDPARTY_ARCHITECTURE_PREFIX "android")
    ELSEIF(IOS)
        IF(MENGINE_TARGET_IOS)
            SET(MENGINE_THIRDPARTY_ARCHITECTURE_PREFIX "ios")
        ELSEIF(MENGINE_TARGET_IOS_SIMULATOR)
            SET(MENGINE_THIRDPARTY_ARCHITECTURE_PREFIX "iossimulator")
        ELSE()
            MESSAGE(FATAL_ERROR "unsupport architecture prefix")
        ENDIF()
    ELSEIF(APPLE)
        SET(MENGINE_THIRDPARTY_ARCHITECTURE_PREFIX "mac")
    ELSEIF(UNIX)
        SET(MENGINE_THIRDPARTY_ARCHITECTURE_PREFIX "unix")
    ELSE()
        message(FATAL_ERROR "unsupport architecture prefix")
    ENDIF()

    MESSAGE("MENGINE_THIRDPARTY_ARCHITECTURE_PREFIX: ${MENGINE_THIRDPARTY_ARCHITECTURE_PREFIX}")
    
    IF(MENGINE_RENDER_OPENGL)
        SET(MENGINE_THIRDPARTY_RENDER_PREFIX "ogl")
    ELSEIF(MENGINE_RENDER_DIRECTX9)
        SET(MENGINE_THIRDPARTY_RENDER_PREFIX "dx9")
    ELSEIF(MENGINE_RENDER_DIRECTX11)
        SET(MENGINE_THIRDPARTY_RENDER_PREFIX "dx11")
    ELSE()
        MESSAGE(FATAL_ERROR "unsupported render")
    ENDIF()
    
    MESSAGE("MENGINE_THIRDPARTY_RENDER_PREFIX: ${MENGINE_THIRDPARTY_RENDER_PREFIX}")

    IF(NOT MENGINE_TARGET_ANDROID)
        IF(MENGINE_ARCHITECTURE_X86)
            SET(MENGINE_THIRDPARTY_ARCHITECTURE_SUFFIX "x86")
        ELSEIF(MENGINE_ARCHITECTURE_X64)
            SET(MENGINE_THIRDPARTY_ARCHITECTURE_SUFFIX "x64")
        ELSE()
            MESSAGE(FATAL_ERROR "unsupported architecture") 
        ENDIF()
    ENDIF()
    
    MESSAGE("MENGINE_THIRDPARTY_ARCHITECTURE_SUFFIX: ${MENGINE_THIRDPARTY_ARCHITECTURE_SUFFIX}")
    
    SET(MENGINE_THIRDPARTY_TARGET "")

    IF(MENGINE_THIRDPARTY_ARCHITECTURE_PREFIX)
        STRING(CONCAT MENGINE_THIRDPARTY_TARGET ${MENGINE_THIRDPARTY_TARGET} "${MENGINE_THIRDPARTY_ARCHITECTURE_PREFIX}")
    ENDIF()
    
    IF(MENGINE_THIRDPARTY_RENDER_PREFIX)
        STRING(CONCAT MENGINE_THIRDPARTY_TARGET ${MENGINE_THIRDPARTY_TARGET} "_${MENGINE_THIRDPARTY_RENDER_PREFIX}")
    ENDIF()

    IF(MENGINE_THIRDPARTY_ARCHITECTURE_SUFFIX)
        STRING(CONCAT MENGINE_THIRDPARTY_TARGET ${MENGINE_THIRDPARTY_TARGET} "_${MENGINE_THIRDPARTY_ARCHITECTURE_SUFFIX}")
    ENDIF()
    
    MESSAGE("MENGINE_THIRDPARTY_TARGET: ${MENGINE_THIRDPARTY_TARGET}")
    MESSAGE("*********************************************")
    
    SET(MENGINE_THIRDPARTY_PLATFORM "")
    
    IF(CMAKE_GENERATOR_PLATFORM)
        STRING(STRIP "${CMAKE_GENERATOR} ${CMAKE_SYSTEM_NAME} ${CMAKE_GENERATOR_PLATFORM}" MENGINE_THIRDPARTY_PLATFORM)
    ELSE()
        STRING(STRIP "${CMAKE_GENERATOR}" MENGINE_THIRDPARTY_PLATFORM)
    ENDIF()
    
    STRING(REPLACE " " "_" MENGINE_THIRDPARTY_PLATFORM ${MENGINE_THIRDPARTY_PLATFORM})
    
    SET(THIRDPARTY_LIB_OUT "${MENGINE_REPOSITORY}/lib/dependencies/${MENGINE_THIRDPARTY_PLATFORM}/${MENGINE_THIRDPARTY_TARGET}")
    SET(THIRDPARTY_CONFIG_OUT "${MENGINE_REPOSITORY}/dependencies/configuration/${MENGINE_THIRDPARTY_PLATFORM}/${MENGINE_THIRDPARTY_TARGET}")
    
    SET(THIRDPARTY_LIB_DIR ${THIRDPARTY_LIB_OUT}/${THIRDPARTY_LIB_OUT_SUFFIX})
    SET(THIRDPARTY_CONFIG_DIR ${THIRDPARTY_CONFIG_OUT}/${THIRDPARTY_LIB_OUT_SUFFIX})
    
    MESSAGE("THIRDPARTY_LIB_DIR: ${THIRDPARTY_LIB_DIR}")
    MESSAGE("THIRDPARTY_CONFIG_DIR: ${THIRDPARTY_CONFIG_DIR}")
    MESSAGE("*********************************************")
    MESSAGE("MENGINE_TARGET_WINDOWS: ${MENGINE_TARGET_WINDOWS}")
    MESSAGE("MENGINE_TARGET_UWP: ${MENGINE_TARGET_UWP}")
    MESSAGE("MENGINE_TARGET_ANDROID: ${MENGINE_TARGET_ANDROID}")
    MESSAGE("MENGINE_TARGET_APPLE: ${MENGINE_TARGET_APPLE}")
    MESSAGE("MENGINE_TARGET_MACOS: ${MENGINE_TARGET_MACOS}")
    MESSAGE("MENGINE_TARGET_IOS: ${MENGINE_TARGET_IOS}")
    MESSAGE("MENGINE_TARGET_IOS_SIMULATOR: ${MENGINE_TARGET_IOS_SIMULATOR}")
    MESSAGE("MENGINE_TARGET_UNIX: ${MENGINE_TARGET_UNIX}")
    MESSAGE("*********************************************")
    MESSAGE("MENGINE_RENDER_OPENGL: ${MENGINE_RENDER_OPENGL}")
    MESSAGE("MENGINE_RENDER_DIRECTX9: ${MENGINE_RENDER_DIRECTX9}")
    MESSAGE("MENGINE_RENDER_DIRECTX11: ${MENGINE_RENDER_DIRECTX11}")
    MESSAGE("*********************************************")
    MESSAGE("MENGINE_PLATFORM_SDL: ${MENGINE_PLATFORM_SDL}")
    MESSAGE("MENGINE_PLATFORM_WIN32: ${MENGINE_PLATFORM_WIN32}")
    MESSAGE("*********************************************")
    MESSAGE("MENGINE_TOOLCHAIN_MSVC: ${MENGINE_TOOLCHAIN_MSVC}")
    MESSAGE("MENGINE_TOOLCHAIN_MINGW: ${MENGINE_TOOLCHAIN_MINGW}")
    MESSAGE("MENGINE_TOOLCHAIN_GRADLE: ${MENGINE_TOOLCHAIN_GRADLE}")
    MESSAGE("MENGINE_TOOLCHAIN_XCODE: ${MENGINE_TOOLCHAIN_XCODE}")
    MESSAGE("MENGINE_TOOLCHAIN_GCC: ${MENGINE_TOOLCHAIN_GCC}")
    MESSAGE("*********************************************")
    MESSAGE("MENGINE_PROJECT_NAME: ${MENGINE_PROJECT_NAME}")
    MESSAGE("MENGINE_BUILD_MENGINE_MASTER_RELEASE: ${MENGINE_BUILD_MENGINE_MASTER_RELEASE}")
    MESSAGE("MENGINE_BUILD_MENGINE_BUILD_PUBLISH: ${MENGINE_BUILD_MENGINE_BUILD_PUBLISH}")
    MESSAGE("MENGINE_BUILD_MENGINE_LOGGER_LEVEL_FORCE_VERBOSE: ${MENGINE_BUILD_MENGINE_LOGGER_LEVEL_FORCE_VERBOSE}")
    MESSAGE("MENGINE_BUILD_MENGINE_DEVELOPMENT: ${MENGINE_BUILD_MENGINE_DEVELOPMENT}")
    MESSAGE("MENGINE_USE_PRECOMPILED_HEADER: ${MENGINE_USE_PRECOMPILED_HEADER}")
    MESSAGE("MENGINE_USE_SCRIPT_SERVICE: ${MENGINE_USE_SCRIPT_SERVICE}")
    MESSAGE("MENGINE_SECURE_VALUE: ${MENGINE_SECURE_VALUE}")
    MESSAGE("MENGINE_EXTERNAL_PDB: ${MENGINE_EXTERNAL_PDB}")
    MESSAGE("MENGINE_EXTERNAL_PDB_PATH: ${MENGINE_EXTERNAL_PDB_PATH}")
    MESSAGE("MENGINE_EXTERNAL_PROJECT_NAME: ${MENGINE_EXTERNAL_PROJECT_NAME}")
    MESSAGE("MENGINE_EXTERNAL_SOURCE: ${MENGINE_EXTERNAL_SOURCE}")
    MESSAGE("MENGINE_EXTERNAL_SOURCE_PATH: ${MENGINE_EXTERNAL_SOURCE_PATH}")
    MESSAGE("MENGINE_EXTERNAL_TEMP_PATH: ${MENGINE_EXTERNAL_TEMP_PATH}")
    MESSAGE("MENGINE_USE_SUBFOLDER: ${MENGINE_USE_SUBFOLDER}")
    MESSAGE("MENGINE_SUBFOLDER_NAME: ${MENGINE_SUBFOLDER_NAME}")
    MESSAGE("MENGINE_INSTALL_PATH: ${MENGINE_INSTALL_PATH}")
    MESSAGE("*********************************************")
ENDMACRO()

MACRO(SET_MENGINE_OUTPUT_DIRECTORY)
    get_directory_property(DirDefs COMPILE_DEFINITIONS)
    foreach(d ${DirDefs})
        LIST(APPEND MENGINE_DEFINITIONS_SHA1_LIST "${d}: [${${d}}]")
    endforeach()
    
    LIST(APPEND MENGINE_DEFINITIONS_SHA1_LIST "MENGINE_EXTERNAL_PROJECT_NAME: ${MENGINE_EXTERNAL_PROJECT_NAME}")    
    
    STRING(SHA1 MENGINE_DEFINITIONS_SHA1 "${MENGINE_DEFINITIONS_SHA1_LIST}")

    if(UNIX AND NOT ANDROID AND NOT APPLE AND NOT IOS)
        # output paths
        SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${MENGINE_DEFINITIONS_SHA1}/${CMAKE_BUILD_TYPE})
        SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE})
    elseif(ANDROID)
        # output paths
        SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${MENGINE_DEFINITIONS_SHA1}/${ANDROID_ABI}/${CMAKE_BUILD_TYPE})
        SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${ANDROID_ABI}/${CMAKE_BUILD_TYPE})
    elseif(MINGW)
        # output paths
        SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${MENGINE_DEFINITIONS_SHA1}/${CMAKE_BUILD_TYPE})
        SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE})
    elseif(APPLE)
        # output paths
        if(MENGINE_TARGET_MACOS)
            SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${MENGINE_DEFINITIONS_SHA1}/${CMAKE_BUILD_TYPE})
            SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR})
        elseif(MENGINE_TARGET_IOS)
            SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${MENGINE_DEFINITIONS_SHA1})
            SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE})
        elseif(MENGINE_TARGET_IOS_SIMULATOR)
            SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${MENGINE_DEFINITIONS_SHA1})
            SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}/${CMAKE_BUILD_TYPE})
        else()
            MESSAGE(FATAL_ERROR "unsupported target") 
        endif()
    else()
        # output paths
        if(CMAKE_GENERATOR_PLATFORM)
            SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR} ${CMAKE_GENERATOR_PLATFORM}/${MENGINE_DEFINITIONS_SHA1}")
            SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR} ${CMAKE_GENERATOR_PLATFORM}")
        else()
            SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MENGINE_REPOSITORY}/lib/${PROJECT_NAME}/${CMAKE_GENERATOR}/${MENGINE_DEFINITIONS_SHA1}")
            SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MENGINE_INSTALL_PATH}/${PROJECT_NAME}/${CMAKE_GENERATOR}")
        endif()
    endif()

    MESSAGE("CMAKE_ARCHIVE_OUTPUT_DIRECTORY: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
    MESSAGE("CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    
    if(MENGINE_EXTERNAL_PDB)
        MESSAGE("MENGINE_SHARED_PDB_OUTPUT_DIRECTORY: ${MENGINE_SHARED_PDB_OUTPUT_DIRECTORY}")
    endif()
ENDMACRO()

MACRO(MENGINE_SUBFOLDER)
	SET(APPLICATION_LINK_LIBRARIES ${APPLICATION_LINK_LIBRARIES} PARENT_SCOPE)
	SET(APPLICATION_DEPENDENCIES ${APPLICATION_DEPENDENCIES} PARENT_SCOPE)
	SET(APPLICATION_STATIC_PLUGINS ${APPLICATION_STATIC_PLUGINS} PARENT_SCOPE)
    SET(APPLICATION_APPLE_RESOURCES ${APPLICATION_APPLE_RESOURCES} PARENT_SCOPE)
	SET(APPLICATION_APPLE_FRAMEWORKS ${APPLICATION_APPLE_FRAMEWORKS} PARENT_SCOPE)
	SET(APPLICATION_APPLE_FRAMEWORK_WITH_PATH ${APPLICATION_APPLE_FRAMEWORK_WITH_PATH} PARENT_SCOPE)
	SET(APPLICATION_APPLE_COCOAPODS ${APPLICATION_APPLE_COCOAPODS} PARENT_SCOPE)
	SET(APPLICATION_APPLE_APPLICATION_DELEGATE ${APPLICATION_APPLE_APPLICATION_DELEGATE} PARENT_SCOPE)
	SET(APPLICATION_MACOS_DYLIB ${APPLICATION_MACOS_DYLIB} PARENT_SCOPE)
	SET(APPLICATION_MACOS_DYLIB_WITH_SIGN ${APPLICATION_MACOS_DYLIB_WITH_SIGN} PARENT_SCOPE)
    
    SET(MENGINE_CONFIG_DEFINITIONS ${MENGINE_CONFIG_DEFINITIONS} PARENT_SCOPE)
ENDMACRO()

MACRO(MENGINE_PROJECT name)
    PROJECT(${name} C CXX)
    SET(SRC_FILES)
ENDMACRO()

MACRO(MENGINE_PROJECT_EXTRA_LANGUAGE name language)
    PROJECT(${name} C CXX ${language})
    SET(SRC_FILES)
ENDMACRO()

MACRO(ADD_FILTER group_name)
    SOURCE_GROUP(${group_name} FILES ${ARGN})
    
    SET(SRC_FILES ${SRC_FILES} ${ARGN})
ENDMACRO()

MACRO(ADD_FILTER_C group_name)
    SOURCE_GROUP(${group_name} FILES ${ARGN})
    
    SET(SRC_FILES ${SRC_FILES} ${ARGN})
    
    set_source_files_properties(${ARGN} PROPERTIES LANGUAGE C)
ENDMACRO()

# solution
MACRO(ADD_MENGINE_FRAMEWORK)
    IF(MENGINE_EXTERNAL_SOURCE)
        ADD_PLUGIN(MENGINE_PLUGIN_EXTERNALBOOTSTRAPPER ON OFF "MENGINE_PLUGIN_EXTERNALBOOTSTRAPPER")
        
        add_definitions(-DMENGINE_EXTERNAL_SOURCE)
    ENDIF()
ENDMACRO()

MACRO(ENABLE_PLUGIN Plugin)
    SET(${Plugin}_FORCE_ENABLE ${MENGINE_VARIABLES_CACHE_TIMESTEMP} CACHE STRING "${Plugin}_FORCE_ENABLE" FORCE)
ENDMACRO()

MACRO(DISABLE_PLUGIN Plugin)
    SET(${Plugin}_FORCE_DISABLE ${MENGINE_VARIABLES_CACHE_TIMESTEMP} CACHE STRING "${Plugin}_FORCE_DISABLE" FORCE)
ENDMACRO()

MACRO(ENABLE_PLUGIN_DEBUG Plugin)
    SET(${Plugin}_DEBUG_FORCE_ENABLE ${MENGINE_VARIABLES_CACHE_TIMESTEMP} CACHE STRING "${Plugin}_DEBUG_FORCE_ENABLE" FORCE)
ENDMACRO()

MACRO(DISABLE_PLUGIN_DEBUG Plugin)
    SET(${Plugin}_DEBUG_FORCE_DISABLE ${MENGINE_VARIABLES_CACHE_TIMESTEMP} CACHE STRING "${Plugin}_DEBUG_FORCE_DISABLE" FORCE)
ENDMACRO()

MACRO(ENABLE_PLUGIN_OPTION Plugin Option)
    SET(${Plugin}_${Option}_FORCE_ENABLE ${MENGINE_VARIABLES_CACHE_TIMESTEMP} CACHE STRING "${Plugin}_${Option}_FORCE_ENABLE" FORCE)
ENDMACRO()

MACRO(DISABLE_PLUGIN_OPTION Plugin Option)
    SET(${Plugin}_${Option}_FORCE_DISABLE ${MENGINE_VARIABLES_CACHE_TIMESTEMP} CACHE STRING "${Plugin}_${Option}_FORCE_DISABLE" FORCE)
ENDMACRO()

MACRO(ADD_PLUGIN Plugin Toggle DLL MSG)
    IF(${Plugin}_FORCE_ENABLE AND ${Plugin}_FORCE_ENABLE STREQUAL ${MENGINE_VARIABLES_CACHE_TIMESTEMP})
        SET(${Plugin} ON CACHE BOOL ${MSG} FORCE)
    ELSEIF(${Plugin}_FORCE_DISABLE AND ${Plugin}_FORCE_DISABLE STREQUAL ${MENGINE_VARIABLES_CACHE_TIMESTEMP})
        SET(${Plugin} OFF CACHE BOOL ${MSG} FORCE)
    ELSE()
        IF(MENGINE_BUILD_TYPE_DEBUG)
            IF(${Plugin}_DEBUG_FORCE_ENABLE AND ${Plugin}_DEBUG_FORCE_ENABLE STREQUAL ${MENGINE_VARIABLES_CACHE_TIMESTEMP})
                SET(${Plugin} ON CACHE BOOL ${MSG} FORCE)
            ELSEIF(${Plugin}_DEBUG_FORCE_DISABLE AND ${Plugin}_DEBUG_FORCE_DISABLE STREQUAL ${MENGINE_VARIABLES_CACHE_TIMESTEMP})
                SET(${Plugin} OFF CACHE BOOL ${MSG} FORCE)
            ELSE()
                SET(${Plugin} ${Toggle} CACHE BOOL ${MSG} FORCE)
            ENDIF()
        ELSE()
            SET(${Plugin} ${Toggle} CACHE BOOL ${MSG} FORCE)
        ENDIF()
    ENDIF()
    
    IF(${Plugin})
        SET(${Plugin}_DLL ${DLL} CACHE BOOL ${MSG} FORCE)
        
        IF(${Plugin}_DLL)
            SET(${Plugin}_STATIC OFF CACHE BOOL ${MSG} FORCE)
        ELSE()
            SET(${Plugin}_STATIC ON CACHE BOOL ${MSG} FORCE)
        ENDIF()
        
        MENGINE_ADD_DEFINITION(${Plugin})
    ELSE()
        SET(${Plugin}_DLL OFF CACHE BOOL ${MSG} FORCE)
        SET(${Plugin}_STATIC OFF CACHE BOOL ${MSG} FORCE)
    ENDIF()
    
    IF(${Plugin})
        IF(${Plugin}_DLL)
            MENGINE_ADD_DEFINITION(${Plugin}_DLL)
        ENDIF()
        
        IF(${Plugin}_STATIC)
            MENGINE_ADD_DEFINITION(${Plugin}_STATIC)
        ENDIF()
    ENDIF()
    
    MESSAGE("PLUGIN: ${Plugin} = ${${Plugin}} [${${Plugin}_DLL}]")
ENDMACRO()

MACRO(ADD_PLUGIN_IF Plugin Toggle DLL MSG DIRECTORY)
    IF(${Plugin}_FORCE_ENABLE AND ${Plugin}_FORCE_ENABLE STREQUAL ${MENGINE_VARIABLES_CACHE_TIMESTEMP})
        SET(${Plugin} ON CACHE BOOL ${MSG} FORCE)
    ELSEIF(${Plugin}_FORCE_DISABLE AND ${Plugin}_FORCE_DISABLE STREQUAL ${MENGINE_VARIABLES_CACHE_TIMESTEMP})
        SET(${Plugin} OFF CACHE BOOL ${MSG} FORCE)
    ELSE()
        IF(EXISTS ${DIRECTORY})
            IF(MENGINE_BUILD_TYPE_DEBUG)
                IF(${Plugin}_DEBUG_FORCE_ENABLE AND ${Plugin}_DEBUG_FORCE_ENABLE STREQUAL ${MENGINE_VARIABLES_CACHE_TIMESTEMP})
                    SET(${Plugin} ON CACHE BOOL ${MSG} FORCE)
                ELSEIF(${Plugin}_DEBUG_FORCE_DISABLE AND ${Plugin}_DEBUG_FORCE_DISABLE STREQUAL ${MENGINE_VARIABLES_CACHE_TIMESTEMP})
                    SET(${Plugin} OFF CACHE BOOL ${MSG} FORCE)
                ELSE()
                    SET(${Plugin} ${Toggle} CACHE BOOL ${MSG} FORCE)
                ENDIF()
            ELSE()
                SET(${Plugin} ${Toggle} CACHE BOOL ${MSG} FORCE)
            ENDIF()
        ELSE()
            SET(${Plugin} OFF CACHE BOOL ${MSG} FORCE)
        ENDIF()
    ENDIF()
    
    IF(${Plugin})
        SET(${Plugin}_DLL ${DLL} CACHE BOOL ${MSG} FORCE)
        
        IF(${Plugin}_DLL)
            SET(${Plugin}_STATIC OFF CACHE BOOL ${MSG} FORCE)
        ELSE()
            SET(${Plugin}_STATIC ON CACHE BOOL ${MSG} FORCE)
        ENDIF()
        
        MENGINE_ADD_DEFINITION(${Plugin})
    ELSE()
        SET(${Plugin}_DLL OFF CACHE BOOL ${MSG} FORCE)
        SET(${Plugin}_STATIC OFF CACHE BOOL ${MSG} FORCE)
    ENDIF()
    
    IF(${Plugin})
        IF(${Plugin}_DLL)
            MENGINE_ADD_DEFINITION(${Plugin}_DLL)
        ELSE()
            MENGINE_ADD_DEFINITION(${Plugin}_STATIC)
        ENDIF()
    ENDIF()
    
    MESSAGE("PLUGIN: ${Plugin} = ${${Plugin}} [${${Plugin}_DLL}]")
ENDMACRO()

MACRO(ADD_PLUGIN_OPTION Plugin Option Toggle MSG)
    IF(${Plugin})
        IF(${Plugin}_${Option}_FORCE_ENABLE AND ${Plugin}_${Option}_FORCE_ENABLE STREQUAL ${MENGINE_VARIABLES_CACHE_TIMESTEMP})
            SET(${Plugin}_${Option} ON CACHE BOOL ${MSG} FORCE)
        ELSEIF(${Plugin}_${Option}_FORCE_DISABLE AND ${Plugin}_${Option}_FORCE_DISABLE STREQUAL ${MENGINE_VARIABLES_CACHE_TIMESTEMP})
            SET(${Plugin}_${Option} OFF CACHE BOOL ${MSG} FORCE)
        ELSE()
            SET(${Plugin}_${Option} ${Toggle} CACHE BOOL ${MSG} FORCE)
        ENDIF()
        
        IF(${Plugin}_${Option})
            MENGINE_ADD_DEFINITION(${Plugin}_${Option})
        ENDIF()
        
        MESSAGE("PLUGIN OPTION: ${Plugin}_${Option} = ${${Plugin}_${Option}}")
    ENDIF()
ENDMACRO()

MACRO(CREATE_PRECOMPILED_HEADER)
  IF(MSVC AND MENGINE_USE_PRECOMPILED_HEADER)
	SET(PrecompiledHeader "${MENGINE_SOURCE_DIR}/PrecompiledHeader/PrecompiledHeader.h")
	SET(PrecompiledSource "${MENGINE_SOURCE_DIR}/PrecompiledHeader/PrecompiledHeader.cpp")
    SET(PrecompiledBinary "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/PrecompiledHeader.pch")

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
  ENDIF()
ENDMACRO()

MACRO(ADD_PRECOMPILED_HEADER)
  IF(MSVC AND MENGINE_USE_PRECOMPILED_HEADER)
	SET(PrecompiledHeader "${MENGINE_SOURCE_DIR}/PrecompiledHeader/PrecompiledHeader.h")
    SET(PrecompiledBinary "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/PrecompiledHeader.pch")

    SET_SOURCE_FILES_PROPERTIES(${SRC_FILES}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           )
  ENDIF()
ENDMACRO()

MACRO(ADD_MENGINE_INTERFACE)
    ADD_LIBRARY(${PROJECT_NAME} INTERFACE)
    
    add_custom_target(${PROJECT_NAME}.headers SOURCES ${SRC_FILES})
    
    set(extra_macro_args ${ARGN})
    list(LENGTH extra_macro_args num_extra_args)
    
    set(USE_SUBFOLDER ON)
    if(${num_extra_args} EQUAL 1)
        list(GET extra_macro_args 0 FILTER_FOLDER)
    elseif(${num_extra_args} EQUAL 2)
        list(GET extra_macro_args 0 FILTER_FOLDER)
        list(GET extra_macro_args 1 USE_SUBFOLDER)
    endif()

    if(MENGINE_USE_SUBFOLDER AND USE_SUBFOLDER)
        if(${num_extra_args} GREATER 0)
            set_target_properties (${PROJECT_NAME}.headers PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}/${FILTER_FOLDER}
            )
        else()
            set_target_properties (${PROJECT_NAME}.headers PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}
            )
        endif()
    else()
        if(${num_extra_args} GREATER 0)
            set_target_properties (${PROJECT_NAME}.headers PROPERTIES
                FOLDER ${FILTER_FOLDER}
            )
        endif()
    endif()
ENDMACRO()

MACRO(ADD_MENGINE_LIBRARY)
    ADD_PRECOMPILED_HEADER()
    
    ADD_DEFINITIONS(-DMENGINE_LIBRARY_NAME="${PROJECT_NAME}")
    
    ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRC_FILES})
    
    if(MSVC)
        target_sources(${PROJECT_NAME} INTERFACE ${MENGINE_REPOSITORY}/contributing/codestyle/mengine.natvis)
    endif()
	
	if(XCODE)
		SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_SKIP_INSTALL YES)
	endif()
    
    IF(MSVC AND MENGINE_USE_PRECOMPILED_HEADER)
        ADD_DEPENDENCIES(${PROJECT_NAME} PrecompiledHeader)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} PrecompiledHeader)
    ENDIF()
    
    set(extra_macro_args ${ARGN})
    list(LENGTH extra_macro_args num_extra_args)
    
    set(USE_SUBFOLDER ON)
    if(${num_extra_args} EQUAL 1)
        list(GET extra_macro_args 0 FILTER_FOLDER)
    elseif(${num_extra_args} EQUAL 2)
        list(GET extra_macro_args 0 FILTER_FOLDER)
        list(GET extra_macro_args 1 USE_SUBFOLDER)
    endif()

    if(MENGINE_USE_SUBFOLDER AND USE_SUBFOLDER)
        if(${num_extra_args} GREATER 0)
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}/${FILTER_FOLDER}
            )
        else()
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}
            )
        endif()
    else()
        if(${num_extra_args} GREATER 0)
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER ${FILTER_FOLDER}
            )
        endif()
    endif()
    
    SET(APPLICATION_LINK_LIBRARIES ${APPLICATION_LINK_LIBRARIES} ${PROJECT_NAME} PARENT_SCOPE)
ENDMACRO()

MACRO(ADD_MENGINE_SHARED)
    ADD_PRECOMPILED_HEADER()
    
    ADD_DEFINITIONS(-DMENGINE_LIBRARY_NAME="${PROJECT_NAME}")
    
    ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_FILES})
    
    if(MSVC)
        target_sources(${PROJECT_NAME} INTERFACE ${MENGINE_REPOSITORY}/contributing/codestyle/mengine.natvis)
    endif()
	
	if(XCODE)
		SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_SKIP_INSTALL YES)
	endif()
    
    if(MENGINE_EXTERNAL_PDB)
        if(MSVC)
            SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES 
                COMPILE_PDB_NAME ${PROJECT_NAME} 
                PDB_OUTPUT_DIRECTORY ${MENGINE_EXTERNAL_PDB_PATH}
            )
        elseif(MINGW)
            target_compile_options(${PROJECT_NAME} PRIVATE -gcodeview)
            target_link_options(${PROJECT_NAME} PRIVATE -Wl,-pdb=${MENGINE_EXTERNAL_PDB_PATH}/${PROJECT_NAME}.pdb)
        endif()
    else()
        if(MENGINE_BUILD_TYPE_DEBUG)
            if(MINGW)
                target_compile_options(${PROJECT_NAME} PRIVATE -gcodeview)
                target_link_options(${PROJECT_NAME} PRIVATE -Wl,-pdb=)
            endif()
        endif()
    endif()
    
    SET_TARGET_PROPERTIES( ${PROJECT_NAME} 
        PROPERTIES PREFIX ""
        )
    
    IF(MSVC AND MENGINE_USE_PRECOMPILED_HEADER)
        ADD_DEPENDENCIES(${PROJECT_NAME} PrecompiledHeader)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} PrecompiledHeader)
    ENDIF()
    
    set(extra_macro_args ${ARGN})
    list(LENGTH extra_macro_args num_extra_args)
    
    set(USE_SUBFOLDER ON)
    if(${num_extra_args} EQUAL 1)
        list(GET extra_macro_args 0 FILTER_FOLDER)
    elseif(${num_extra_args} EQUAL 2)
        list(GET extra_macro_args 0 FILTER_FOLDER)
        list(GET extra_macro_args 1 USE_SUBFOLDER)
    endif()
    
    if(MENGINE_USE_SUBFOLDER AND USE_SUBFOLDER)
        if(${num_extra_args} GREATER 0)
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}/${FILTER_FOLDER}
            )
        else()
            set_target_properties (${PROJECT_NAME} PROPERTIES
                    FOLDER ${MENGINE_SUBFOLDER_NAME}
                )
        endif()
    else()
        if(${num_extra_args} GREATER 0)
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER ${FILTER_FOLDER}
            )
        endif()
    endif()
    
    SET(APPLICATION_DEPENDENCIES ${APPLICATION_DEPENDENCIES} ${PROJECT_NAME} PARENT_SCOPE)
ENDMACRO()

MACRO(ADD_MENGINE_EXECUTABLE_WIN32)
    ADD_DEFINITIONS(-DMENGINE_LIBRARY_NAME="${PROJECT_NAME}")

    ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${SRC_FILES})
    
    if(MSVC)
        target_sources(${PROJECT_NAME} INTERFACE ${MENGINE_REPOSITORY}/contributing/codestyle/mengine.natvis)
    endif()
    
    if(MENGINE_EXTERNAL_PDB)
        if(MSVC)
            set_target_properties(${PROJECT_NAME} PROPERTIES
                COMPILE_PDB_NAME ${PROJECT_NAME}
                PDB_OUTPUT_DIRECTORY ${MENGINE_EXTERNAL_PDB_PATH}
            )
        elseif(MINGW)
            target_compile_options(${PROJECT_NAME} PRIVATE -gcodeview)
            target_link_options(${PROJECT_NAME} PRIVATE -Wl,-pdb=${MENGINE_EXTERNAL_PDB_PATH}/${PROJECT_NAME}.pdb)
        endif()
    else()
        if(MENGINE_BUILD_TYPE_DEBUG)
            if(MINGW)
                target_compile_options(${PROJECT_NAME} PRIVATE -gcodeview)
                target_link_options(${PROJECT_NAME} PRIVATE -Wl,-pdb=)
            endif()
        endif()
    endif()
    
    IF(MSVC AND MENGINE_USE_PRECOMPILED_HEADER)
        ADD_DEPENDENCIES(${PROJECT_NAME} PrecompiledHeader)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} PrecompiledHeader)
    ENDIF()
ENDMACRO()

MACRO(ADD_MENGINE_EXECUTABLE_UNIX)
    ADD_DEFINITIONS(-DMENGINE_LIBRARY_NAME="${PROJECT_NAME}")

    ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_FILES})
ENDMACRO()

MACRO(ADD_MENGINE_EXECUTABLE_MACOSX)
    ADD_DEFINITIONS(-DMENGINE_LIBRARY_NAME="${PROJECT_NAME}")
    
    ADD_EXECUTABLE(${PROJECT_NAME} MACOSX_BUNDLE ${SRC_FILES})
ENDMACRO()

MACRO(ADD_MENGINE_EXECUTABLE_ANDROID)
    ADD_DEFINITIONS(-DMENGINE_LIBRARY_NAME="${PROJECT_NAME}")

    ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_FILES})
ENDMACRO()


MACRO(ADD_MENGINE_PLUGIN PLUGIN_NAME)
    IF(${PLUGIN_NAME}_DLL)
        add_definitions(-DMENGINE_PLUGIN_DLL)
        ADD_MENGINE_SHARED()
    ELSE()
        SET(APPLICATION_STATIC_PLUGINS ${APPLICATION_STATIC_PLUGINS} ${PROJECT_NAME} PARENT_SCOPE)
        ADD_MENGINE_LIBRARY()
    ENDIF()
    
    set(extra_macro_args ${ARGN})
    list(LENGTH extra_macro_args num_extra_args)
    
    set(USE_SUBFOLDER ON)
    if(${num_extra_args} EQUAL 1)
        list(GET extra_macro_args 0 FILTER_FOLDER)
    elseif(${num_extra_args} EQUAL 2)
        list(GET extra_macro_args 0 FILTER_FOLDER)
        list(GET extra_macro_args 1 USE_SUBFOLDER)
    endif()
    
    if(MENGINE_USE_SUBFOLDER AND ${USE_SUBFOLDER})
        if(${num_extra_args} GREATER 0)
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}/${FILTER_FOLDER}
            )
        else()
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}/Plugins
            )
        endif()
    else()
        if(${num_extra_args} GREATER 0)
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER ${FILTER_FOLDER}
            )
        else()
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER Plugins
            )
        endif()
    endif()
ENDMACRO()

MACRO(ADD_MENGINE_PLUGIN_MODULE)
    ADD_PRECOMPILED_HEADER()
    
    ADD_DEFINITIONS(-DMENGINE_LIBRARY_NAME="${PROJECT_NAME}")
    
    ADD_LIBRARY(${PROJECT_NAME} MODULE ${SRC_FILES})
    
    if(MSVC)
        target_sources(${PROJECT_NAME} INTERFACE ${MENGINE_REPOSITORY}/contributing/codestyle/mengine.natvis)
    endif()
	
	if(XCODE)
		SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_SKIP_INSTALL YES)
	endif()
    
    IF(MSVC AND MENGINE_USE_PRECOMPILED_HEADER)
        ADD_DEPENDENCIES(${PROJECT_NAME} PrecompiledHeader)
        TARGET_LINK_LIBRARIES(${PROJECT_NAME} PrecompiledHeader)
    ENDIF()
    
    set(extra_macro_args ${ARGN})
    list(LENGTH extra_macro_args num_extra_args)
    
    set(USE_SUBFOLDER ON)
    if(${num_extra_args} EQUAL 1)
        list(GET extra_macro_args 0 FILTER_FOLDER)
    elseif(${num_extra_args} EQUAL 2)
        list(GET extra_macro_args 0 FILTER_FOLDER)
        list(GET extra_macro_args 1 USE_SUBFOLDER)
    endif()
    
    if(MENGINE_USE_SUBFOLDER AND ${USE_SUBFOLDER})
        if(${num_extra_args} GREATER 0)
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}/${FILTER_FOLDER}
            )
        else()
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER ${MENGINE_SUBFOLDER_NAME}/Plugins
            )
        endif()
    else()
        if(${num_extra_args} GREATER 0)
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER ${FILTER_FOLDER}
            )
        else()
            set_target_properties (${PROJECT_NAME} PROPERTIES
                FOLDER Plugins
            )
        endif()
    endif()
ENDMACRO()

MACRO(ADD_MENGINE_COCOAPOD COCOAPOD_NAME COCOAPOD_GIT COCOAPOD_TAG)
    LIST(APPEND APPLICATION_APPLE_COCOAPODS ${PROJECT_NAME} ${COCOAPOD_NAME} ${COCOAPOD_GIT} ${COCOAPOD_TAG})
    SET(APPLICATION_APPLE_COCOAPODS ${APPLICATION_APPLE_COCOAPODS} PARENT_SCOPE)
ENDMACRO()

macro(set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property(TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY} ${XCODE_VALUE})
endmacro()

macro(ADD_MENGINE_MACOS_DYLIB PATH)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PATH})

    LIST(APPEND APPLICATION_MACOS_DYLIB ${PATH})

    SET(APPLICATION_MACOS_DYLIB ${APPLICATION_MACOS_DYLIB} PARENT_SCOPE)
endmacro()

macro(ADD_MENGINE_MACOS_DYLIB_WITH_SIGN PATH)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PATH})

    LIST(APPEND APPLICATION_MACOS_DYLIB_WITH_SIGN ${PATH})

    SET(APPLICATION_MACOS_DYLIB_WITH_SIGN ${APPLICATION_MACOS_DYLIB_WITH_SIGN} PARENT_SCOPE)
endmacro()

macro(ADD_MENGINE_APPLE_FRAMEWORK NAME)
    find_library(FRAMEWORK_${NAME} ${NAME})

    if( ${FRAMEWORK_${NAME}} STREQUAL FRAMEWORK_${NAME}-NOTFOUND)
        MESSAGE(FATAL_ERROR "Framework ${NAME} not found")
    else()
        target_link_libraries(${PROJECT_NAME} "-framework ${NAME}")
        
        MESSAGE(STATUS "Project ${PROJECT_NAME} Framework ${NAME} found at ${FRAMEWORK_${NAME}}")
        
        LIST(APPEND APPLICATION_APPLE_FRAMEWORKS ${NAME})
        SET(APPLICATION_APPLE_FRAMEWORKS ${APPLICATION_APPLE_FRAMEWORKS} PARENT_SCOPE)
    endif()
endmacro()

macro(ADD_MENGINE_APPLE_FRAMEWORK_WITH_PATH NAME PATH)
    find_library(FRAMEWORK_${NAME} NAMES ${NAME} PATHS ${PATH} NO_CMAKE_FIND_ROOT_PATH)

    if(${FRAMEWORK_${NAME}} STREQUAL FRAMEWORK_${NAME}-NOTFOUND)
        MESSAGE(FATAL_ERROR "Framework ${NAME} not found")
    else()
        target_link_libraries(${PROJECT_NAME} ${FRAMEWORK_${NAME}})

        MESSAGE(STATUS "Project ${PROJECT_NAME} Framework ${NAME} found at ${FRAMEWORK_${NAME}}")
        
        LIST(APPEND APPLICATION_APPLE_FRAMEWORK_WITH_PATH ${FRAMEWORK_${NAME}})
        SET(APPLICATION_APPLE_FRAMEWORK_WITH_PATH ${APPLICATION_APPLE_FRAMEWORK_WITH_PATH} PARENT_SCOPE)
    endif()
endmacro()

macro(ADD_MENGINE_APPLE_APPLICATION_DELEGATE DELEGATE)
    LIST(APPEND APPLICATION_APPLE_APPLICATION_DELEGATE ${DELEGATE})
    
    SET(APPLICATION_APPLE_APPLICATION_DELEGATE ${APPLICATION_APPLE_APPLICATION_DELEGATE} PARENT_SCOPE)
endmacro()

macro(ADD_MENGINE_APPLE_RESOURCES RESOURCES)
    LIST(APPEND APPLICATION_APPLE_RESOURCES ${RESOURCES})
    
    SET(APPLICATION_APPLE_RESOURCES ${APPLICATION_APPLE_RESOURCES} PARENT_SCOPE)
endmacro()

macro(ADD_LIBRARY_IMPORTED NAME PATH)
    add_library(${NAME}
            SHARED
            IMPORTED)
             
    set_target_properties(
            # Specifies the target library.
            ${NAME}
            
            # Specifies the parameter you want to define.
            PROPERTIES IMPORTED_LOCATION

            # Provides the path to the library you want to import.
            ${PATH}
            )
            
    get_property(${NAME}_location TARGET ${NAME} PROPERTY LOCATION)
            
    MESSAGE("link imported library ${NAME}: ${${NAME}_location}")

    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${NAME})
endmacro()