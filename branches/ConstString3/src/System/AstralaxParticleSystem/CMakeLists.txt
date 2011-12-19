
set( MY_LIB_NAME AstralaxParticleSystem )

ADD_FILTER(
src
	AstralaxEmitter.cpp
	AstralaxEmitter.h
	AstralaxEmitterContainer.cpp
	AstralaxEmitterContainer.h
	AstralaxParticleSystem.cpp
	AstralaxParticleSystem.h
	AstralaxParticleSystem.def
)

INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/astralax )

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( magic_lib_path )

if( WIN32 )
 SET( magic_lib_path ${magic_lib_path}win/lib/ )
elseif( APPLE )
 SET( magic_lib_path ${magic_lib_path}win/lib/mingw/ )
endif( WIN32 )

if( MINGW )
 SET( magic_lib_path ${magic_lib_path}mingw/ )
elseif( MSVC )
 SET( magic_lib_path ${magic_lib_path}microsoft/ )
endif( MINGW )


SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 ${THIRDPARTY_DIR}/Astralax/${magic_lib_path}
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)

if( WIN32 )
	SET( APPLICATION_LIBS
	${APPLICATION_LIBS}
	magic${MENGINE_LIB_POSTFIX}
	)
elseif( APPLE )
	#SET( APPLICATION_LIBS
	#${APPLICATION_LIBS}
	#magic${MENGINE_LIB_POSTFIX}
	#)

endif( WIN32 )


SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)
