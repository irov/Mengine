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
		EmitterInterface * createEmitterFromContainer( const String & _name, const EmitterContainerInterface * _container ) override;
		void releaseEmitter( EmitterInterface * _emitter ) override;

		void getEmitterPosition( EmitterInterface * _emitter, mt::vec2f & _pos ) override;

	public:	
		bool flushParticles( EmitterInterface * _emitter, TVectorParticleMeshes & _meshes, TVectorParticleVerices & _particles, int _particlesLimit ) override;

	protected:
		void fillParticles_( TVectorParticleVerices & _particles, TVectorParticleVerices::size_type _offset, int _count );
	};
}