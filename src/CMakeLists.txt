
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

MACRO( ADD_FILTER group_name )
	SOURCE_GROUP( ${group_name} FILES ${ARGN} )
	SET( SRC_FILES ${SRC_FILES} ${ARGN} )
	#MESSAGE( ${ARGN} )
ENDMACRO( ADD_FILTER group_name files )

# solution

PROJECT( Mengine )

SET( MENGINE_REPOSITORY ${Mengine_SOURCE_DIR}/.. )
SET( MENGINE_DEBUG_POSTFIX "_d")

# output paths
SET( LIBRARY_OUTPUT_PATH ${MENGINE_REPOSITORY}/lib )

SET( EXECUTABLE_OUTPUT_PATH ${MENGINE_REPOSITORY}/bin )
SET( EXECUTABLE_OUTPUT_PATH_Release ${MENGINE_REPOSITORY}/bin )
SET( EXECUTABLE_OUTPUT_PATH_Debug ${MENGINE_REPOSITORY}/bin_d )

# input and thirdparty paths
SET( THIRDPARTY_DIR ${MENGINE_REPOSITORY}/dependencies )

# implementation
SET( CMAKE_DEBUG_POSTFIX "${MENGINE_DEBUG_POSTFIX}" )

if (NOT MSVC)
	ADD_DEFINITIONS(-D'override = /* */')
endif(NOT MSVC)


ADD_SUBDIRECTORY( libs )

ADD_SUBDIRECTORY( Menge )
ADD_DEPENDENCIES( Menge
	Math
	XmlParser
)

ADD_SUBDIRECTORY( System )

IF( WIN32 )
	ADD_SUBDIRECTORY( WinApplication )

	SET( APP_DEPENDENCIES
	Menge
	Box2DPhysicSystem
	HGERenderSystem
	MengeLogSystem
	MengeWin32FileSystem
	OISInputSystem
	OALSoundSystem
	)

	ADD_DEPENDENCIES( WinApplication ${APP_DEPENDENCIES} )

ELSEIF( APPLE )
	ADD_SUBDIRECTORY( MacOSApplication )

ENDIF( WIN32 )
