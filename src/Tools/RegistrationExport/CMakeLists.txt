set( MY_LIB_NAME RegistrationExport )

ADD_FILTER(
src
    RegistrationExport.cpp
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

LINK_DIRECTORIES( ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
ADD_EXECUTABLE( ${MY_LIB_NAME} WIN32 ${SRC_FILES} )

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}    
    )   
