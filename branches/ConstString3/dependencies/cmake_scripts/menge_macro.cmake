MACRO( MENGE_SET_PROJECT_DEFAULTS project_name )

	# SET LIBNAME
	set(CMAKE_DEBUG_POSTFIX "d")
	set( ${project_name}_LIB_NAME ${project_name}_static )

	#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)
	if(MSVC)
		
		set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /Zm1000 /MP2 /EHsc /GR")
		set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /Zi /Ob0 /Od /RTC1")
		set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /D NDEBUG")
		set(CMAKE_C_FLAGS "/DWIN32 /D_WINDOWS /W3 /Zm1000")
		set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /Zi /Ob0 /Od /RTC1")
		set(CMAKE_C_FLAGS_RELEASE "/O2 /Ob2 /D NDEBUG")
		
		#set(CMAKE_EXE_LINKER_FLAGS "/STACK:10000000 /machine:I386")
		#set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:YES")
		#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO")

		if(MENGE_USE_MT)
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
			set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
			set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
			set( ${project_name}_LIB_NAME ${${project_name}_LIB_NAME}_mt )
			
		else(MENGE_USE_MT)
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
			set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
			set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")

		endif(MENGE_USE_MT)


		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_SECURE_SCL=0")
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -D_SECURE_SCL=0")
		#add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS )
        add_definitions(-D_HAS_ITERATOR_DEBUGGING )

	endif(MSVC)
    
    if(MINGW)
        set(CMAKE_CXX_FLAGS "-static-libstdc++ -static-libgcc -DMASTER_RELEASE")
    endif(MINGW)
	
ENDMACRO( MENGE_SET_PROJECT_DEFAULTS project_name )
