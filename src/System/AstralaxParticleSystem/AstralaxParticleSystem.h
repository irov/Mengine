#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include "magic.h"

#	include "AstralaxEmitter.h"

#	define MAX_TEXTURES 20

class AstralaxParticleSystem 
	: public Menge::ParticleSystemInterface
{
public:
	AstralaxParticleSystem();
	~AstralaxParticleSystem();

public:
	Menge::EmitterContainerInterface * createEmitterContainerFromMemory( void * _buffer );
	void releaseEmitterContainer( Menge::EmitterContainerInterface* _containerInterface ) override;
	Menge::EmitterInterface * createEmitterFromContainer( const Menge::String & _name, const Menge::EmitterContainerInterface * _container );
public:
	void releaseEmitter( Menge::EmitterInterface * _emitter );
	void lockEmitter( Menge::EmitterInterface * _emitter, int _typeParticle );
	Menge::String getTextureName() const;
	Menge::RenderParticle * nextParticle();
	void unlockEmitter( Menge::EmitterInterface * _emitter );
private:
	MAGIC_TEXTURE m_texture[MAX_TEXTURES];
	float m_currentX;
	float m_currentY;

};
