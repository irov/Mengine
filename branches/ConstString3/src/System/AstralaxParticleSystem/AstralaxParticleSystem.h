#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include "magic.h"

#	include "AstralaxEmitter.h"

#	define ASTRALAX_PARTICLE_MAX_TEXTURES 20

namespace Menge
{
	class AstralaxParticleSystem 
		: public ParticleSystemInterface
	{
	public:
		AstralaxParticleSystem();
		~AstralaxParticleSystem();

	public:
		EmitterContainerInterface * createEmitterContainerFromMemory( const void * _buffer ) override;
		void releaseEmitterContainer( EmitterContainerInterface* _containerInterface ) override;

	public:	
		bool flushParticles( EmitterInterface * _emitter, TVectorParticleMeshes & _meshes, TVectorParticleVerices & _particles, size_t _particlesLimit, EmitterRenderFlush & _flush ) override;

	protected:
		void fillParticles_( TVectorParticleVerices & _particles, TVectorParticleVerices::size_type _offset, int _count );
		void loadEmitter( const char * _magicName, HM_FILE _file, AstralaxEmitterContainer * _container );
		void loadEmittersFolder( const char * _path, HM_FILE _file, AstralaxEmitterContainer * _container );
	};
}