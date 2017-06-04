CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

MACRO( ADD_FILTER group_name )
	SOURCE_GROUP( ${group_name} FILES ${ARGN} )
	SET( SRC_FILES ${SRC_FILES} ${ARGN} )
	#MESSAGE( ${ARGN} )
ENDMACRO()
# solution

MACRO( ADD_PLUGIN Plugin Toggle DLL MSG )
    SET(${Plugin} ${Toggle} CACHE BOOL ${MSG} FORCE)
    SET(${Plugin}_DLL ${DLL} CACHE BOOL ${MSG} FORCE)
    
    IF(${Plugin})
        add_definitions(-D${Plugin})
    ENDIF()
    
    IF(${Plugin}_DLL)
        add_definitions(-D${Plugin}_DLL)
    ENDIF()
ENDMACRO()

if(APPLE)
  macro(ADD_FRAMEWORK appname fwname)
    find_library(FRAMEWORK_${fwname}
        NAMES ${fwname}
        PATHS ${CMAKE_OSX_SYSROOT}/System/Library
        PATH_SUFFIXES Frameworks
        NO_DEFAULT_PATH)
    if( ${FRAMEWORK_${fwname}} STREQUAL FRAMEWORK_${fwname}-NOTFOUND)
        MESSAGE(ERROR ": Framework ${fwname} not found")
    else()
        TARGET_LINK_LIBRARIES(${appname} ${FRAMEWORK_${fwname}})
        MESSAGE(STATUS "Framework ${fwname} found at ${FRAMEWORK_${fwname}}")
    endif()
  endmacro()
endif()