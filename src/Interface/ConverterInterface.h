#pragma once

#include "Config/Typedef.h"

#include "Interface/PluginInterface.h"
#include "Interface/StreamInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/Factorable.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    struct ConverterOptions
    {
        FileGroupInterfacePtr fileGroup;

        FilePath inputFileName;
        FilePath outputFileName;

        String params;
    };

    class ConverterInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;

    public:
        virtual void setOptions( ConverterOptions * _options ) = 0;

    public:
        virtual const String & getConvertExt() const = 0;

    public:
        virtual bool validateVersion( const InputStreamInterfacePtr & _stream ) const = 0;

    public:
        virtual bool convert() = 0;
    };

    typedef IntrusivePtr<ConverterInterface> ConverterInterfacePtr;

    class ConverterFactoryInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;

    public:
        virtual ConverterInterfacePtr createConverter() = 0;
    };

    typedef IntrusivePtr<ConverterFactoryInterface> ConverterFactoryInterfacePtr;

    class ConverterServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ConverterService" )

    public:
        virtual bool registerConverter( const ConstString& _type, const ConverterFactoryInterfacePtr & _converter ) = 0;

    public:
        virtual bool unregisterConverter( const ConstString& _type ) = 0;

        virtual ConverterInterfacePtr createConverter( const ConstString & _type ) = 0;

    public:
        virtual bool convert( const ConstString & _converter, const FileGroupInterfacePtr & _category, const FilePath & _in, FilePath & _out ) = 0;
    };

#   define CONVERTER_SERVICE()\
    SERVICE_GET(Mengine::ConverterServiceInterface)
}
