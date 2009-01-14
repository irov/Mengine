MACRO( MENGINE_ADD_LIBRARY libName )
	
	add_library( ${libName} ${ARGN} )
	
	if(MSVC)
	  SET( CMPLR_FLAGS /M )
	  if(USE_MD)
		SET( CMPLR_FLAGS ${CMPLR_FLAGS}D )
	  else(USE_MD)
		SET( CMPLR_FLAGS ${CMPLR_FLAGS}T )
	  endif(USE_MD)

	  if(DEBUG)
		SET( CMPLR_FLAGS ${CMPLR_FLAGS}d )
	  endif(DEBUG)

	  set_target_properties( ${libName} PROPERTIES COMPILE_FLAGS ${CMPLR_FLAGS} )
	endif(MSVC)

ENDMACRO( MENGINE_ADD_LIBRARY libName )
