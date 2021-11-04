CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )

set(MENGINE_DOWNLOADS_PATH "${THIRDPARTY_DIR}" CACHE STRING "MENGINE_DOWNLOADS_PATH")

if(MENGINE_DOWNLOADS_SILENT)
    SET(MENGINE_GIT_PROGRESS FALSE)
    SET(MENGINE_DOWNLOAD_NO_PROGRESS 1)
    SET(MENGINE_FILE_DOWNLOAD_SHOW_PROGRESS FALSE)
else()
    SET(MENGINE_GIT_PROGRESS TRUE)
    SET(MENGINE_DOWNLOAD_NO_PROGRESS 0)
    SET(MENGINE_FILE_DOWNLOAD_SHOW_PROGRESS TRUE)
endif()

MESSAGE("MENGINE_DOWNLOADS_PATH: " ${MENGINE_DOWNLOADS_PATH})
MESSAGE("MENGINE_DOWNLOADS_SILENT: " ${MENGINE_DOWNLOADS_SILENT})

include(ExternalProject)

find_package(Git REQUIRED)

MESSAGE("GIT_EXECUTABLE: ${GIT_EXECUTABLE}")
MESSAGE("GIT_VERSION_STRING: ${GIT_VERSION_STRING}")

macro(DOWNLOAD_FILE NAME URL FILE)
    MESSAGE("Download ${NAME}: ${URL}")
    if(NOT EXISTS ${MENGINE_DOWNLOADS_PATH}/${NAME}/${FILE})
        if(MENGINE_FILE_DOWNLOAD_SHOW_PROGRESS)
            file(DOWNLOAD ${URL} ${MENGINE_DOWNLOADS_PATH}/${NAME}/${FILE} SHOW_PROGRESS)
        else()
            file(DOWNLOAD ${URL} ${MENGINE_DOWNLOADS_PATH}/${NAME}/${FILE})
        endif()
    endif()
endmacro()

macro(DOWNLOAD_URL NAME URL)
    MESSAGE("Download ${NAME}: ${URL}")
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

macro(DOWNLOAD_URL_FOLDER NAME URL FOLDER)
    MESSAGE("Download ${NAME}: ${URL}")
    ExternalProject_Add(${NAME}_download PREFIX ${NAME}
        SOURCE_DIR ${THIRDPARTY_DIR}/${FOLDER}
        
        URL ${URL}
        DOWNLOAD_NO_PROGRESS ${MENGINE_DOWNLOAD_NO_PROGRESS}

        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
    )
    
    add_library(${NAME} STATIC IMPORTED)
endmacro()

macro(DOWNLOAD_URL_HASH NAME URL HASH_ALGO HASH)
    MESSAGE("Download ${NAME}: ${URL}")
    ExternalProject_Add(${NAME}_download PREFIX ${NAME}
        SOURCE_DIR ${THIRDPARTY_DIR}/${NAME}
        
        URL ${URL}
        URL_HASH ${HASH_ALGO}=${HASH}
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
        MESSAGE("Git ${NAME}: ${REPOSITORY} [${TAG}]")
        ExternalProject_Add( "${NAME}_${TAG}_download" PREFIX ${NAME}
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
        MESSAGE("Git ${NAME}: ${REPOSITORY}")
        
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
    
    if(EXISTS ${THIRDPARTY_DIR}/${NAME})
        execute_process(COMMAND ${GIT_EXECUTABLE} -C ${THIRDPARTY_DIR}/${NAME} reset --hard --recurse-submodule)
    endif()
endmacro()