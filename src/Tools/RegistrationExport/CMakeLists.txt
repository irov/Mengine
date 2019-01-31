set( MY_LIB_NAME RegistrationExport )

ADD_FILTER(
src
    RegistrationExport.cpp
)

LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}    
    )   
