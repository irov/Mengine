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