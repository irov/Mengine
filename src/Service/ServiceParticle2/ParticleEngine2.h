#	pragma once

#	include "Interface/ParticleSystemInterface2.h"

#   include "Config/Blobject.h"

#	include	"math/vec3.h"
#	include "math/mat3.h"

#	include "Core/ConstString.h"

namespace Menge
{
	class ParticleEngine2
        : public ParticleServiceInterface2
	{
	public:
		ParticleEngine2();
		~ParticleEngine2();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

    public:
        void update() override;

	public:
		ParticleEmitterContainerInterface2Ptr createEmitterContainerFromFile( const ConstString& _fileGroupName, const FilePath & _fileName ) override;

		bool flushEmitter( const mt::mat4f & _viewMatrix, ParticleEmitterInterface * _emitter, ParticleMesh * _meshes, size_t _meshLimit, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush ) override; 

		void getEmitterPosition( ParticleEmitterInterface * _emitter, mt::vec2f & _pos );
				
		size_t getMaxParticlesCount() const override;
		void setMaxParticlesCount( size_t _count );

	public:		
		size_t renderParticlesCount( size_t _count );

	protected:
        ServiceProviderInterface * m_serviceProvider;

		ArchivatorInterfacePtr m_archivator;

		size_t m_maxParticlesNum;
		size_t m_renderParticleNum;
	};
};
