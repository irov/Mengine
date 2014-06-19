#	pragma once

#   include "Interface/ParticleSystemInterface.h"
#	include "Interface/ArchiveInterface.h"

#	include "Core/Magic.h"

namespace Menge
{
	DECLARE_MAGIC_NUMBER( MAGIC_PTZ, 'P', 'T', 'Z', '2', 1 );

	class ParticleEmitterContainerInterface2
        : public FactorablePtr
	{
	public:
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() const = 0;

    public:
        virtual bool initialize( const ConstString & _name, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator ) = 0;
		virtual void finalize() = 0;

    public:
        virtual bool isValid() const = 0;

    public:
		virtual ParticleEmitterInterfacePtr createEmitter() = 0;
	};

    typedef stdex::intrusive_ptr<ParticleEmitterContainerInterface2> ParticleEmitterContainerInterface2Ptr;

	class ParticleSystemInterface2
		: public ServiceInterface
	{
		SERVICE_DECLARE("ParticleSystem2")

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual ParticleEmitterContainerInterface2Ptr createEmitterContainerFromMemory( const ConstString & _name, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator ) = 0;
	};

#   define PARTICLE_SYSTEM2( serviceProvider )\
	SERVICE_GET(serviceProvider, Menge::ParticleSystemInterface2)

	class ParticleServiceInterface2
		: public ServiceInterface
	{
		SERVICE_DECLARE("ParticleService2")

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
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
