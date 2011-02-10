#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include "magic.h"

#	include "AstralaxEmitter.h"

#	define ASTRALAX_PARTICLE_MAX_TEXTURES 20

class AstralaxParticleSystem 
	: public Menge::ParticleSystemInterface
{
public:
	AstralaxParticleSystem();
	~AstralaxParticleSystem();

public:
	Menge::EmitterContainerInterface * createEmitterContainerFromMemory( void * _buffer ) override;
	void releaseEmitterContainer( Menge::EmitterContainerInterface* _containerInterface ) override;

public:
	Menge::EmitterInterface * createEmitterFromContainer( const Menge::String & _name, const Menge::EmitterContainerInterface * _container ) override;
	void releaseEmitter( Menge::EmitterInterface * _emitter ) override;

	void getEmitterPosition( Menge::EmitterInterface * _emitter, mt::vec2f & _pos ) override;

public:	
	bool lockEmitter( Menge::EmitterInterface * _emitter, int _typeParticle ) override;
	void unlockEmitter( Menge::EmitterInterface * _emitter ) override;

	int getTextureCount() const override;
	const char * getTextureName( int _index ) const override;
	int flushParticles( Menge::TVectorRenderParticle & _particles, int _particlesLimit ) override;

private:
	MAGIC_TEXTURE* m_texture[ASTRALAX_PARTICLE_MAX_TEXTURES];
	int m_textureCount;
};
