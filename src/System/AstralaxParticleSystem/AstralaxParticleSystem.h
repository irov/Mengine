#	pragma once

#	include "Interface\ParticleSystemInterface.h"

#	include "magic.h"

#	include "AstralaxEmitter.h"

#	define MAX_TEXTURES 20

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
	MAGIC_TEXTURE m_texture[MAX_TEXTURES];
	int m_currentX;
	int m_currentY;

};

bool	initInterfaceSystem(ParticleSystemInterface** );
void	releaseInterfaceSystem(ParticleSystemInterface* );