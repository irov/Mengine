CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

MACRO( ADD_FILTER group_name )
	SOURCE_GROUP( ${group_name} FILES ${ARGN} )
	SET( SRC_FILES ${SRC_FILES} ${ARGN} )
	#MESSAGE( ${ARGN} )
ENDMACRO()
# solution

MACRO( ADD_PLUGIN Plugin Toggle MSG )
    SET(${Plugin} ${Toggle} CACHE BOOL ${MSG} FORCE)
    
    IF(${Plugin})
        add_definitions(-D${Plugin})
    ENDIF()
ENDMACRO()