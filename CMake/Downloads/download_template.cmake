CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )

include(ExternalProject)

macro(DOWNLOAD_URL NAME URL)
    ExternalProject_Add(${NAME}_download PREFIX ${NAME}
        SOURCE_DIR ${THIRDPARTY_DIR}/${NAME}
        
        URL ${URL}

        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
    )
    
    add_library(${NAME} STATIC IMPORTED)
endmacro()

macro(GIT_CLONE NAME REPOSITORY)
    set(TAG ${ARGN})
    
    list(LENGTH TAG EXIST_TAG)
    
    if(${EXIST_TAG} GREATER 0)
        ExternalProject_Add(${NAME}_download PREFIX ${NAME}
            SOURCE_DIR ${THIRDPARTY_DIR}/${NAME}
            
            GIT_REPOSITORY ${REPOSITORY}
            GIT_TAG ${TAG}
            GIT_SHALLOW 1
            GIT_PROGRESS TRUE

            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            UPDATE_COMMAND ""
            INSTALL_COMMAND ""
        )
    else()
        ExternalProject_Add(${NAME}_download PREFIX ${NAME}
            SOURCE_DIR ${THIRDPARTY_DIR}/${NAME}
            
            GIT_REPOSITORY ${REPOSITORY}
            GIT_SHALLOW 1
            GIT_PROGRESS TRUE

            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            UPDATE_COMMAND ${GIT_EXECUTABLE} pull
            INSTALL_COMMAND ""
            )
    endif()
    
    add_library(${NAME} STATIC IMPORTED)
endmacro()