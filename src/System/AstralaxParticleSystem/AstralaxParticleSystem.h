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
	Menge::EmitterInterface * createEmitterFromContainer( const Menge::String & _name, const Menge::EmitterContainerInterface * _container ) override;

public:
	void releaseEmitter( Menge::EmitterInterface * _emitter ) override;
	bool lockEmitter( Menge::EmitterInterface * _emitter, int _typeParticle ) override;
	int getTextureCount() const override;
	Menge::String getTextureName( int _index ) const override;
	int flushParticles( Menge::TVectorRenderParticle & _particles, int _particlesLimit ) override;
	bool flushParticle( Menge::RenderParticle & _particle ) override;
	void unlockEmitter( Menge::EmitterInterface * _emitter ) override;

private:
	MAGIC_TEXTURE* m_texture[ASTRALAX_PARTICLE_MAX_TEXTURES];
	int m_textureCount;
};
