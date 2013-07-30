#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include "AstralaxEmitterContainer.h"
#	include "AstralaxEmitter.h"

#   include "Factory/FactoryPool.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#	endif 

#	include "magic.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#	endif

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
		ParticleEmitterContainerInterface * createEmitterContainerFromMemory( const void * _buffer ) override;
		void releaseEmitterContainer( ParticleEmitterContainerInterface* _containerInterface ) override;

	public:	
		bool flushParticles( const mt::mat4f & _viewMatrix, ParticleEmitterInterface * _emitter, ParticleMesh * _meshes, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush ) override;

	protected:
		void fillParticles_( ParticleVertices * _particles, size_t _offset, int _count );
		bool loadEmitter( const char * _magicName, HM_FILE _file, AstralaxEmitterContainer * _container );
		void loadEmittersFolder( const char * _path, HM_FILE _file, AstralaxEmitterContainer * _container );

    protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef FactoryPool<AstralaxEmitterContainer, 16> TFactoryPoolAstralaxEmitterContainer;
        TFactoryPoolAstralaxEmitterContainer m_factoryPoolAstralaxEmitterContainer;
	};
}