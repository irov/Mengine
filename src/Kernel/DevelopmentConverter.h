#pragma once

#include "Interface/ConverterInterface.h"

namespace Mengine
{
    class DevelopmentConverter
        : public ConverterInterface
    {
    public:
        DevelopmentConverter();
        ~DevelopmentConverter() override;

    private:
        bool initialize() override;
        void finalize() override;

    protected:
        virtual bool _initialize() = 0;
        virtual void _finalize() = 0;

    public:
        void setOptions( const ConverterOptions & _options ) override;
        const ConverterOptions & getOptions() const override;

    public:
        const ConstString & getConvertExt() const override;

    public:
        bool validateVersion( const InputStreamInterfacePtr & _stream ) const override;

    protected:
        const FileGroupInterfacePtr & getDevFileGroup() const;

    protected:
        FileGroupInterfacePtr m_devFileGroup;

        ConverterOptions m_options;

        ConstString m_convertExt;
    };
}
