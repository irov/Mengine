
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

ADD_LIBRARY( AstralaxParticleSystem ${SRC_FILES} )

#SET( APPLICATION_DEPENDENCIES
# ${APPLICATION_DEPENDENCIES}
# AstralaxParticleSystem
#)

#SET( APPLICATION_DEPENDENCIES
# ${APPLICATION_DEPENDENCIES}
# PARENT_SCOPE
#)