#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#   include "Config/Blobject.h"

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
        void update() override;

	public:
		ParticleEmitterContainerInterfacePtr createEmitterContainerFromFile( const ConstString& _fileSystemName, const FilePath & _filename ) override;

		bool flushEmitter( const mt::mat4f & _viewMatrix, ParticleEmitterInterface * _emitter, ParticleMesh * _meshes, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush ) override; 

		void getEmitterPosition( ParticleEmitterInterface * _emitter, mt::vec2f & _pos );
				
		size_t getMaxParticlesCount() const override;
		void setMaxParticlesCount( size_t _count );

	public:		
		size_t renderParticlesCount( size_t _count );

	protected:
        ServiceProviderInterface * m_serviceProvider;

		size_t m_maxParticlesNum;
		size_t m_renderParticleNum;
	};
};
