#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#   include "Config/Blobject.h"

#	include	"math/vec3.h"
#	include "math/mat3.h"

#	include "Core/ConstString.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
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
		bool isAvailable() const override;

	public:
		ParticleEmitterContainerInterface2Ptr createEmitterContainerFromFile( const ConstString& _fileGroupName, const FilePath & _fileName ) override;

	public:
		uint32_t getMaxParticlesCount() const override;		

	protected:
        ServiceProviderInterface * m_serviceProvider;

		ArchivatorInterfacePtr m_archivator;

		uint32_t m_maxParticlesNum;

		bool m_available;
	};
};
