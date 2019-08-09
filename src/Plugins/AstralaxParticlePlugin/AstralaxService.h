#pragma once

#include "AstralaxInterface.h"

#include "Kernel/ServiceBase.h"

#include "math/vec3.h"
#include "math/mat3.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AstralaxService
        : public ServiceBase<AstralaxServiceInterface>
    {
    public:
        AstralaxService();
        ~AstralaxService() override;

    public:
        bool _availableService() const override;
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        AstralaxEmitterContainerInterfacePtr createEmitterContainerFromFile( const FileGroupInterfacePtr & _fileGroupName, const FilePath & _fileName, const Char * _doc ) override;

    public:
        uint32_t getMaxParticlesCount() const override;

    protected:
        ArchivatorInterfacePtr m_archivator;

        uint32_t m_maxParticlesNum;
    };
};
