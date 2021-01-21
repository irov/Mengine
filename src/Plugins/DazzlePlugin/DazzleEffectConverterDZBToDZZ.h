#pragma once

#include "Interface/ArchivatorInterface.h"
#include "Interface/ConverterInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class DazzleEffectConverterDZBToDZZ
        : public ConverterInterface
        , public Factorable
    {
    public:
        DazzleEffectConverterDZBToDZZ();
        ~DazzleEffectConverterDZBToDZZ() override;

    private:
        bool initialize() override;
        void finalize() override;

    public:
        bool convert() override;

    public:
        const ConstString & getConvertExt() const override;

    public:
        void setOptions( const ConverterOptions * _options ) override;

    protected:
        bool validateVersion( const InputStreamInterfacePtr & _stream ) const override;

    protected:
        ArchivatorInterfacePtr m_archivator;

    protected:
        FileGroupInterfacePtr m_fileGroup;

        ConverterOptions m_options;

        ConstString m_convertExt;
    };
}
