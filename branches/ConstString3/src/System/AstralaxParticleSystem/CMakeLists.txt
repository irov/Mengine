
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
 SET( magic_lib_path ${magic_lib_path}/win/lib )
elseif( APPLE )
 #if(DEFINED MENGINE_IPAD)
  SET( magic_lib_path ${magic_lib_path}/iphone )
 #endif(DEFINED MENGINE_IPAD)

 if(DEFINED MENGINE_MACOS)
  SET( magic_lib_path ${magic_lib_path}/win/lib/mingw )
 endif(DEFINED MENGINE_MACOS)
endif( WIN32 )

if( MINGW )
 SET( magic_lib_path ${magic_lib_path}/mingw )
elseif( MSVC )
 SET( magic_lib_path ${magic_lib_path}/microsoft )
endif( MINGW )

SET( magic_lib_path ${THIRDPARTY_DIR}/astralax${magic_lib_path} )
MESSAGE( ${magic_lib_path} )

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 ${magic_lib_path}
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
 #if(DEFINED MENGINE_IPAD)
	SET( APPLICATION_LIBS
	${APPLICATION_LIBS}
	${magic_lib_path}/libimagic_simulator.a
	)
 #endif(DEFINED MENGINE_IPAD)
endif( WIN32 )


SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)
