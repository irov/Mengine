CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(MENGINE_DEPENDENCIES_DEBUG_SUFFIX "d")
ELSE()
    SET(MENGINE_DEPENDENCIES_DEBUG_SUFFIX "")
ENDIF()

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
    
    IF(${Plugin})
        IF(${Plugin}_DLL)
            add_definitions(-D${Plugin}_DLL)
        ELSE()
            add_definitions(-D${Plugin}_STATIC)
        ENDIF()
    ENDIF()
ENDMACRO()

MACRO(CREATE_PRECOMPILED_HEADER)
  IF(MSVC)
	SET(PrecompiledHeader "PrecompiledHeader.h")
	SET(PrecompiledSource "${MENGINE_SOURCE_DIR}/PrecompiledHeader/PrecompiledHeader.cpp")
    SET(PrecompiledBinary "${MENGINE_SOURCE_DIR}/PrecompiledHeader/PrecompiledHeader.pch")

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
  ENDIF()
ENDMACRO()

MACRO(ADD_PRECOMPILED_HEADER)
  IF(MSVC)
	SET(PrecompiledHeader "PrecompiledHeader.h")
    SET(PrecompiledBinary "${MENGINE_SOURCE_DIR}/PrecompiledHeader/PrecompiledHeader.pch")

    SET_SOURCE_FILES_PROPERTIES(${SRC_FILES}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           )  
										   #OBJECT_DEPENDS "${PrecompiledBinary}"
  ENDIF()
ENDMACRO()

MACRO(ADD_MENGINE_LIBRARY)
	#ADD_PRECOMPILED_HEADER()

	ADD_LIBRARY( ${MY_LIB_NAME} STATIC ${SRC_FILES})
	
	#ADD_DEPENDENCIES( ${MY_LIB_NAME} PrecompiledHeader )
ENDMACRO()

MACRO(ADD_MENGINE_SHARED)
	#ADD_PRECOMPILED_HEADER()

	ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES})
	
	SET_TARGET_PROPERTIES( ${MY_LIB_NAME} 
		PROPERTIES PREFIX ""
		)
	
	#ADD_DEPENDENCIES( ${MY_LIB_NAME} PrecompiledHeader )
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