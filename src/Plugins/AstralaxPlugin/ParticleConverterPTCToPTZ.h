#pragma once

#include "Kernel/DevelopmentConverter.h"

#include "Interface/ArchivatorInterface.h"

namespace Mengine
{
    class ParticleConverterPTCToPTZ
        : public DevelopmentConverter
    {
        DECLARE_FACTORABLE( ParticleConverterPTCToPTZ );

    public:
        ParticleConverterPTCToPTZ();
        ~ParticleConverterPTCToPTZ() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool convert() override;

    protected:
        bool validateVersion( const InputStreamInterfacePtr & _stream ) const override;

    protected:
        ArchivatorInterfacePtr m_archivator;
    };
}
