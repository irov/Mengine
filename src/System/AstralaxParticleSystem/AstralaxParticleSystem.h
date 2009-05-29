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
	void lockEmitter( Menge::EmitterInterface * _emitter, int _typeParticle ) override;
	Menge::String getTextureName() const override;
	void flushParticles( Menge::TVectorRenderParticle & _particles ) override; 
	void unlockEmitter( Menge::EmitterInterface * _emitter ) override;

private:
	MAGIC_TEXTURE* m_texture[ASTRALAX_PARTICLE_MAX_TEXTURES];
};
