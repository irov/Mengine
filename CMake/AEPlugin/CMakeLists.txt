 
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

MACRO( ADD_FILTER group_name )
	SOURCE_GROUP( ${group_name} FILES ${ARGN} )
	SET( SRC_FILES ${SRC_FILES} ${ARGN} )
	#MESSAGE( ${ARGN} )
ENDMACRO( ADD_FILTER group_name files )


# solution

PROJECT( AEPlugin )

SET(MENGINE_TARGET "WIN64")

SET(MENGINE_TARGET_AEPLUGIN ON)

INCLUDE( ../win64_template.cmake )

SET( MENGINE_REPOSITORY ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

SET( MENGINE_LIB_PREFIX "" )
SET( MENGINE_LIB_SUFFIX ".lib" )


# output paths
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/${MENGINE_LIB_DIR} )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MENGINE_REPOSITORY}/bin_aeplugin )

# input and thirdparty paths
SET( THIRDPARTY_DIR ${MENGINE_REPOSITORY}/dependencies )
SET( SOURCE_DIR ${MENGINE_REPOSITORY}/src )
SET( CMAKE_TEMP_DIR ${MENGINE_REPOSITORY}/CMakeTemp/AEPlugin/${CMAKE_BUILD_TYPE} )

# implementation

SET( MENGE_DEPENDENCIES )
SET( APPLICATION_DEPENDENCIES )
SET( APPLICATION_LIBS )

INCLUDE_DIRECTORIES( ${SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${SOURCE_DIR}/Utils )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/boost )
INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/stdex/include )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/Utils ${CMAKE_TEMP_DIR}/Utils )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Kernel ${CMAKE_TEMP_DIR}/Kernel )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Metacode ${CMAKE_TEMP_DIR}/Metacode )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/Menge ${CMAKE_TEMP_DIR}/Menge )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/WindowsLayer ${CMAKE_TEMP_DIR}/WindowsLayer )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/System/Win32UnicodeSystem ${CMAKE_TEMP_DIR}/System/Win32UnicodeSystem )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/System/Win32ThreadSystem ${CMAKE_TEMP_DIR}/System/Win32ThreadSystem )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/System/DX9RenderSystem ${CMAKE_TEMP_DIR}/System/DX9RenderSystem )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/System/MockupOptionsSystem ${CMAKE_TEMP_DIR}/System/MockupOptionsSystem )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceProvider ${CMAKE_TEMP_DIR}/Service/ServiceProvider )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceOptions ${CMAKE_TEMP_DIR}/Service/ServiceOptions )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceStringize ${CMAKE_TEMP_DIR}/Service/ServiceStringize )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceArchive ${CMAKE_TEMP_DIR}/Service/ServiceArchive )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceFile ${CMAKE_TEMP_DIR}/Service/ServiceFile )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceCodec ${CMAKE_TEMP_DIR}/Service/ServiceCodec )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceData ${CMAKE_TEMP_DIR}/Service/ServiceData )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceMemory ${CMAKE_TEMP_DIR}/Service/ServiceMemory )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceConverter ${CMAKE_TEMP_DIR}/Service/ServiceConverter )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceLogger ${CMAKE_TEMP_DIR}/Service/ServiceLogger )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServicePlugin ${CMAKE_TEMP_DIR}/Service/ServicePlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceRender ${CMAKE_TEMP_DIR}/Service/ServiceRender )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceUnicode ${CMAKE_TEMP_DIR}/Service/ServiceUnicode )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceThread ${CMAKE_TEMP_DIR}/Service/ServiceThread )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceConfig ${CMAKE_TEMP_DIR}/Service/ServiceConfig )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Service/ServiceNotification ${CMAKE_TEMP_DIR}/Service/ServiceNotification )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeDevelopmentConverterPlugin ${CMAKE_TEMP_DIR}/Plugin/MengeDevelopmentConverterPlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeImageCodecPlugin ${CMAKE_TEMP_DIR}/Plugin/MengeImageCodecPlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeZipPlugin ${CMAKE_TEMP_DIR}/Plugin/MengeZipPlugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeLZ4Plugin ${CMAKE_TEMP_DIR}/Plugin/MengeLZ4Plugin )
ADD_SUBDIRECTORY( ${SOURCE_DIR}/Plugin/MengeWin32FileGroupPlugin ${CMAKE_TEMP_DIR}/Plugin/MengeWin32FileGroupPlugin )

ADD_SUBDIRECTORY( ${SOURCE_DIR}/AEPlugin ${CMAKE_TEMP_DIR}/AEPlugin )