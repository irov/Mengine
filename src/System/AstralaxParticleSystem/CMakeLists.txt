
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

ADD_LIBRARY( AstralaxParticleSystem ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 AstralaxParticleSystem
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 ${THIRDPARTY_DIR}/Astralax/${MENGINE_LIB_DIR}
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 debug ${MENGINE_LIB_PREFIX}magic_d${MENGINE_LIB_SUFFIX}
 optimized ${MENGINE_LIB_PREFIX}magic${MENGINE_LIB_SUFFIX}
)


SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)
