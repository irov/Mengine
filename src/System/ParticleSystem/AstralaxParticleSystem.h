#	pragma once

#	include "Interface\ParticleSystemInterface.h"

#	include "magic.h"

#	include "AstralaxEmitter.h"

class ParticleSystem : public ParticleSystemInterface
{
public:
	ParticleSystem();
	~ParticleSystem();
public:
	EmitterContainerInterface * createEmitterContainerFromMemory( void * _buffer );
	EmitterInterface * createEmitterFromContainer( const char * _name, const EmitterContainerInterface * _container );
public:
	void lockEmitter( EmitterInterface * _emitter, int _typeParticle );
	RenderParticle * nextParticle();
	void unlockEmitter( EmitterInterface * _emitter );
};

bool	initInterfaceSystem(ParticleSystemInterface** );
void	releaseInterfaceSystem(ParticleSystemInterface* );