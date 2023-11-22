MACRO(MENGINE_GENERATE_COCOAPODS)
    SET(PODFILE_BUFFER "")
    
    STRING(APPEND PODFILE_BUFFER "project '${MENGINE_PROJECT_NAME}.xcodeproj'\n")
    
    if(MENGINE_TARGET_MACOS)
        STRING(APPEND PODFILE_BUFFER "platform :macos, '${CMAKE_OSX_DEPLOYMENT_TARGET}'\n")
    endif()
    
    if(MENGINE_TARGET_IOS OR MENGINE_TARGET_IOS_SIMULATOR)
        STRING(APPEND PODFILE_BUFFER "platform :ios, '${CMAKE_OSX_DEPLOYMENT_TARGET}'\n")
    endif()
    
    STRING(APPEND PODFILE_BUFFER "inhibit_all_warnings!\n")
    STRING(APPEND PODFILE_BUFFER "\n")

    SET(LENGTH_APPLICATION_APPLE_COCOAPODS)
    list(LENGTH APPLICATION_APPLE_COCOAPODS LENGTH_APPLICATION_APPLE_COCOAPODS)
    
    math(EXPR LENGTH_APPLICATION_APPLE_COCOAPODS "${LENGTH_APPLICATION_APPLE_COCOAPODS}-1")
    
    if(NOT ${LENGTH_APPLICATION_APPLE_COCOAPODS} EQUAL -1)
        SET(APPLICATION_APPLE_COCOAPODS_PROJECTS)

        foreach(COCOAPODS_INDEX RANGE 0 ${LENGTH_APPLICATION_APPLE_COCOAPODS} 5)
            SET(COCOAPOD_PROJECT_NAME)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_PROJECT_NAME)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
        
            SET(COCOAPOD_NAME)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_NAME)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
            
            SET(COCOAPOD_GIT)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_GIT)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
            
            SET(COCOAPOD_TAG)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_TAG)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
            
            SET(COCOAPOD_PATCH)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_PATCH)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
            
            SET(APPLICATION_APPLE_COCOAPODS_PROJECT_${COCOAPOD_PROJECT_NAME})
            LIST(APPEND APPLICATION_APPLE_COCOAPODS_PROJECTS "APPLICATION_APPLE_COCOAPODS_PROJECT_${COCOAPOD_PROJECT_NAME}")
        endforeach()
        
        list(REMOVE_DUPLICATES APPLICATION_APPLE_COCOAPODS_PROJECTS)
        
        foreach(COCOAPODS_INDEX RANGE 0 ${LENGTH_APPLICATION_APPLE_COCOAPODS} 5)
            SET(COCOAPOD_PROJECT_NAME)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_PROJECT_NAME)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
        
            SET(COCOAPOD_NAME)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_NAME)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
            
            SET(COCOAPOD_GIT)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_GIT)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
            
            SET(COCOAPOD_TAG)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_TAG)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
            
            SET(COCOAPOD_PATCH)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_PATCH)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
            
            LIST(APPEND APPLICATION_APPLE_COCOAPODS_PROJECT_${COCOAPOD_PROJECT_NAME} ${COCOAPOD_PROJECT_NAME} ${COCOAPOD_NAME} ${COCOAPOD_GIT} ${COCOAPOD_TAG} ${COCOAPOD_PATCH})
        endforeach()
        
        foreach(APPLICATION_APPLE_COCOAPODS_PROJECT ${APPLICATION_APPLE_COCOAPODS_PROJECTS})
            SET(LENGTH_APPLICATION_APPLE_COCOAPODS_PROJECT)
            list(LENGTH ${APPLICATION_APPLE_COCOAPODS_PROJECT} LENGTH_APPLICATION_APPLE_COCOAPODS_PROJECT)
        
            math(EXPR LENGTH_APPLICATION_APPLE_COCOAPODS_PROJECT "${LENGTH_APPLICATION_APPLE_COCOAPODS_PROJECT}-1")
            
            SET(COCOAPOD_PROJECT_NAME_0)
            list(GET ${APPLICATION_APPLE_COCOAPODS_PROJECT} 0 COCOAPOD_PROJECT_NAME_0)
            
            STRING(APPEND PODFILE_BUFFER "target '" ${COCOAPOD_PROJECT_NAME_0} "' do\n")
            
            foreach(COCOAPODS_INDEX RANGE 0 ${LENGTH_APPLICATION_APPLE_COCOAPODS_PROJECT} 5)
                SET(COCOAPOD_PROJECT_NAME)
                list(GET ${APPLICATION_APPLE_COCOAPODS_PROJECT} ${COCOAPODS_INDEX} COCOAPOD_PROJECT_NAME)
                math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
            
                SET(COCOAPOD_NAME)
                list(GET ${APPLICATION_APPLE_COCOAPODS_PROJECT} ${COCOAPODS_INDEX} COCOAPOD_NAME)
                math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
                
                SET(COCOAPOD_GIT)
                list(GET ${APPLICATION_APPLE_COCOAPODS_PROJECT} ${COCOAPODS_INDEX} COCOAPOD_GIT)
                math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
                
                SET(COCOAPOD_TAG)
                list(GET ${APPLICATION_APPLE_COCOAPODS_PROJECT} ${COCOAPODS_INDEX} COCOAPOD_TAG)
                math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
                
                SET(COCOAPOD_PATCH)
                list(GET ${APPLICATION_APPLE_COCOAPODS_PROJECT} ${COCOAPODS_INDEX} COCOAPOD_PATCH)
                math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")

                string(COMPARE EQUAL ${COCOAPOD_GIT} "NO-GIT" COCOAPODS_NO_GIT)
                string(COMPARE EQUAL ${COCOAPOD_TAG} "NO-TAG" COCOAPODS_NO_TAG)
                string(COMPARE EQUAL ${COCOAPOD_PATCH} "NO-PATCH" COCOAPODS_NO_PATCH)
                
                STRING(APPEND PODFILE_BUFFER "  pod '" ${COCOAPOD_NAME} "'")
                
                if(NOT COCOAPODS_NO_GIT)
                    STRING(APPEND PODFILE_BUFFER ", :git => '" ${COCOAPOD_GIT} "'")
                endif()
                
                if(NOT COCOAPODS_NO_TAG)
                    STRING(APPEND PODFILE_BUFFER ", :tag => '" ${COCOAPOD_TAG} "'")
                endif()
                
                if(NOT COCOAPODS_NO_PATCH)
                    STRING(APPEND PODFILE_BUFFER ", " ${COCOAPODS_PATCH})
                endif()
                
                STRING(APPEND PODFILE_BUFFER "\n")
            endforeach()
            
            STRING(APPEND PODFILE_BUFFER "  use_frameworks!\n")
            STRING(APPEND PODFILE_BUFFER "end\n")
            STRING(APPEND PODFILE_BUFFER "\n")
        endforeach()
    endif()

    STRING(APPEND PODFILE_BUFFER "target '" ${PROJECT_NAME} "' do\n")

    if(NOT ${LENGTH_APPLICATION_APPLE_COCOAPODS} EQUAL -1)
        foreach(COCOAPODS_INDEX RANGE 0 ${LENGTH_APPLICATION_APPLE_COCOAPODS} 5)
            SET(COCOAPOD_PROJECT_NAME)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_PROJECT_NAME)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
        
            SET(COCOAPOD_NAME)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_NAME)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
            
            SET(COCOAPOD_GIT)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_GIT)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
            
            SET(COCOAPOD_TAG)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_TAG)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")
            
            SET(COCOAPOD_PATCH)
            list(GET APPLICATION_APPLE_COCOAPODS ${COCOAPODS_INDEX} COCOAPOD_PATCH)
            math(EXPR COCOAPODS_INDEX "${COCOAPODS_INDEX}+1")

            string(COMPARE EQUAL ${COCOAPOD_GIT} "NO-GIT" COCOAPODS_NO_GIT)
            string(COMPARE EQUAL ${COCOAPOD_TAG} "NO-TAG" COCOAPODS_NO_TAG)
            string(COMPARE EQUAL ${COCOAPOD_PATCH} "NO-PATCH" COCOAPODS_NO_PATCH)
        
            STRING(APPEND PODFILE_BUFFER "  pod '" ${COCOAPOD_NAME} "'")
            
            if(NOT COCOAPODS_NO_GIT)
                STRING(APPEND PODFILE_BUFFER ", :git => '" ${COCOAPOD_GIT} "'")
            endif()
            
            if(NOT COCOAPODS_NO_TAG)
                STRING(APPEND PODFILE_BUFFER ", :tag => '" ${COCOAPOD_TAG} "'")
            endif()
            
            if(NOT COCOAPODS_NO_PATCH)
                STRING(APPEND PODFILE_BUFFER ", " ${COCOAPODS_PATCH})
            endif()
            
            STRING(APPEND PODFILE_BUFFER "\n")
        endforeach()
    else()
        STRING(APPEND PODFILE_BUFFER "  pod 'Null'\n")
    endif()
    
    STRING(APPEND PODFILE_BUFFER "  use_frameworks!\n")
    STRING(APPEND PODFILE_BUFFER "end\n")
    STRING(APPEND PODFILE_BUFFER "\n")
    
    STRING(APPEND PODFILE_BUFFER "#patch\n")
    STRING(APPEND PODFILE_BUFFER "\n")

    SET(PODS_END_PATCH)
    
    if(APPLE AND IOS)
        file(STRINGS "iOSPods.patch" PODS_END_PATCH NEWLINE_CONSUME)
    endif()
    
    if(APPLE AND NOT IOS)
        file(STRINGS "MacOSPods.patch" PODS_END_PATCH NEWLINE_CONSUME)
    endif()
            
    STRING(APPEND PODFILE_BUFFER ${PODS_END_PATCH})
    
    FILE(WRITE ${CMAKE_BINARY_DIR}/Podfile ${PODFILE_BUFFER})

    FILE(REMOVE ${CMAKE_BINARY_DIR}/Podfile.lock)
ENDMACRO()