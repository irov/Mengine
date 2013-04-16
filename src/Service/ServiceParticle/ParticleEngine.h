#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include	"math/vec3.h"
#	include "math/mat3.h"

#	include "Core/ConstString.h"

namespace Menge
{
	class ParticleEngine
        : public ParticleServiceInterface
	{
	public:
		ParticleEngine();
		~ParticleEngine();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		EmitterContainerInterface * createEmitterContainerFromFile( const ConstString& _fileSystemName, const FilePath & _filename ) override;
		void releaseEmitterContainer( EmitterContainerInterface* _containerInterface );

		//EmitterInterface * createEmitterFromContainer( const ConstString & _name, const EmitterContainerInterface * _container );
		//void releaseEmitter( EmitterInterface * _emitter );

		bool flushEmitter( const mt::mat4f & _viewMatrix, EmitterInterface * _emitter, ParticleMesh * _meshes, ParticleVertices * _particles, size_t _particlesLimit, EmitterRenderFlush & _flush ) override; 

		void getEmitterPosition( EmitterInterface * _emitter, mt::vec2f & _pos );
				
		size_t getMaxParticlesCount() const override;
		void setMaxParticlesCount( size_t _count );

	public:
		void beginParticlesCount() override;
		size_t renderParticlesCount( size_t _count );

	protected:
		ParticleSystemInterface * m_particleSystem;

        ServiceProviderInterface * m_serviceProvider;

		size_t m_maxParticlesNum;
		size_t m_renderParticleNum;
	};
};
