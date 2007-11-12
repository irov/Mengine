#	pragma once

#	include "Interface\ParticleSystemInterface.h"

#	include "magic.h"

#	include "AstralaxEmitter.h"

class AstralaxParticleSystem : public ParticleSystemInterface
{
public:
	AstralaxParticleSystem();
	~AstralaxParticleSystem();
public:
	EmitterContainerInterface * createEmitterContainerFromMemory( void * _buffer );
	EmitterInterface * createEmitterFromContainer( const char * _name, const EmitterContainerInterface * _container );
public:
	void releaseEmitter( EmitterInterface * _emitter );
	void lockEmitter( EmitterInterface * _emitter, int _typeParticle );
	const char * getTextureName() const;
	RenderParticle * nextParticle();
	void unlockEmitter( EmitterInterface * _emitter );
private:
	MAGIC_TEXTURE m_texture;
};

bool	initInterfaceSystem(ParticleSystemInterface** );
void	releaseInterfaceSystem(ParticleSystemInterface* );