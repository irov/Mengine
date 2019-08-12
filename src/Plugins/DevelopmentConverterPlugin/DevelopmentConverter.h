#pragma once

#include "Interface/ConverterInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class DevelopmentConverter
        : public ConverterInterface
        , public Factorable
    {
    public:
        DevelopmentConverter();
        ~DevelopmentConverter() override;

    private:
        bool initialize() override;

    protected:
        virtual bool _initialize() = 0;

    public:
        const ConstString & getConvertExt() const override;

    public:
        void setOptions( ConverterOptions * _options ) override;

    public:
        bool validateVersion( const InputStreamInterfacePtr & _stream ) const override;

    protected:
        FileGroupInterfacePtr m_fileGroup;

        ConverterOptions m_options;

        ConstString m_convertExt;
    };
}
