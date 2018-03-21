#pragma once

#include "Interface/ParticleSystemInterface.h"

#   include "Core/ServiceBase.h"

#include	"math/vec3.h"
#include "math/mat3.h"

#include "Core/ConstString.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class ParticleEngine2
        : public ServiceBase<ParticleServiceInterface2>
	{
	public:
		ParticleEngine2();
		~ParticleEngine2();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool isAvailable() const override;

	public:
		ParticleEmitterContainerInterface2Ptr createEmitterContainerFromFile( const ConstString& _fileGroupName, const FilePath & _fileName ) override;

	public:
		uint32_t getMaxParticlesCount() const override;		

	protected:
		ArchivatorInterfacePtr m_archivator;

		uint32_t m_maxParticlesNum;

		bool m_available;
	};
};
