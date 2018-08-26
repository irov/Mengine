#pragma once

#include "DevelopmentConverter.h"

#include "Interface/ArchiveInterface.h"

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
        bool convert_( const FilePath & _input, const FilePath & _output );

    protected:
        bool validateVersion( const InputStreamInterfacePtr & _stream ) const override;

    protected:
        ArchivatorInterfacePtr m_archivator;
    };
}
