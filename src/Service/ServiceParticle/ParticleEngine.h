#pragma once

#include "Interface/ParticleSystemInterface.h"

#include "Kernel/ServiceBase.h"

#include "math/vec3.h"
#include "math/mat3.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ParticleEngine2
        : public ServiceBase<ParticleServiceInterface2>
    {
    public:
        ParticleEngine2();
        ~ParticleEngine2() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool isAvailable() const override;

    public:
        ParticleEmitterContainerInterface2Ptr createEmitterContainerFromFile( const FileGroupInterfacePtr& _fileGroupName, const FilePath & _fileName, const ConstString & _whoName ) override;

    public:
        uint32_t getMaxParticlesCount() const override;

    protected:
        ArchivatorInterfacePtr m_archivator;

        uint32_t m_maxParticlesNum;

        bool m_available;
    };
};
