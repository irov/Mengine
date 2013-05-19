#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#pragma warning(push, 0) 
#	include "magic.h"
#pragma warning(pop) 

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
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		EmitterContainerInterface * createEmitterContainerFromMemory( const void * _buffer ) override;
		void releaseEmitterContainer( EmitterContainerInterface* _containerInterface ) override;

	public:	
		bool flushParticles( const mt::mat4f & _viewMatrix, EmitterInterface * _emitter, ParticleMesh * _meshes, ParticleVertices * _particles, size_t _particlesLimit, EmitterRenderFlush & _flush ) override;

	protected:
		void fillParticles_( ParticleVertices * _particles, size_t _offset, int _count );
		bool loadEmitter( const char * _magicName, HM_FILE _file, AstralaxEmitterContainer * _container );
		void loadEmittersFolder( const char * _path, HM_FILE _file, AstralaxEmitterContainer * _container );

    protected:
        ServiceProviderInterface * m_serviceProvider;
	};
}