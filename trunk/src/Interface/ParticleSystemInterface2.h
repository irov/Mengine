#	pragma once

#   include "Interface/ParticleSystemInterface.h"

namespace Menge
{
	class ParticleEmitterContainerInterface2
        : public FactorablePtr
	{
	public:
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() const = 0;

    public:
        virtual bool initialize( const ConstString & _name, const InputStreamInterfacePtr & _stream ) = 0;
		virtual void finalize() = 0;

    public:
        virtual bool isValid() const = 0;

    public:
		virtual ParticleEmitterInterfacePtr createEmitter( const ConstString & _name ) = 0;
	};

    typedef stdex::intrusive_ptr<ParticleEmitterContainerInterface2> ParticleEmitterContainerInterface2Ptr;

	class ParticleSystemInterface2
		: public ServiceInterface
	{
		SERVICE_DECLARE("ParticleSystem2")

	public:
		virtual ParticleEmitterContainerInterface2Ptr createEmitterContainerFromMemory( const ConstString & _name, const InputStreamInterfacePtr & _stream ) = 0;

	public:
		virtual bool flushParticles( const mt::mat4f & _viewMatrix, ParticleEmitterInterface * _emitter, ParticleMesh * _meshes, size_t _meshLimit, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush ) = 0;
	};

#   define PARTICLE_SYSTEM2( serviceProvider )\
	SERVICE_GET(serviceProvider, Menge::ParticleSystemInterface2)

	class ParticleServiceInterface2
		: public ServiceInterface
	{
		SERVICE_DECLARE("ParticleService2")

	public:
		virtual bool flushEmitter( const mt::mat4f & _viewMatrix, ParticleEmitterInterface * _emitter, ParticleMesh * _meshes, size_t _meshLimit, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush ) = 0;
		virtual size_t renderParticlesCount( size_t _count ) = 0;

	public:
		virtual ParticleEmitterContainerInterface2Ptr createEmitterContainerFromFile( const ConstString& _fileGroupName, const FilePath & _fileName ) = 0;

	public:
		virtual size_t getMaxParticlesCount() const = 0;

	public:
		virtual void update() = 0;
	};

#   define PARTICLE_SERVICE2( serviceProvider )\
	SERVICE_GET(serviceProvider, Menge::ParticleServiceInterface2)
}
