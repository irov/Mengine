#pragma once

#include "Interface/ArchivatorInterface.h"

#include "Kernel/DevelopmentConverter.h"

namespace Mengine
{
    class DazzleEffectConverterDZBToDZZ
        : public DevelopmentConverter
    {
    public:
        DazzleEffectConverterDZBToDZZ();
        ~DazzleEffectConverterDZBToDZZ() override;

    private:
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
