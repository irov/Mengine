MACRO(ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES)
    ADD_MENGINE_COCOAPOD("GoogleUtilities" "NO-GIT" "8.1.0")
ENDMACRO()

MACRO(MENGINE_GENERATE_COCOAPODS)
    SET(PODFILE_BUFFER "")
    
    STRING(APPEND PODFILE_BUFFER "source 'https://github.com/CocoaPods/Specs.git'\n")
    
    STRING(APPEND PODFILE_BUFFER "project '${MENGINE_PROJECT_NAME}.xcodeproj'\n")
    
    if(MENGINE_TARGET_MACOS)
        STRING(APPEND PODFILE_BUFFER "platform :macos, '${CMAKE_OSX_DEPLOYMENT_TARGET}'\n")
    endif()
    
    if(MENGINE_TARGET_IOS OR MENGINE_TARGET_IOS_SIMULATOR)
        STRING(APPEND PODFILE_BUFFER "platform :ios, '${CMAKE_OSX_DEPLOYMENT_TARGET}'\n")
    endif()

    STRING(APPEND PODFILE_BUFFER "use_frameworks!\n")
    STRING(APPEND PODFILE_BUFFER "inhibit_all_warnings!\n")
    STRING(APPEND PODFILE_BUFFER "\n")

    SET(LENGTH_APPLICATION_APPLE_COCOAPODS)
    list(LENGTH APPLICATION_APPLE_COCOAPODS LENGTH_APPLICATION_APPLE_COCOAPODS)
    
    math(EXPR LENGTH_APPLICATION_APPLE_COCOAPODS "${LENGTH_APPLICATION_APPLE_COCOAPODS}-1")
    
    SET(LENGTH_APPLICATION_APPLE_GLOBAL_COCOAPODS)
    list(LENGTH APPLICATION_APPLE_GLOBAL_COCOAPODS LENGTH_APPLICATION_APPLE_GLOBAL_COCOAPODS)
    
    math(EXPR LENGTH_APPLICATION_APPLE_GLOBAL_COCOAPODS "${LENGTH_APPLICATION_APPLE_GLOBAL_COCOAPODS}-1")
    
    SET(LENGTH_APPLICATION_APPLE_SCRIPT_PHASES)
    list(LENGTH APPLICATION_APPLE_SCRIPT_PHASES LENGTH_APPLICATION_APPLE_SCRIPT_PHASES)
    
    math(EXPR LENGTH_APPLICATION_APPLE_SCRIPT_PHASES "${LENGTH_APPLICATION_APPLE_SCRIPT_PHASES}-1")
    
    if(NOT ${LENGTH_APPLICATION_APPLE_COCOAPODS} EQUAL -1)
        SET(APPLICATION_APPLE_COCOAPODS_PROJECTS)

        foreach(INDEX RANGE 0 ${LENGTH_APPLICATION_APPLE_COCOAPODS} 5)
            SET(COCOAPOD_PROJECT_NAME)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_PROJECT_NAME)
            math(EXPR INDEX "${INDEX}+1")
        
            SET(COCOAPOD_NAME)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_NAME)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(COCOAPOD_GIT)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_GIT)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(COCOAPOD_TAG)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_TAG)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(COCOAPOD_PATCH)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_PATCH)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(APPLICATION_APPLE_COCOAPODS_PROJECT_${COCOAPOD_PROJECT_NAME})
            LIST(APPEND APPLICATION_APPLE_COCOAPODS_PROJECTS "APPLICATION_APPLE_COCOAPODS_PROJECT_${COCOAPOD_PROJECT_NAME}")
        endforeach()
        
        list(REMOVE_DUPLICATES APPLICATION_APPLE_COCOAPODS_PROJECTS)
        
        foreach(INDEX RANGE 0 ${LENGTH_APPLICATION_APPLE_COCOAPODS} 5)
            SET(COCOAPOD_PROJECT_NAME)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_PROJECT_NAME)
            math(EXPR INDEX "${INDEX}+1")
        
            SET(COCOAPOD_NAME)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_NAME)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(COCOAPOD_GIT)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_GIT)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(COCOAPOD_TAG)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_TAG)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(COCOAPOD_PATCH)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_PATCH)
            math(EXPR INDEX "${INDEX}+1")
            
            LIST(APPEND APPLICATION_APPLE_COCOAPODS_PROJECT_${COCOAPOD_PROJECT_NAME} ${COCOAPOD_PROJECT_NAME} ${COCOAPOD_NAME} ${COCOAPOD_GIT} ${COCOAPOD_TAG} ${COCOAPOD_PATCH})
        endforeach()
        
        foreach(APPLICATION_APPLE_COCOAPODS_PROJECT ${APPLICATION_APPLE_COCOAPODS_PROJECTS})
            SET(LENGTH_APPLICATION_APPLE_COCOAPODS_PROJECT)
            list(LENGTH ${APPLICATION_APPLE_COCOAPODS_PROJECT} LENGTH_APPLICATION_APPLE_COCOAPODS_PROJECT)
        
            math(EXPR LENGTH_APPLICATION_APPLE_COCOAPODS_PROJECT "${LENGTH_APPLICATION_APPLE_COCOAPODS_PROJECT}-1")
            
            SET(COCOAPOD_PROJECT_NAME_0)
            list(GET ${APPLICATION_APPLE_COCOAPODS_PROJECT} 0 COCOAPOD_PROJECT_NAME_0)
            
            STRING(APPEND PODFILE_BUFFER "target '" ${COCOAPOD_PROJECT_NAME_0} "' do\n")
            STRING(APPEND PODFILE_BUFFER "  use_frameworks!\n")
            
            foreach(INDEX RANGE 0 ${LENGTH_APPLICATION_APPLE_COCOAPODS_PROJECT} 5)
                SET(COCOAPOD_PROJECT_NAME)
                list(GET ${APPLICATION_APPLE_COCOAPODS_PROJECT} ${INDEX} COCOAPOD_PROJECT_NAME)
                math(EXPR INDEX "${INDEX}+1")
            
                SET(COCOAPOD_NAME)
                list(GET ${APPLICATION_APPLE_COCOAPODS_PROJECT} ${INDEX} COCOAPOD_NAME)
                math(EXPR INDEX "${INDEX}+1")
                
                SET(COCOAPOD_GIT)
                list(GET ${APPLICATION_APPLE_COCOAPODS_PROJECT} ${INDEX} COCOAPOD_GIT)
                math(EXPR INDEX "${INDEX}+1")
                
                SET(COCOAPOD_TAG)
                list(GET ${APPLICATION_APPLE_COCOAPODS_PROJECT} ${INDEX} COCOAPOD_TAG)
                math(EXPR INDEX "${INDEX}+1")
                
                SET(COCOAPOD_PATCH)
                list(GET ${APPLICATION_APPLE_COCOAPODS_PROJECT} ${INDEX} COCOAPOD_PATCH)
                math(EXPR INDEX "${INDEX}+1")

                string(COMPARE EQUAL ${COCOAPOD_GIT} "NO-GIT" COCOAPOD_NO_GIT)
                string(COMPARE EQUAL ${COCOAPOD_TAG} "NO-TAG" COCOAPOD_NO_TAG)
                string(COMPARE EQUAL ${COCOAPOD_PATCH} "NO-PATCH" COCOAPOD_NO_PATCH)
                
                STRING(APPEND PODFILE_BUFFER "  pod '" ${COCOAPOD_NAME} "'")
                
                if(NOT COCOAPOD_NO_GIT)
                    STRING(APPEND PODFILE_BUFFER ", :git => '" ${COCOAPOD_GIT} "'")
                    
                    if(NOT COCOAPOD_NO_TAG)
                        STRING(APPEND PODFILE_BUFFER ", :tag => '" ${COCOAPOD_TAG} "'")
                    endif()
                else()
                    if(NOT COCOAPOD_NO_TAG)
                        STRING(APPEND PODFILE_BUFFER ", '" ${COCOAPOD_TAG} "'")
                    endif()
                endif()
                
                if(NOT COCOAPOD_NO_PATCH)
                    STRING(APPEND PODFILE_BUFFER ", " ${COCOAPOD_PATCH})
                endif()
                
                STRING(APPEND PODFILE_BUFFER "\n")
            endforeach()
            
            STRING(APPEND PODFILE_BUFFER "end\n")
            STRING(APPEND PODFILE_BUFFER "\n")
        endforeach()
    endif()

    STRING(APPEND PODFILE_BUFFER "target '" ${PROJECT_NAME} "' do\n")
    STRING(APPEND PODFILE_BUFFER "  use_frameworks!\n")

    if(NOT ${LENGTH_APPLICATION_APPLE_GLOBAL_COCOAPODS} EQUAL -1)
        foreach(INDEX RANGE 0 ${LENGTH_APPLICATION_APPLE_GLOBAL_COCOAPODS} 5)
            SET(COCOAPOD_PROJECT_NAME)
            list(GET APPLICATION_APPLE_GLOBAL_COCOAPODS ${INDEX} COCOAPOD_PROJECT_NAME)
            math(EXPR INDEX "${INDEX}+1")
        
            SET(COCOAPOD_NAME)
            list(GET APPLICATION_APPLE_GLOBAL_COCOAPODS ${INDEX} COCOAPOD_NAME)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(COCOAPOD_GIT)
            list(GET APPLICATION_APPLE_GLOBAL_COCOAPODS ${INDEX} COCOAPOD_GIT)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(COCOAPOD_TAG)
            list(GET APPLICATION_APPLE_GLOBAL_COCOAPODS ${INDEX} COCOAPOD_TAG)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(COCOAPOD_PATCH)
            list(GET APPLICATION_APPLE_GLOBAL_COCOAPODS ${INDEX} COCOAPOD_PATCH)
            math(EXPR INDEX "${INDEX}+1")

            string(COMPARE EQUAL ${COCOAPOD_GIT} "NO-GIT" COCOAPOD_NO_GIT)
            string(COMPARE EQUAL ${COCOAPOD_TAG} "NO-TAG" COCOAPOD_NO_TAG)
            string(COMPARE EQUAL ${COCOAPOD_PATCH} "NO-PATCH" COCOAPOD_NO_PATCH)
        
            STRING(APPEND PODFILE_BUFFER "  pod '" ${COCOAPOD_NAME} "'")
            
            if(NOT COCOAPOD_NO_GIT)
                STRING(APPEND PODFILE_BUFFER ", :git => '" ${COCOAPOD_GIT} "'")
                
                if(NOT COCOAPOD_NO_TAG)
                    STRING(APPEND PODFILE_BUFFER ", :tag => '" ${COCOAPOD_TAG} "'")
                endif()
            else()
                if(NOT COCOAPOD_NO_TAG)
                    STRING(APPEND PODFILE_BUFFER ", '" ${COCOAPOD_TAG} "'")
                endif()
            endif()
            
            if(NOT COCOAPOD_NO_PATCH)
                STRING(APPEND PODFILE_BUFFER ", " ${COCOAPOD_PATCH})
            endif()
            
            STRING(APPEND PODFILE_BUFFER "\n")
        endforeach()
    endif()
    
    if(NOT ${LENGTH_APPLICATION_APPLE_COCOAPODS} EQUAL -1)
        foreach(INDEX RANGE 0 ${LENGTH_APPLICATION_APPLE_COCOAPODS} 5)
            SET(COCOAPOD_PROJECT_NAME)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_PROJECT_NAME)
            math(EXPR INDEX "${INDEX}+1")
        
            SET(COCOAPOD_NAME)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_NAME)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(COCOAPOD_GIT)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_GIT)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(COCOAPOD_TAG)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_TAG)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(COCOAPOD_PATCH)
            list(GET APPLICATION_APPLE_COCOAPODS ${INDEX} COCOAPOD_PATCH)
            math(EXPR INDEX "${INDEX}+1")

            string(COMPARE EQUAL ${COCOAPOD_GIT} "NO-GIT" COCOAPOD_NO_GIT)
            string(COMPARE EQUAL ${COCOAPOD_TAG} "NO-TAG" COCOAPOD_NO_TAG)
            string(COMPARE EQUAL ${COCOAPOD_PATCH} "NO-PATCH" COCOAPOD_NO_PATCH)
        
            STRING(APPEND PODFILE_BUFFER "  pod '" ${COCOAPOD_NAME} "'")
            
            if(NOT COCOAPOD_NO_GIT)
                STRING(APPEND PODFILE_BUFFER ", :git => '" ${COCOAPOD_GIT} "'")
                
                if(NOT COCOAPOD_NO_TAG)
                    STRING(APPEND PODFILE_BUFFER ", :tag => '" ${COCOAPOD_TAG} "'")
                endif()
            else()
                if(NOT COCOAPOD_NO_TAG)
                    STRING(APPEND PODFILE_BUFFER ", '" ${COCOAPOD_TAG} "'")
                endif()
            endif()
            
            if(NOT COCOAPOD_NO_PATCH)
                STRING(APPEND PODFILE_BUFFER ", " ${COCOAPOD_PATCH})
            endif()
            
            STRING(APPEND PODFILE_BUFFER "\n")
        endforeach()
    else()
        STRING(APPEND PODFILE_BUFFER "  pod 'Null'\n")
    endif()
    
    if(NOT ${LENGTH_APPLICATION_APPLE_SCRIPT_PHASES} EQUAL -1)
        foreach(INDEX RANGE 0 ${LENGTH_APPLICATION_APPLE_SCRIPT_PHASES} 4)
            SET(SCRIPT_PHASES_PROJECT_NAME)
            list(GET APPLICATION_APPLE_SCRIPT_PHASES ${INDEX} SCRIPT_PHASES_PROJECT_NAME)
            math(EXPR INDEX "${INDEX}+1")
        
            SET(SCRIPT_PHASES_NAME)
            list(GET APPLICATION_APPLE_SCRIPT_PHASES ${INDEX} SCRIPT_PHASES_NAME)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(SCRIPT_PHASES_SCRIPT)
            list(GET APPLICATION_APPLE_SCRIPT_PHASES ${INDEX} SCRIPT_PHASES_SCRIPT)
            math(EXPR INDEX "${INDEX}+1")
            
            SET(SCRIPT_PHASES_INPUT_FILES)
            list(GET APPLICATION_APPLE_SCRIPT_PHASES ${INDEX} SCRIPT_PHASES_INPUT_FILES)
            math(EXPR INDEX "${INDEX}+1")
            
            string(COMPARE EQUAL ${SCRIPT_PHASES_INPUT_FILES} "NO-INPUT-FILES" SCRIPT_PHASES_NO_INPUT_FILES)
            
            string(REPLACE "($)" "$" SCRIPT_PHASES_SCRIPT_II ${SCRIPT_PHASES_SCRIPT})
            string(REPLACE "($)" "$" SCRIPT_PHASES_INPUT_FILES_II ${SCRIPT_PHASES_INPUT_FILES})

            STRING(APPEND PODFILE_BUFFER "  script_phase :name => '" ${SCRIPT_PHASES_NAME} "', :script => '" ${SCRIPT_PHASES_SCRIPT_II} "'")
            
            if(NOT SCRIPT_PHASES_NO_INPUT_FILES)
                STRING(APPEND PODFILE_BUFFER ", :input_files => " ${SCRIPT_PHASES_INPUT_FILES_II} "")
            endif()
            
            STRING(APPEND PODFILE_BUFFER "\n")
        endforeach()
    endif()
    
    STRING(APPEND PODFILE_BUFFER "end\n")
    STRING(APPEND PODFILE_BUFFER "\n")
    
    STRING(APPEND PODFILE_BUFFER "#patch\n")
    STRING(APPEND PODFILE_BUFFER "\n")

    SET(PODS_END_PATCH)
    
    if(APPLE AND IOS)
        #configure_file("iOSPods.patch.in" "${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/iOSPods.patch" @ONLY)
        
        #file(STRINGS "${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/iOSPods.patch" PODS_END_PATCH NEWLINE_CONSUME)
    endif()
    
    if(APPLE AND NOT IOS)
        configure_file("MacOSPods.patch.in" "${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/MacOSPods.patch" @ONLY)
        
        file(STRINGS "${MENGINE_SOLUTIONS_CONFIG_DIR}/include/Configuration/MacOSPods.patch" PODS_END_PATCH NEWLINE_CONSUME)
    endif()
            
    STRING(APPEND PODFILE_BUFFER ${PODS_END_PATCH})
    
    FILE(WRITE ${CMAKE_BINARY_DIR}/Podfile ${PODFILE_BUFFER})

    FILE(REMOVE ${CMAKE_BINARY_DIR}/Podfile.lock)
ENDMACRO()