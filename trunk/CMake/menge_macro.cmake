MACRO( MENGE_SET_PROJECT_DEFAULTS project_name )

	# SET LIBNAME
	set(CMAKE_DEBUG_POSTFIX "d")
	set( ${project_name}_LIB_NAME ${project_name}_static )  

	#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)
	if(DEFINED MENGINE_WIN32)		
		set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /Zm1000 /MP2 /EHsc /GR")
		set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /Zi /Ob0 /Od /RTC1")
		set(CMAKE_CXX_FLAGS_RELEASE "/O2 /D NDEBUG")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi /O2 /D NDEBUG")

		set(CMAKE_C_FLAGS "/DWIN32 /D_WINDOWS /W3 /Zm1000")
		set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /Zi /Ob0 /Od /RTC1")
		set(CMAKE_C_FLAGS_RELEASE "/O2 /D NDEBUG")
        set(CMAKE_C_FLAGS_RELWITHDEBINFO "/Zi /O2 /D NDEBUG")
        
		#set(CMAKE_EXE_LINKER_FLAGS "/STACK:10000000 /machine:I386")
		#set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:YES")
		#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO")

        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
        
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
        
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
        set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MT")
        
        set( ${project_name}_LIB_NAME ${${project_name}_LIB_NAME} )

        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_SECURE_SCL=1 -D_HAS_ITERATOR_DEBUGGING=1")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0")
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0")
        
		add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
	endif(DEFINED MENGINE_WIN32)
    
    if(MINGW)
        set(CMAKE_CXX_FLAGS "-static-libstdc++ -static-libgcc")
        add_definitions(-DMENGE_MASTER_RELEASE)
        add_definitions(-D"override = /* */")
    endif(MINGW)
    
    if(APPLE)
        set(CMAKE_CXX_COMPILER clang++)
        set(CMAKE_C_COMPILER clang)

        add_definitions(-DMENGE_MASTER_RELEASE)
    endif(APPLE)
	
ENDMACRO( MENGE_SET_PROJECT_DEFAULTS project_name )


 






