CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )

if(MENGINE_DOWNLOADS_SILENT)
    SET(MENGINE_GIT_PROGRESS FALSE)
    SET(MENGINE_DOWNLOAD_NO_PROGRESS 1)
    SET(MENGINE_FILE_DOWNLOAD_SHOW_PROGRESS FALSE)
else()
    SET(MENGINE_GIT_PROGRESS TRUE)
    SET(MENGINE_DOWNLOAD_NO_PROGRESS 0)
    SET(MENGINE_FILE_DOWNLOAD_SHOW_PROGRESS TRUE)
endif()

MESSAGE("MENGINE_DOWNLOADS_SILENT: " ${MENGINE_DOWNLOADS_SILENT})

include(ExternalProject)

macro(DOWNLOAD_FILE NAME URL FILE)
    if(NOT EXISTS ${THIRDPARTY_DIR}/${NAME}/${FILE})
        if(MENGINE_FILE_DOWNLOAD_SHOW_PROGRESS)
            file(DOWNLOAD ${URL} ${THIRDPARTY_DIR}/${NAME}/${FILE} SHOW_PROGRESS)
        else()
            file(DOWNLOAD ${URL} ${THIRDPARTY_DIR}/${NAME}/${FILE})
        endif()
    endif()
endmacro()

macro(DOWNLOAD_URL NAME URL)
    ExternalProject_Add(${NAME}_download PREFIX ${NAME}
        SOURCE_DIR ${THIRDPARTY_DIR}/${NAME}
        
        URL ${URL}
        
        DOWNLOAD_NO_PROGRESS ${MENGINE_DOWNLOAD_NO_PROGRESS}

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
            GIT_PROGRESS ${MENGINE_GIT_PROGRESS}

            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            UPDATE_COMMAND ""
            INSTALL_COMMAND ""
        )
    else()
        ExternalProject_Add(${NAME}_download PREFIX ${NAME}
            SOURCE_DIR ${THIRDPARTY_DIR}/${NAME}
            
            GIT_REPOSITORY ${REPOSITORY}
            GIT_PROGRESS ${MENGINE_GIT_PROGRESS}

            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            UPDATE_COMMAND ${GIT_EXECUTABLE} pull
            INSTALL_COMMAND ""
            )
    endif()
    
    add_library(${NAME} STATIC IMPORTED)
endmacro()