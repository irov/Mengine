CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

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

macro(DOWNLOAD_GDRIVE_FOLDER NAME FOLDER_ID DEST)
    # Downloads every file from a public Google Drive folder into DEST.
    # The public 'embeddedfolderview' page is parsed to discover each file id and
    # name, then every file is fetched through the direct 'uc?export=download'
    # endpoint. Files already present in DEST are skipped.
    MESSAGE("Download GDrive folder ${NAME}: ${FOLDER_ID}")

    set(MENGINE_GDRIVE_LISTING "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_gdrive_listing.html")

    file(DOWNLOAD
        "https://drive.google.com/embeddedfolderview?id=${FOLDER_ID}#list"
        "${MENGINE_GDRIVE_LISTING}"
        STATUS MENGINE_GDRIVE_STATUS
    )

    list(GET MENGINE_GDRIVE_STATUS 0 MENGINE_GDRIVE_CODE)

    if(NOT MENGINE_GDRIVE_CODE EQUAL 0)
        MESSAGE(WARNING "GDrive folder ${NAME}: listing download failed (${MENGINE_GDRIVE_STATUS})")
    else()
        file(READ "${MENGINE_GDRIVE_LISTING}" MENGINE_GDRIVE_HTML)

        # Each entry exposes its file id as id="entry-<FILE_ID>" and its display
        # name inside a <div class="flip-entry-title">NAME</div> element.
        string(REGEX MATCHALL "id=\"entry-[-_A-Za-z0-9]+\"" MENGINE_GDRIVE_IDS "${MENGINE_GDRIVE_HTML}")
        string(REGEX MATCHALL "flip-entry-title\">[^<]+<" MENGINE_GDRIVE_TITLES "${MENGINE_GDRIVE_HTML}")

        list(LENGTH MENGINE_GDRIVE_IDS MENGINE_GDRIVE_COUNT)
        list(LENGTH MENGINE_GDRIVE_TITLES MENGINE_GDRIVE_TITLE_COUNT)

        if(NOT MENGINE_GDRIVE_COUNT EQUAL MENGINE_GDRIVE_TITLE_COUNT)
            MESSAGE(WARNING "GDrive folder ${NAME}: id/title count mismatch (${MENGINE_GDRIVE_COUNT} vs ${MENGINE_GDRIVE_TITLE_COUNT})")
        elseif(MENGINE_GDRIVE_COUNT EQUAL 0)
            MESSAGE(WARNING "GDrive folder ${NAME}: no files found in listing")
        else()
            math(EXPR MENGINE_GDRIVE_LAST "${MENGINE_GDRIVE_COUNT}-1")

            foreach(MENGINE_GDRIVE_I RANGE ${MENGINE_GDRIVE_LAST})
                list(GET MENGINE_GDRIVE_IDS ${MENGINE_GDRIVE_I} MENGINE_GDRIVE_ID_RAW)
                list(GET MENGINE_GDRIVE_TITLES ${MENGINE_GDRIVE_I} MENGINE_GDRIVE_TITLE_RAW)

                string(REGEX REPLACE "^id=\"entry-(.+)\"$" "\\1" MENGINE_GDRIVE_FILE_ID "${MENGINE_GDRIVE_ID_RAW}")
                string(REGEX REPLACE "^flip-entry-title\">(.+)<$" "\\1" MENGINE_GDRIVE_FILE_NAME "${MENGINE_GDRIVE_TITLE_RAW}")

                if(NOT EXISTS "${DEST}/${MENGINE_GDRIVE_FILE_NAME}")
                    MESSAGE("  GDrive ${NAME}: ${MENGINE_GDRIVE_FILE_NAME} (${MENGINE_GDRIVE_FILE_ID})")

                    if(MENGINE_FILE_DOWNLOAD_SHOW_PROGRESS)
                        file(DOWNLOAD
                            "https://drive.google.com/uc?export=download&id=${MENGINE_GDRIVE_FILE_ID}"
                            "${DEST}/${MENGINE_GDRIVE_FILE_NAME}"
                            SHOW_PROGRESS
                        )
                    else()
                        file(DOWNLOAD
                            "https://drive.google.com/uc?export=download&id=${MENGINE_GDRIVE_FILE_ID}"
                            "${DEST}/${MENGINE_GDRIVE_FILE_NAME}"
                        )
                    endif()
                endif()
            endforeach()
        endif()
    endif()
endmacro()

macro(DOWNLOAD_URL NAME URL)
	if(NOT EXISTS ${THIRDPARTY_DIR}/${NAME})
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
	endif()
endmacro()

macro(DOWNLOAD_URL_FOLDER NAME URL FOLDER)
	if(NOT EXISTS ${THIRDPARTY_DIR}/${FOLDER})
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
	endif()
endmacro()

macro(DOWNLOAD_URL_HASH NAME URL HASH_ALGO HASH)
    if(NOT EXISTS ${THIRDPARTY_DIR}/${NAME})
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
	endif()
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
            UPDATE_COMMAND ${GIT_EXECUTABLE} fetch --tags --force --prune
                COMMAND ${GIT_EXECUTABLE} checkout -f ${TAG}
                COMMAND ${GIT_EXECUTABLE} reset --hard ${TAG}
                COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
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
        execute_process( COMMAND git -C ${THIRDPARTY_DIR}/${NAME} config --get remote.origin.url OUTPUT_VARIABLE THIRDPARTY_REMOTE_URL)
        
        if(REPOSITORY STREQUAL THIRDPARTY_REMOTE_URL)
            execute_process(COMMAND ${GIT_EXECUTABLE} -C ${THIRDPARTY_DIR}/${NAME} reset --hard --recurse-submodule)
        endif()
    endif()
endmacro()