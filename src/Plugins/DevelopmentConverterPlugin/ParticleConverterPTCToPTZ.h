#pragma once

#include "DevelopmentConverter.h"

#include "Interface/ArchivatorInterface.h"

namespace Mengine
{
    class ParticleConverterPTCToPTZ
        : public DevelopmentConverter
    {
    public:
        ParticleConverterPTCToPTZ();
        ~ParticleConverterPTCToPTZ() override;

    public:
        bool _initialize() override;

    public:
        bool convert() override;

    protected:
        bool validateVersion( const InputStreamInterfacePtr & _stream ) const override;

    protected:
        ArchivatorInterfacePtr m_archivator;
    };
}
