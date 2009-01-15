MACRO( MENGE_SET_PROJECT_DEFAULTS )

	set(CMAKE_DEBUG_POSTFIX "d")

	if(MSVC)
		if(NOT MENGE_USE_MD)
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
			set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
			set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
		endif(NOT MENGE_USE_MD)

		add_definitions(-D_CRT_SECURE_NO_DEPRECATE)

	endif(MSVC)
	
ENDMACRO( MENGE_SET_PROJECT_DEFAULTS )
