#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include "Config/String.h"

#	include "AstralaxEmitterContainer.h"
#	include "AstralaxEmitter.h"

#   include "Factory/FactoryStore.h"

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
		ParticleEmitterContainerInterfacePtr createEmitterContainerFromMemory( const ConstString & _name, const void * _buffer ) override;

	public:	
		bool flushParticles( const mt::mat4f & _viewMatrix, ParticleEmitterInterface * _emitter, ParticleMesh * _meshes, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush ) override;

	protected:
		void fillParticles_( ParticleVertices * _particles, size_t _offset, size_t _count );
		bool loadEmitter( const char * _magicName, HM_FILE _file, const AstralaxEmitterContainerPtr & _container );
		bool loadEmittersFolder( const char * _path, HM_FILE _file, const AstralaxEmitterContainerPtr & _container );

	protected:
		bool loadEmitters_( HM_FILE _file, const AstralaxEmitterContainerPtr & _container );

    protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef FactoryPoolStore<AstralaxEmitterContainer, 16> TFactoryPoolAstralaxEmitterContainer;
        TFactoryPoolAstralaxEmitterContainer m_factoryPoolAstralaxEmitterContainer;

		String m_loadEmitterCacheFullname;
		String m_loadEmitterCacheName;
	};
}